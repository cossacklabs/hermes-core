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

#include "key_store.h"
#include "credential_store.h"
#include "data_store.h"

#include <themis/themis.h>

#include <string.h>

#define COMMAND_GET_BLOCK 1
#define COMMAND_ADD_BLOCK 2
#define COMMAND_UPD_BLOCK 3
#define COMMAND_DEL_BLOCK 4
#define COMMAND_GRANT_READ 5
#define COMMAND_GRANT_UPDATE 6
#define COMMAND_DENY_READ 7
#define COMMAND_DENY_UPDATE 8
#define COMMAND_ROTATE 9

key_store_t* ks=NULL;
credential_store_t* cs=NULL;
data_store_t* ds=NULL;

int finalize(){
  key_store_destroy(&ks);
  data_store_destroy(&ds);
  credential_store_destroy(&cs);
  return 0;
}

int init(){
  ks=key_store_create();
  ds=data_store_create();
  cs=credential_store_create();
  if(!ks || !cs || !ds){
    return 1;
}
return 0;
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

int main(int argc, char* argv[]){
  if(0!=init()){
    fprintf(stderr, "error: initialisation\n");
    finalize();
    return 1;
  }
  if(argc<2){
    fprintf(stderr, "error: params error\n");
    finalize();
    return 1;
  }
  int res=0;
  switch(parse_command(argv[1])){
  case COMMAND_GET_BLOCK:
  case COMMAND_ADD_BLOCK:
  case COMMAND_UPD_BLOCK:
  case COMMAND_DEL_BLOCK:
  case COMMAND_GRANT_READ:
  case COMMAND_GRANT_UPDATE:
  case COMMAND_DENY_READ:
  case COMMAND_DENY_UPDATE:
  case COMMAND_ROTATE:
  default:
    fprintf(stderr, "error: undefined command %s\n", argv[1]);
    finalize();
    return 1;    
  }
  
  finalize();
  return 0;
}
