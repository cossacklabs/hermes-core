/*
 * Copyright (c) 2016 Cossack Labs Limited
 *
 * This file is part of Hermes.
 *
 * Hermes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hermes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Hermes.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <themis/themis.h>

#include <hermes/utils.h>

#include <hermes/thread_pool.h>
#include <hermes/connection_manager.h>

#define COMMAND_WORKER_READY 1
#define COMMAND_WORKER_FINISHED 2
#define COMMAND_WORKER_QUIT 3
#define COMMAND_BIND 4
#define COMMAND_QUIT 5
#define COMMAND_REQUEST 6
#define COMMAND_RUN 7
#define COMMAND_CONNECT 8
#define COMMAND_REPLY 9

const uint8_t bad_data[8]="bad data";
  
struct connection_manager_t{
  zctx_t* zctx_;
  char frontend_endpoint_[256];
  thread_pool_t* worker_threads_;
  void* frontend_socket_;
  void* brocker_socket_;
  uint64_t workers_count_;
  char** worker_identities_;
  uint64_t worker_identities_count_;
  uint64_t current_worker_;
  uint64_t binded_;
  void* server_socket_;
  server_function_t server_function_;
  void* server_data_;  
  void* client_socket_;
  int stop_;
};

void worker_function(void* param){
  HERMES_CHECK(param, return);
  connection_manager_t* manager=(connection_manager_t*)param;
  void* socket=zsocket_new(manager->zctx_, ZMQ_DEALER);
  char identity [10];
  sprintf (identity, "%16X", socket);
  zsocket_set_identity (socket, identity);
  HERMES_CHECK(socket, return);
  HERMES_CHECK(0==zsocket_connect(socket, manager->frontend_endpoint_), zsocket_destroy(manager->zctx_, socket); return);
  char command=COMMAND_WORKER_READY;
  HERMES_CHECK(0==zsocket_sendmem(socket, &command, 1, 0), zsocket_destroy(manager->zctx_, socket); return);
  int quit=1;
  while(quit){
    if(zsocket_poll(socket, 10*ZMQ_POLL_MSEC)){
      zmsg_t *msg = zmsg_recv(socket);
      zframe_t *command_frame = zmsg_pop(msg);    
      HERMES_CHECK(zframe_size(command_frame)==1, continue);
      switch (zframe_data(command_frame)[0]){
      case COMMAND_WORKER_QUIT:
	zmsg_destroy(&msg);
	quit=0;
	break;
      case COMMAND_RUN:{
	zframe_t* client_id=zmsg_first(msg);
	HERMES_CHECK(client_id, continue);
	zframe_t* request=zmsg_last(msg);
	HERMES_CHECK(request, continue);
	zmsg_remove(msg, request);
	uint8_t* response=NULL;
	size_t response_length=0;
	manager->server_function_(manager->server_data_, strchr(zframe_data(client_id), '.')+1, zframe_data(request), zframe_size(request), &response, &response_length);
	HERMES_CHECK(0==zmsg_addmem(msg, (response)?response:bad_data, response_length), continue);
	zmsg_send(&msg, manager->server_socket_);
	zframe_destroy(&request);
	//	if(response)free(response);
      }
	break;
      case COMMAND_REPLY:{
	HERMES_CHECK(zmsg_size(msg)==3, zmsg_destroy(&msg); continue);
	zframe_t* handler_pointer = zmsg_pop(msg);
	HERMES_CHECK(handler_pointer, continue);
	HERMES_CHECK(zframe_size(handler_pointer)==sizeof(response_handler_t), zframe_destroy(&handler_pointer);continue);
	response_handler_t response_handler=(response_handler_t)*((uint64_t*)(zframe_data(handler_pointer)));
	zframe_destroy(&handler_pointer);
	zframe_t* user_data_pointer = zmsg_pop(msg);
	HERMES_CHECK(user_data_pointer, continue);
	HERMES_CHECK(zframe_size(user_data_pointer)==sizeof(void*), zframe_destroy(&user_data_pointer);continue);
	void* user_data=(void*)*((uint64_t*)(zframe_data(user_data_pointer)));
	zframe_destroy(&user_data_pointer);
	zframe_t* response = zmsg_pop(msg);
	HERMES_CHECK(response, continue);
	response_handler(user_data, zframe_data(response), zframe_size(response));
      }
	break;
      default:
	continue;
      }
      zframe_destroy(&command_frame);
    }
  }
  zsocket_destroy(manager->zctx_, socket);
}

void stop_all_workers(connection_manager_t* manager){
  int i=0;
  for(;i<manager->worker_identities_count_; ++i){
    HERMES_CHECK(0==zsocket_sendmem(manager->frontend_socket_, manager->worker_identities_[i], strlen(manager->worker_identities_[i]), ZFRAME_MORE), continue);
    char command=COMMAND_WORKER_QUIT;
    HERMES_CHECK(0==zsocket_sendmem(manager->frontend_socket_, &command, 1, 0), continue);
  }
}

void server_thread(void* param){
  HERMES_CHECK(param, return);
  connection_manager_t* manager=(connection_manager_t*)param;
  int quit=1;
  while(quit && !(manager->stop_)){
    if(zsocket_poll(manager->server_socket_, 10*ZMQ_POLL_MSEC)){
      zmsg_t* msg=zmsg_recv(manager->server_socket_);
      HERMES_CHECK(msg, continue);
      char command=COMMAND_RUN;
      HERMES_CHECK(0==zmsg_pushmem(msg, &command, 1), continue);
      HERMES_CHECK(0==zmsg_pushstr(msg, manager->worker_identities_[manager->current_worker_]), continue);
      zmsg_send(&msg, manager->frontend_socket_);
      ++manager->current_worker_;
      if(manager->current_worker_==manager->worker_identities_count_){
	manager->current_worker_=0;
      }
    }
  }
  zsocket_destroy(manager->zctx_, manager->server_socket_);
}

void client_function(void* param){
  HERMES_CHECK(param, return);
  connection_manager_t* manager=(connection_manager_t*)param;
  int quit=1;
  while(quit && !(manager->stop_)){
    if(zsocket_poll(manager->client_socket_, 10*ZMQ_POLL_MSEC)){
      zmsg_t* msg=zmsg_recv(manager->client_socket_);
      HERMES_CHECK(msg, continue);
      char command=COMMAND_REPLY;
      HERMES_CHECK(0==zmsg_pushmem(msg, &command, 1), continue);
      HERMES_CHECK(0==zmsg_pushstr(msg, manager->worker_identities_[manager->current_worker_]), continue);
      zmsg_send(&msg, manager->frontend_socket_);
      ++manager->current_worker_;
      if(manager->current_worker_==manager->worker_identities_count_){
	manager->current_worker_=0;
      }
    }
  }
  zsocket_destroy(manager->zctx_, manager->client_socket_);  
}

void broker_function(void* param){
  HERMES_CHECK(param, return);
  connection_manager_t* manager=(connection_manager_t*)param;
  /*wait for workers ready*/
  int i=0;
  for(;i<manager->workers_count_; ++i){
    zmsg_t* msg=zmsg_recv(manager->frontend_socket_);
    HERMES_CHECK(msg, continue);
    manager->worker_identities_[manager->worker_identities_count_]=zmsg_popstr(msg);
    HERMES_CHECK(manager->worker_identities_[manager->worker_identities_count_], continue);
    zframe_t* command_frame=zmsg_pop(msg);
    HERMES_CHECK(zframe_size(command_frame)==1 && zframe_data(command_frame)[0]==COMMAND_WORKER_READY, continue);
    ++manager->worker_identities_count_;
  }
  HERMES_CHECK(i==manager->workers_count_, stop_all_workers(manager); );
  /*TODO set signal that all threads ready*/
  int quit=1;
  while(quit){
    int res;
    if(zsocket_poll(manager->frontend_socket_, 10*ZMQ_POLL_MSEC)){
      zmsg_t* msg=zmsg_recv(manager->frontend_socket_);
      HERMES_CHECK(msg, continue);
      char* sender=zmsg_popstr(msg);
      HERMES_CHECK(sender, continue);
      zframe_t* command_frame=zmsg_pop(msg);
      HERMES_CHECK(zframe_size(command_frame)==1, continue);
      switch((zframe_data(command_frame)[0])){
      case COMMAND_BIND:{
	char *endpoint;
	endpoint=zmsg_popstr(msg);
	manager->server_socket_=zsocket_new(manager->zctx_, ZMQ_ROUTER);
	HERMES_CHECK(manager->server_socket_, return);
	int linger_ms=0;
	HERMES_CHECK(0==zmq_setsockopt(manager->server_socket_, ZMQ_LINGER, &linger_ms, sizeof(linger_ms)), continue);
	HERMES_CHECK(0<zsocket_bind(manager->server_socket_, endpoint), continue);
	HERMES_CHECK(THREAD_POOL_SUCCESS==thread_pool_add(manager->worker_threads_, server_thread, (void*)manager), continue);
	HERMES_CHECK(0==zsocket_sendmem(manager->frontend_socket_, sender, strlen(sender), ZFRAME_MORE), continue);
	HERMES_CHECK(0==zsocket_sendmem(manager->frontend_socket_, NULL, 0, 0), continue);
	free(endpoint);
	zmsg_destroy(&msg);
      }
	break;
      case COMMAND_CONNECT:{
	char *endpoint;
	endpoint=zmsg_popstr(msg);
	manager->client_socket_=zsocket_new(manager->zctx_, ZMQ_DEALER);
	HERMES_CHECK(manager->client_socket_, return);
	int linger_ms=0;
	HERMES_CHECK(0==zmq_setsockopt(manager->client_socket_, ZMQ_LINGER, &linger_ms, sizeof(linger_ms)), continue);
	char identity[256];
	sprintf (identity, "%08X.%08X", randof (0x100000000), randof (0x100000000));
	zsocket_set_identity (manager->client_socket_, identity);
	HERMES_CHECK(0==zsocket_connect(manager->client_socket_, endpoint), continue);
	HERMES_CHECK(THREAD_POOL_SUCCESS==thread_pool_add(manager->worker_threads_, client_function, (void*)manager), continue);
	HERMES_CHECK(0==zsocket_sendmem(manager->frontend_socket_, sender, strlen(sender), ZFRAME_MORE), continue);
	HERMES_CHECK(0==zsocket_sendmem(manager->frontend_socket_, NULL, 0, 0), continue);
	free(endpoint);
	zmsg_destroy(&msg);
      }
	break;
      case COMMAND_QUIT:
	stop_all_workers(manager);
	quit=0;
	zmsg_destroy(&msg);
	break;
      case COMMAND_REQUEST:
	HERMES_CHECK(0==zmsg_send(&msg, manager->client_socket_), continue);
	break;
      case COMMAND_WORKER_FINISHED:
      default:
	zmsg_destroy(&msg);
	break;
      }
      zframe_destroy(&command_frame);
    }
  }
  zsocket_destroy(manager->zctx_, manager->frontend_socket_);
}

