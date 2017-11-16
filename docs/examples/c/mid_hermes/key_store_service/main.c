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



#include <hermes/key_store/service.h>
#include "../common/transport.h"
#include "../common/config.h"
#include <hermes/secure_transport/session_callback.h>
#include "db.h"

#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <hermes/secure_transport/transport.h>
#include <string.h>

#define SUCCESS 0
#define FAIL 1

#define SERVER_PORT 8890

int socket_desc=0;

void exit_handler(int s){
  UNUSED(s);
  fprintf(stderr, "\rexited...\n");
  close(socket_desc);
  exit(1); 
}

void* key_store(void* arg){
  // create transport with credential store to use him as secure session callback for retrieving public keys
  hm_rpc_transport_t* raw_credential_store_transport = server_connect(CREDENTIAL_STORE_IP, CREDENTIAL_STORE_PORT);
  if (!raw_credential_store_transport){
    perror("can't connect to credential store\n");
    return (void*)FAIL;
  }
  hm_rpc_transport_t* credential_store_transport = create_secure_transport(
          key_store_id, strlen((char*)key_store_id), key_store_private_key, sizeof(key_store_private_key),
          credential_store_pk, sizeof(credential_store_pk),
          credential_store_id, strlen((char*)credential_store_id), raw_credential_store_transport, false);
  if(!credential_store_transport){
    perror("can't initialize secure transport to credential store\n");
    transport_destroy(&raw_credential_store_transport);
    return (void*)FAIL;
  }

  // create secure transport with new client
  hm_rpc_transport_t* client_transport=transport_create((int)(intptr_t)arg);
  if(!client_transport){
    perror("client transport creation error ...\n");
    return (void*)FAIL;
  }

  secure_session_user_callbacks_t* session_callback = get_session_callback_with_remote_credential_store(
          credential_store_transport);
  hm_rpc_transport_t* secure_client_transport = create_secure_transport_with_callback(
          key_store_id, strlen((char*)key_store_id),key_store_private_key, sizeof(key_store_private_key),
          session_callback, client_transport, true);
  if(!secure_client_transport){
      perror("can't establish secure transport with client\n");
      return (void*)FAIL;
  }

  hm_ks_db_t* db=db_create();
  if(!db){
    transport_destroy(&client_transport);
    perror("can't create key store\n");
    return (void*)FAIL;
  }
  hm_key_store_service_t* service=hm_key_store_service_create(secure_client_transport, db);
  if(!service){
    transport_destroy(&client_transport);
    perror("service creation error ...\n");
    return (void*)FAIL;
  }
  fprintf(stderr, "service started ...\n");
  hm_key_store_service_start(service);
  fprintf(stderr, "service stoped ...\n");
  hm_key_store_service_destroy(&service);
  transport_destroy(&client_transport);
  return NULL;
}

int main(int argc, char** argv){
  UNUSED(argc);
  UNUSED(argv);
  struct sockaddr_in server , client;
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1){
    perror("Could not create socket");
    return FAIL;
  }
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( KEY_STORE_PORT );
#ifdef REUSE_SOCKET
  int reuse=1;
  if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0){
        perror("setsockopt(SO_REUSEADDR) failed");
  }
  if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) {
        perror("setsockopt(SO_REUSEPORT) failed");
        return FAIL;
  }
#endif
  if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
      perror("bind failed. Error");
      return FAIL;
  }
  listen(socket_desc , 3);
  signal(SIGINT,exit_handler);
  fprintf(stderr, "listen ...\n");
  while(true){
    int c = sizeof(struct sockaddr_in);
    int64_t client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    pthread_t pid;
    if(client_sock<0 || 0!=pthread_create(&pid, NULL, key_store, (void*)client_sock)){
       printf("acception error ...\n");
    }
  }
  close(socket_desc);
  return SUCCESS;
}
