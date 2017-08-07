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



#include <hermes/data_store/service.h>
#include "../common/transport.h"
#include "db.h"

#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>

#define SUCCESS 0
#define FAIL 1

#define SERVER_PORT 8889

int socket_desc=0;

void exit_handler(int s){
  fprintf(stderr, "\rexited...\n");
  close(socket_desc);
  exit(1); 
}

void* data_store(void* arg){
  hm_rpc_transport_t* client_transport=transport_create((int64_t)arg);
  if(!client_transport){
    perror("client transport creation error ...");
    return NULL;
  }
  hm_ds_db_t* db=db_create();
  if(!db){
    transport_destroy(&client_transport);
    return NULL;
  }
  hm_data_store_service_t* service=hm_data_store_service_create(client_transport, db);
  if(!service){
    transport_destroy(&client_transport);
    perror("service creation error ...\n");
    return NULL;
  }
  fprintf(stderr, "service started ...\n");
  hm_data_store_service_start(service);
  fprintf(stderr, "service stoped ...\n");
  hm_data_store_service_destroy(&service);
  transport_destroy(&client_transport);
  return NULL;
}

int main(int argc, char** argv){
  struct sockaddr_in server , client;
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1){
    perror("Could not create socket");
    return FAIL;
  }
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( SERVER_PORT );

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
    if(client_sock<0 || 0!=pthread_create(&pid, NULL, data_store, (void*)client_sock)){
       printf("acception error ...\n");
    }
  }
  close(socket_desc);
  return SUCCESS;
}
