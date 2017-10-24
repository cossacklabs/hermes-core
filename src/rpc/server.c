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




#include <hermes/rpc/server.h>
#include <hermes/common/hash_table.h>
#include <hermes/common/errors.h>
#include <assert.h>

#include <stdio.h>

struct hm_rpc_server_type{
  hm_rpc_transport_t* transport;
  hm_hash_table_t* func_table;
};

hm_rpc_server_t* hm_rpc_server_create(hm_rpc_transport_t* transport){
  if(!transport){
    return NULL;
  }
  hm_rpc_server_t* server=calloc(sizeof(hm_rpc_server_t),1);
  assert(server);
  server->transport=transport;
  server->func_table=hm_hash_table_create();
  if(!(server->func_table)){
    hm_rpc_server_destroy(&server);
    return NULL;
  }
  return server;
}

uint32_t hm_rpc_server_destroy(hm_rpc_server_t** server){
  if(!server || !(*server)){
    return HM_INVALID_PARAMETER;
  }
  hm_hash_table_destroy(&((*server)->func_table));
  free(*server);
  *server=NULL;
  return HM_SUCCESS;
}

uint32_t hm_rpc_server_reg_func(
        hm_rpc_server_t* server,
        const uint8_t* func_signature, const size_t func_signature_length,
        hm_server_func_t func){
  if(!server || !func_signature || !func_signature_length || !func){
    return HM_INVALID_PARAMETER;
  }
  return hm_hash_table_set(
          server->func_table, func_signature, func_signature_length, (const uint8_t*)(&func), sizeof(hm_server_func_t));
}

uint32_t hm_rpc_server_send_error(hm_rpc_server_t* server, uint32_t error){
  if(!server || HM_SUCCESS==error){
    return HM_INVALID_PARAMETER;
  }
  return server->transport->send(server->transport->user_data, (uint8_t*)&error, sizeof(uint32_t));
}

uint32_t hm_rpc_server_send(hm_rpc_server_t* server, hm_param_pack_t* pack){
  if(!server){
    return HM_INVALID_PARAMETER;
  }
  size_t error=HM_SUCCESS;
  if(HM_SUCCESS!=server->transport->send(server->transport->user_data, (uint8_t*)&error, sizeof(uint32_t))){
    return HM_FAIL;
  }
  if(pack){
     return hm_param_pack_send(pack, server->transport);
  }
  return HM_SUCCESS;
}


uint32_t hm_rpc_server_call_func(hm_rpc_server_t* server, const uint8_t* func_signature, const size_t func_signature_length, void* user_data){
  if(!server || !func_signature || !func_signature_length){
    return HM_INVALID_PARAMETER;
  }
  hm_server_func_t* func=NULL;
  size_t func_length=sizeof(hm_server_func_t);
#ifdef DEBUG
  fprintf(stderr, "calling %s...", (const char*)func_signature);
#endif
  if(HM_SUCCESS!=hm_hash_table_get(server->func_table, func_signature, func_signature_length, (uint8_t**)&func, &func_length)){
    hm_rpc_server_send_error(server, HM_FAIL);
#ifdef DEBUG
#define ANSI_COLOR_RED     "\033[1m\033[31m"
#define ANSI_COLOR_GREEN   "\033[1m\033[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"
    fprintf(stderr, ANSI_COLOR_RED "FAIL (calling of unregistered function)\n" ANSI_COLOR_RESET);
#endif
    return HM_FAIL;    
  }
  hm_param_pack_t* pack=hm_param_pack_receive(server->transport);
  if(!pack){
    hm_rpc_server_send_error(server, HM_FAIL);
#ifdef DEBUG
    fprintf(stderr, ANSI_COLOR_RED "FAIL (parameters receiving error)\n" ANSI_COLOR_RESET);
#endif
    return HM_FAIL;
  }
  hm_param_pack_t* out_pack=NULL;
  uint32_t res=(*func)(pack, &out_pack, user_data);
  hm_param_pack_destroy(&pack);
  if(HM_SUCCESS!=res){
#ifdef DEBUG
#define ANSI_COLOR_RED     "\033[1m\033[31m"
#define ANSI_COLOR_GREEN   "\033[1m\033[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"
    fprintf(stderr, ANSI_COLOR_RED "FAIL (calling function error)\n" ANSI_COLOR_RESET);
#endif
    return hm_rpc_server_send_error(server, res);
  }
  res=hm_rpc_server_send(server, out_pack);
  hm_param_pack_destroy(&out_pack);
#ifdef DEBUG
  if(HM_SUCCESS!=res){
     fprintf(stderr, ANSI_COLOR_RED "FAIL (result sending error)\n" ANSI_COLOR_RESET);
  }else{
    fprintf(stderr, ANSI_COLOR_GREEN "SUCCESS\n" ANSI_COLOR_RESET);
  }
#endif
  return res;
}

uint32_t hm_rpc_server_call(hm_rpc_server_t* server, void* user_data){
  if(!server){
    return HM_INVALID_PARAMETER;
  }
  uint32_t func_signature_length=0;
  if(HM_SUCCESS!=server->transport->recv(server->transport->user_data, (uint8_t*)&func_signature_length, sizeof(uint32_t))){
    return HM_FAIL;    
  }
  uint8_t* func_signature=malloc(func_signature_length);
  assert(func_signature);
  if(HM_SUCCESS!=server->transport->recv(server->transport->user_data, func_signature, func_signature_length)){
    free(func_signature);
    return HM_FAIL;    
  }
  uint32_t res=hm_rpc_server_call_func(server, func_signature, func_signature_length, user_data);
  free(func_signature);
  return res;
}