connection_manager_t* connection_manager_create(uint64_t threads_count){
  connection_manager_t* manager=malloc(sizeof(connection_manager_t));
  HERMES_CHECK(manager, return NULL);
  manager->zctx_=zctx_new();
  HERMES_CHECK(manager->zctx_, return NULL);  
  manager->workers_count_=threads_count;
  manager->server_socket_=NULL;
  manager->server_function_=NULL;
  manager->server_data_=NULL;  
  manager->stop_=0;
  manager->worker_identities_=malloc(threads_count*sizeof(char*));
  HERMES_CHECK(manager->worker_identities_, connection_manager_destroy(manager); return NULL);
  manager->worker_identities_count_=0;
  sprintf(manager->frontend_endpoint_, "inproc://%i.cm.frontend", (uint64_t)manager);
  manager->frontend_socket_ = zsocket_new(manager->zctx_, ZMQ_ROUTER);
  HERMES_CHECK(manager->frontend_socket_, connection_manager_destroy(manager); return NULL);
  int linger_ms=0;
  HERMES_CHECK(0==zmq_setsockopt(manager->frontend_socket_, ZMQ_LINGER, &linger_ms, sizeof(linger_ms)), connection_manager_destroy(manager); return NULL);
  HERMES_CHECK(0==zsocket_bind(manager->frontend_socket_, manager->frontend_endpoint_), connection_manager_destroy(manager); return NULL);
  /*threads_count worker threads + 1 brocker_thread + 1 server thread */
  manager->worker_threads_=thread_pool_create(threads_count+2, threads_count*2);
  HERMES_CHECK(manager->worker_threads_, connection_manager_destroy(manager); return NULL);
  int i=0;
  for(;i<threads_count; ++i){
    HERMES_CHECK(THREAD_POOL_SUCCESS==thread_pool_add(manager->worker_threads_, worker_function, (void*)manager), connection_manager_destroy(manager);return NULL);
  }
  manager->current_worker_=0;
  HERMES_CHECK(THREAD_POOL_SUCCESS==thread_pool_add(manager->worker_threads_, broker_function, (void*)manager), connection_manager_destroy(manager);return NULL);
  /*TODO: add signal to check if brocker thread started successfull*/
  sleep(1);
  manager->brocker_socket_ = zsocket_new(manager->zctx_, ZMQ_DEALER);
  HERMES_CHECK(manager->brocker_socket_, connection_manager_destroy(manager); return NULL);
  char identity[256];
  sprintf (identity, "%16X", manager->brocker_socket_);
  zsocket_set_identity (manager->brocker_socket_, identity);
  HERMES_CHECK(0==zsocket_connect(manager->brocker_socket_, manager->frontend_endpoint_), connection_manager_destroy(manager);return NULL);  
  return manager;
}

