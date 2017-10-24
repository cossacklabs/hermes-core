/*
* Copyright (c) 2017 Cossack Labs Limited
*
* This file is a part of Hermes-core.
*
* Hermes-core is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Hermes-core is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Hermes-core.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include <themis/secure_session.h>
#include <hermes/credential_store/service.h>
#include <hermes/secure_transport/transport.h>
#include <hermes/common/errors.h>
#include "../common/transport.h"
#include "../common/session_callback.h"
#include "../common/config.h"
#include "db.h"

#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>

#define SUCCESS 0
#define FAIL 1

#define SERVER_PORT 8888

int socket_desc=0;

void exit_handler(int s){
  fprintf(stderr, "\rexited...\n");
  close(socket_desc);
  exit(1); 
}

void* credential_store(void* arg){
  hm_rpc_transport_t* client_transport=transport_create((int)arg);
  if(!client_transport){
    perror("client transport creation error ...\n");
    return (void*)FAIL;
  }
  hm_cs_db_t* db=db_create();
  if(!db){
    transport_destroy(&client_transport);
      perror("can't create credential store\n");
    return (void*)FAIL;
  }
  secure_session_user_callbacks_t* session_callback = calloc(1, sizeof(secure_session_user_callbacks_t));
  session_callback->user_data = db;
  session_callback->get_public_key_for_id = get_public_key_for_id_from_local_credential_store_callback;

  hm_rpc_transport_t* secure_transport = create_secure_transport_with_callback(
          credential_store_id, strlen(credential_store_id),
          credential_store_private_key, sizeof(credential_store_private_key),
          session_callback, client_transport, true);

  hm_credential_store_service_t* service=hm_credential_store_service_create(secure_transport, db);
  if(!service){
    transport_destroy(&client_transport);
    perror("service creation error ...\n");
    return (void*)FAIL;
  }
  fprintf(stderr, "service started ...\n");
  hm_credential_store_service_start(service);
  fprintf(stderr, "service stoped ...\n");
  hm_credential_store_service_destroy(&service);
  transport_destroy(&client_transport);
  return NULL;
}

int main(int argc, char** argv){
  struct sockaddr_in server , client;
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1){
    perror("Could not create socket\n");
    return FAIL;
  }
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( SERVER_PORT );

  if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
      perror("bind failed. Error\n");
      return FAIL;
  }
  listen(socket_desc , 3);
  signal(SIGINT,exit_handler);
  fprintf(stderr, "listen ...\n");
  while(true){
    int c = sizeof(struct sockaddr_in);
    int64_t client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    pthread_t pid;
    if(client_sock<0 || 0!=pthread_create(&pid, NULL, credential_store, (void*)client_sock)){
       printf("acception error ...\n");
    }
  }
  close(socket_desc);
  return SUCCESS;
}
