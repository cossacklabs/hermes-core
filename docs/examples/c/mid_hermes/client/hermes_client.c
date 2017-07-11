/*
 * Copyright (c) 2017 Cossack Labs Limited
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


#include <hermes/mid_hermes/mid_hermes.h>
#include "transport.h"
#include "../../utils/base64.h"
#include "../../utils/utils.h"


#include <argp.h>
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


#define SUCCESS 0
#define FAIL 1

#define COMMAND_GET_BLOCK 1
#define COMMAND_ADD_BLOCK 2
#define COMMAND_UPD_BLOCK 3
#define COMMAND_DEL_BLOCK 4
#define COMMAND_GRANT_READ 5
#define COMMAND_GRANT_UPDATE 6
#define COMMAND_DENY_READ 7
#define COMMAND_DENY_UPDATE 8
#define COMMAND_ROTATE 9

//#define user_id "user1"
//#define user_id_length strlen(user_id)+1
//#define sk "aaaaa"
//#define sk_length strlen(sk)+1

#define CREDENTIAL_STORE_IP   "127.0.0.1"
#define DATA_STORE_IP         "127.0.0.1"
#define KEY_STORE_IP          "127.0.0.1"

#define CREDENTIAL_STORE_PORT 8888
#define DATA_STORE_PORT       8889
#define KEY_STORE_PORT        8890

hm_rpc_transport_t* server_connect(const char* ip, int port){
  int64_t sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1){
      fprintf(stderr, "connection error (1) (%s:%i)\n",ip, port);
      return NULL;
  }
  struct sockaddr_in server;
  server.sin_addr.s_addr = inet_addr(ip);
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
      fprintf(stderr, "connection error (2) (%s:%i)\n",ip, port);
      return NULL;
  }

  hm_rpc_transport_t* transport=transport_create(sock);
  if(!transport){
    close(sock);
    fprintf(stderr, "connection error (3) (%s:%i)\n",ip, port);
    return NULL;
  }
  return transport;
}

int parse_command(const char* command){
  size_t command_len = strlen(command);
  switch(command[0]){
  case 'b':
    if(command_len!=2){
      return 0;
    }
    switch(command[1]){
    case 'r':
      return COMMAND_GET_BLOCK;
    case 'a':
      return COMMAND_ADD_BLOCK;
    case 'u':
      return COMMAND_UPD_BLOCK;
    case 'd':
      return COMMAND_DEL_BLOCK;
    default:
      return 0;
    }
  case 'r':
    if(command_len!=3){
      return 0;
    }
    switch(command[1]){
    case 'g':
      switch(command[2]){
      case 'r':
        return COMMAND_GRANT_READ;
      case 'u':
        return COMMAND_GRANT_UPDATE;
      default:
        return 0;
      }
    case 'd':
      switch(command[2]){
      case 'r':
        return COMMAND_DENY_READ;
      case 'u':
        return COMMAND_DENY_UPDATE;
      default:
        return 0;
      }
    default:
      return 0;
    }
  case 'u':
    if(command_len!=1){
      return 0;
    }
    return COMMAND_ROTATE;
  default:
    return 0;
  }
}

int main(int argc, char** argv){
  if(argc<5){
    fprintf(stderr, "error: params error\n");
    return 1;
  }
  hm_rpc_transport_t* cs_transport=server_connect(CREDENTIAL_STORE_IP, CREDENTIAL_STORE_PORT);
  hm_rpc_transport_t* ks_transport=server_connect(KEY_STORE_IP, KEY_STORE_PORT);
  hm_rpc_transport_t* ds_transport=server_connect(DATA_STORE_IP, DATA_STORE_PORT);
  mid_hermes_t* mh=NULL;
  uint8_t sk[1024];
  size_t sk_length=sizeof(sk);
  if(!cs_transport
     || !ds_transport
     || !ks_transport
     || !(sk_length=base64_decode(sk, argv[3], sk_length))
     || !(mh=mid_hermes_create(argv[2], strlen(argv[2])+1, sk, sk_length, ks_transport, ds_transport, cs_transport))){
    transport_destroy(&cs_transport);
    transport_destroy(&ds_transport);
    transport_destroy(&ks_transport);
    fprintf(stderr, "mid_hermes creation error ...\n");
    return FAIL;
  }
  int res=0;
  switch(parse_command(argv[1])){
  case COMMAND_GET_BLOCK:{
    uint8_t *data=NULL, *meta=NULL;
    size_t data_length=0, meta_length=0;
    if(argc!=5 || 0!=mid_hermes_read_block(mh, argv[4], strlen(argv[4])+1, &data, &data_length, &meta, &meta_length)){
      fprintf(stderr, "error: block getting error\n");
      break;
    }
    fwrite(data, 1, data_length, stdout);
    fwrite(meta, 1, meta_length, stderr);
    free(data);
    free(meta);
  }
    break;
  case COMMAND_ADD_BLOCK:{
    size_t id_length=strlen(argv[4])+1;
    uint8_t* block=NULL;
    size_t block_length=0;
    if(argc!=6
       || (SUCCESS!=read_whole_file(argv[4], &block, &block_length))
       || (0!=mid_hermes_create_block(mh,(uint8_t**)&(argv[4]), &id_length, block, block_length, argv[5], strlen(argv[5])+1))){
      free(block);
      fprintf(stderr, "error: block adding error\n");
      break;
    }
    free(block);
  }
    break;
  case COMMAND_UPD_BLOCK:{
    uint8_t* block=NULL;
    size_t block_length=0;
    if(argc!=6
       || (SUCCESS!=read_whole_file(argv[4], &block, &block_length))
       || (0!=mid_hermes_update_block(mh, argv[4], strlen(argv[4])+1, block, block_length, argv[5], strlen(argv[5])+1))){
      free(block);
      fprintf(stderr, "error: block adding error\n");
      break;
    }
    free(block);
  }
    break;    
  case COMMAND_DEL_BLOCK:{
    if(argc!=5
       || 0!=mid_hermes_delete_block(mh, argv[4], strlen(argv[4])+1)){
      fprintf(stderr, "error: block deleting error\n");
    }
    break;
  }
  case COMMAND_GRANT_READ:
    if(argc!=6
       || 0!=mid_hermes_grant_read_access(mh, argv[4], strlen(argv[4])+1, argv[5], strlen(argv[5])+1)){
      fprintf(stderr, "error: block read access granting error\n");
    }
    break;
  case COMMAND_GRANT_UPDATE:
    if(argc!=6
       || 0!=mid_hermes_grant_update_access(mh, argv[4], strlen(argv[4])+1, argv[5], strlen(argv[5])+1)){
      fprintf(stderr, "error: block update access granting error\n");
    }
    break;
  case COMMAND_DENY_READ:
    if(argc!=6
       || 0!=mid_hermes_deny_read_access(mh, argv[4], strlen(argv[4])+1, argv[5], strlen(argv[5])+1)){
      fprintf(stderr, "error: block read access denying error\n");
    }
    break;
  case COMMAND_DENY_UPDATE:
    if(argc!=6
       || 0!=mid_hermes_deny_update_access(mh, argv[4], strlen(argv[4])+1, argv[5], strlen(argv[5])+1)){
      fprintf(stderr, "error: block update access denying error\n");
    }
    break;
  case COMMAND_ROTATE:
    if(argc!=5
       || 0!=mid_hermes_rotate_block(mh, argv[4], strlen(argv[4])+1)){
      fprintf(stderr, "error: block rotate error\n");
    }
    break;
  default:
    fprintf(stderr, "error: undefined command %s\n", argv[1]);
  }
  mid_hermes_destroy(&mh);
  transport_destroy(&cs_transport);
  transport_destroy(&ds_transport);
  transport_destroy(&ks_transport);
  return SUCCESS;
}