connection_manager_status_t connection_manager_bind(connection_manager_t* manager, server_function_t server_function, void* user_data){
  manager->server_function_=server_function;
  manager->server_data_=user_data;  
  char command=COMMAND_BIND;
  HERMES_CHECK(0==zsocket_sendmem(manager->brocker_socket_, &command, 1, ZFRAME_MORE), return CONNECTION_MANAGER_FAIL);
  HERMES_CHECK(0==zsocket_sendmem(manager->brocker_socket_, "", 0, 0), return CONNECTION_MANAGER_FAIL);
  zmsg_t* msg=zmsg_recv(manager->brocker_socket_);
  return CONNECTION_MANAGER_SUCCESS;  
}

connection_manager_status_t connection_manager_connect(connection_manager_t* manager, const char* endpoint, const char* id){
  manager->client_socket_=zsocket_new(manager->zctx_, ZMQ_DEALER);
  HERMES_CHECK(manager->client_socket_, return CONNECTION_MANAGER_FAIL);
  int linger_ms=0;
  HERMES_CHECK(0==zmq_setsockopt(manager->client_socket_, ZMQ_LINGER, &linger_ms, sizeof(linger_ms)), zsocket_destroy(manager->zctx_,  manager->client_socket_); return CONNECTION_MANAGER_FAIL);
  uint32_t r;
  HERMES_CHECK(SOTER_SUCCESS==soter_rand((void*)&r, sizeof(uint32_t)), zsocket_destroy(manager->zctx_,  manager->client_socket_); return CONNECTION_MANAGER_FAIL);
  char identity[256];
  sprintf (identity, "%08X.%s", r, id);
  zsocket_set_identity (manager->client_socket_, identity);
  HERMES_CHECK(0==zsocket_connect(manager->client_socket_, endpoint), zsocket_destroy(manager->zctx_,  manager->client_socket_); return CONNECTION_MANAGER_FAIL);
  HERMES_CHECK(THREAD_POOL_SUCCESS==thread_pool_add(manager->worker_threads_, client_function, (void*)manager), zsocket_destroy(manager->zctx_,  manager->client_socket_); return CONNECTION_MANAGER_FAIL);
  return CONNECTION_MANAGER_SUCCESS;  
}

