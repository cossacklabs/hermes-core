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
#include <hermes/rpc/client.h>
#include <common/test_utils.h>
#include <hermes/common/errors.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>

#include "../common/test_transport.h"


#define CS_PIPE_NAME "/tmp/hermes_core_test_cs_pipe" 
#define SC_PIPE_NAME "/tmp/hermes_core_test_sc_pipe"

uint32_t func1(uint32_t a, uint32_t b){
  return a+b;
}

uint32_t func1_proxy(hm_rpc_client_sync_t* c, uint32_t a, uint32_t b, uint32_t* res){

  hm_param_pack_t* in=HM_PARAM_PACK(HM_PARAM_INT32(a), HM_PARAM_INT32(b));
  hm_param_pack_t* out=NULL;
  uint32_t status;
  if(HM_SUCCESS!=hm_rpc_client_sync_call(c, (const uint8_t*)"func1", sizeof("func1"), in, &status, &out)){
    hm_param_pack_destroy(&in);
    return HM_FAIL;
  }
  hm_param_pack_destroy(&in);
  if(HM_SUCCESS!=status){
    return status;
  }
  *res=0;
  if(HM_SUCCESS!=HM_PARAM_EXTRACT(out, HM_PARAM_INT32(res))){
    hm_param_pack_destroy(&out);
    return HM_FAIL;    
  }
  hm_param_pack_destroy(&out);
  return HM_SUCCESS;
}

uint32_t func1_stub(hm_param_pack_t* in, hm_param_pack_t** out, void* user_data){
  uint32_t a,b, res;
  if(HM_SUCCESS!=HM_PARAM_EXTRACT(in, HM_PARAM_INT32(&a), HM_PARAM_INT32(&b))){
    return HM_FAIL;
  }
  res=func1(a,b);
  *out=HM_PARAM_PACK(HM_PARAM_INT32(res));
  if(!(*out)){
    return HM_FAIL;
  }
  return HM_SUCCESS;
}


void* client(void *param){
  hm_rpc_transport_t* transport = hm_test_transport_create(CS_PIPE_NAME, SC_PIPE_NAME, false);
  if(!transport){
    testsuite_fail_if(true, "client transport initializing");
    return (void*)1;
  }
  hm_rpc_client_sync_t* c=hm_rpc_client_sync_create(transport);
  if(!c){
    testsuite_fail_if(true, "client object initializing");
    hm_test_transport_destroy(transport);
    return (void*)1;
  }
  uint32_t res=0;
  if(HM_SUCCESS!=func1_proxy(c, 32, 56, &res)){
    testsuite_fail_if(true, "client call");
    hm_rpc_client_sync_destroy(&c);
    hm_test_transport_destroy(transport);
    return (void*)1;
  }
  hm_rpc_client_sync_destroy(&c);
  hm_test_transport_destroy(transport);
  if(res != func1(32,56)){
    return (void*)1;
  }
  return NULL;
}

void* server(void *param){  
  hm_rpc_transport_t* transport = hm_test_transport_create(SC_PIPE_NAME, CS_PIPE_NAME, true);
  if(!transport){
    testsuite_fail_if(true, "server transport initializing");
    return (void*)1;
  }
  hm_rpc_server_t* s=hm_rpc_server_create(transport);
  if(!s){
    testsuite_fail_if(true, "server object initializing");
    hm_test_transport_destroy(transport);
    return (void*)1;
  }
  if(HM_SUCCESS!=hm_rpc_server_reg_func(s, (const uint8_t*)"func1", sizeof("func1"), func1_stub)){
    testsuite_fail_if(true, "server func registration");
    hm_rpc_server_destroy(&s);
    hm_test_transport_destroy(transport);
    return (void*)1;
  }
  if(HM_SUCCESS!=hm_rpc_server_call(s, NULL)){
    testsuite_fail_if(true, "server func calling");
    hm_rpc_server_destroy(&s);
    hm_test_transport_destroy(transport);
    return (void*)1;    
  }
  hm_rpc_server_destroy(&s);
  hm_test_transport_destroy(transport);
  return NULL;
}


static int server_general_flow(){
  mkfifo(SC_PIPE_NAME, 0666);
  mkfifo(CS_PIPE_NAME, 0666);
  pthread_t client_thread;
  if(pthread_create(&client_thread, NULL, client, NULL)){
    testsuite_fail_if(true, "creating client thread");
    return -11;
  }
  pthread_t server_thread;
  if(pthread_create(&server_thread, NULL, server, NULL)){
    testsuite_fail_if(true, "creating server thread");
    return -11;
  }
  int res1, res2;
  pthread_join(server_thread, (void**)(&res1));
  pthread_join(client_thread, (void**)(&res2));
  unlink(SC_PIPE_NAME);
  unlink(CS_PIPE_NAME);
  if(res1 || res2){
    return -1;
  }
  return 0;
}

void client_server_tests(){
  testsuite_fail_if(0!=server_general_flow(), "client-server general flow");
}