connection_manager_status_t connection_manager_sync_connect(connection_manager_t* manager, const char* endpoint, const char* id){
  manager->client_socket_=zsocket_new(manager->zctx_, ZMQ_DEALER);
  HERMES_CHECK(manager->client_socket_, return CONNECTION_MANAGER_FAIL);
  int linger_ms=0;
  HERMES_CHECK(0==zmq_setsockopt(manager->client_socket_, ZMQ_LINGER, &linger_ms, sizeof(linger_ms)), zsocket_destroy(manager->zctx_,  manager->client_socket_); return CONNECTION_MANAGER_FAIL);
  uint32_t r;
  HERMES_CHECK(SOTER_SUCCESS==soter_rand((void*)&r, sizeof(uint32_t)), zsocket_destroy(manager->zctx_,  manager->client_socket_); return CONNECTION_MANAGER_FAIL);
  char identity[256];
  sprintf (identity, "%08X.%s", r, id);
  zsocket_set_identity (manager->client_socket_, identity);
  HERMES_CHECK(0==zsocket_connect(manager->client_socket_, endpoint), zsocket_destroy(manager->zctx_,  manager->client_socket_); return CONNECTION_MANAGER_FAIL);
  return CONNECTION_MANAGER_SUCCESS;  
}


connection_manager_status_t connection_manager_send_request(connection_manager_t* manager, const uint8_t* request, const size_t request_length, response_handler_t response_handler, void* user_data){
  char command=COMMAND_REQUEST;
  HERMES_CHECK(0==zsocket_sendmem(manager->brocker_socket_, &command, 1, ZFRAME_MORE), return CONNECTION_MANAGER_FAIL);
  char request_id[256];
  HERMES_CHECK(response_handler && 0==zsocket_sendmem(manager->brocker_socket_, (char*)(&response_handler), sizeof(response_handler), ZFRAME_MORE), return CONNECTION_MANAGER_FAIL);
  HERMES_CHECK(0==zsocket_sendmem(manager->brocker_socket_, (char*)(&user_data), sizeof(user_data), ZFRAME_MORE), return CONNECTION_MANAGER_FAIL);
  HERMES_CHECK(0==zsocket_sendmem(manager->brocker_socket_, request, request_length, 0), return CONNECTION_MANAGER_FAIL);
  return CONNECTION_MANAGER_SUCCESS;  
}

connection_manager_status_t connection_manager_send_sync_request(connection_manager_t* manager, const uint8_t* request, const size_t request_length, uint8_t** response, size_t* response_length){
  void* dummy =NULL;
  HERMES_CHECK(0==zsocket_sendmem(manager->client_socket_, (char*)(&dummy), sizeof(dummy), ZFRAME_MORE), return CONNECTION_MANAGER_FAIL);
  HERMES_CHECK(0==zsocket_sendmem(manager->client_socket_, (char*)(&dummy), sizeof(dummy), ZFRAME_MORE), return CONNECTION_MANAGER_FAIL);
  HERMES_CHECK(0==zsocket_sendmem(manager->client_socket_, request, request_length, 0), return CONNECTION_MANAGER_FAIL);
  zmsg_t* msg=zmsg_recv(manager->client_socket_);
  HERMES_CHECK(msg, return CONNECTION_MANAGER_FAIL);
  HERMES_CHECK(zmsg_size(msg)==3, zmsg_destroy(&msg); return CONNECTION_MANAGER_FAIL);
  zframe_t* response_frame = zmsg_last(msg);
  HERMES_CHECK(response_frame, zmsg_destroy(&msg); return CONNECTION_MANAGER_FAIL);
  *response_length = zframe_size(response_frame);
  *response = realloc(*response, *response_length);
  HERMES_CHECK(*response, zframe_destroy(&response_frame); return CONNECTION_MANAGER_FAIL);
  memcpy(*response, zframe_data(response_frame), *response_length);
  zmsg_destroy(&msg);
  return CONNECTION_MANAGER_SUCCESS;
}

connection_manager_status_t connection_manager_destroy(connection_manager_t* manager){
  manager->stop_=1;
  HERMES_CHECK(manager, return CONNECTION_MANAGER_INVALID_PARAM);
  char command=COMMAND_QUIT;
  HERMES_CHECK(0==zsocket_sendmem(manager->brocker_socket_, &command, 1, 0), return CONNECTION_MANAGER_FAIL);
  if(manager->worker_threads_){
    thread_pool_destroy(manager->worker_threads_);
    manager->worker_threads_=NULL;
  }
  if(manager->zctx_){
    zctx_destroy(&(manager->zctx_));
  }
  if(manager->worker_identities_){
    int i=0;
    for(;i<manager->worker_identities_count_;++i){
      free(manager->worker_identities_[i]);
    }
    free(manager->worker_identities_);
    manager->worker_identities_=NULL;
  }
  free(manager);
  return CONNECTION_MANAGER_SUCCESS;
}
