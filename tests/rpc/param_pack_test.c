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

#include <hermes/rpc/param_pack.h>
#include <hermes/common/errors.h>
#include <themis/themis.h>
#include <common/test_utils.h>

#include "../common/test_transport.h"

#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>

#define CS_PIPE_NAME "/tmp/hermes_core_test_cs_pipe" 
#define SC_PIPE_NAME "/tmp/hermes_core_test_sc_pipe"

static int param_pack_general_flow(){
  uint8_t *param1=NULL, *param3=NULL;
  size_t param1_length=0, param3_length=0;
  int32_t param2=0;
  if(THEMIS_SUCCESS!=soter_rand((uint8_t*)(&param1_length), 2)){
    testsuite_fail_if(true, "param generation");
    return -1;
  }
  if(THEMIS_SUCCESS!=soter_rand((uint8_t*)(&param2), sizeof(uint32_t))){
    testsuite_fail_if(true, "param generation");
    return -1;
  }
  if(THEMIS_SUCCESS!=soter_rand((uint8_t*)(&param3_length), 2)){
    testsuite_fail_if(true, "param generation");
    return -1;
  }
  param1 = malloc(param1_length);
  if(!param1){
    testsuite_fail_if(!param1, "memory allocation");
    return -1;
  }
  param3 = malloc(param3_length);
  if(!param3){
    testsuite_fail_if(!param3, "memory allocation");
    return -1;
  }
  hm_param_pack_t* pack=HM_PARAM_PACK(HM_PARAM_BUFFER(param1, param1_length), HM_PARAM_INT32(param2), HM_PARAM_BUFFER_C(param3, param3_length));
  if(!pack){
    testsuite_fail_if(!pack, "param pack object creation");
    return -1;
  }
  size_t buf_len=0;
  if(HM_BUFFER_TOO_SMALL != hm_param_pack_write(pack, NULL, &buf_len)){
    testsuite_fail_if(true, "param pack writing buffer (length determination)");
    return -1;
  }
  uint8_t* buf=malloc(buf_len);
  if(!buf){
    testsuite_fail_if(!buf, "not enough memeory");
    return -1;
  }
  if(HM_SUCCESS != hm_param_pack_write(pack, buf, &buf_len)){
    testsuite_fail_if(true, "param pack writing");
    return -1;
  }
  uint8_t *out_param1=NULL, *out_param3=NULL;
  size_t out_param1_length=0, out_param3_length=0;
  int32_t out_param2=0;
  hm_param_pack_t* out_pack=hm_param_pack_read(buf, buf_len);
  if(!out_pack){
    testsuite_fail_if(!out_pack, "param pack reading");
    return -1;
  }
  if(HM_SUCCESS!=HM_PARAM_EXTRACT(out_pack,HM_PARAM_BUFFER(&out_param1, &out_param1_length), HM_PARAM_INT32(&out_param2), HM_PARAM_BUFFER(&out_param3, &out_param3_length))){
    testsuite_fail_if(true, "param pack extracting error");
    return -1;
  }
  if(param1_length!=out_param1_length){
    testsuite_fail_if(true, "in and out param equal");
    return -1;
  }
  if(0!=memcmp(param1, out_param1, param1_length)){
    testsuite_fail_if(true, "in and out param equal");
    return -1;
  }
  if(param2!=out_param2){
    testsuite_fail_if(true, "in and out param equal");
    return -1;
  }
  if(param3_length!=out_param3_length){
    testsuite_fail_if(true, "in and out param equal");
    return -1;
  }
  if(0!=memcmp(param3, out_param3, param3_length)){
    testsuite_fail_if(true, "in and out param equal");
    return -1;
  }

  out_param1=NULL;
  out_param3=NULL;
  out_param1_length=0;
  out_param3_length=0;
  out_param2=0;
  if(HM_SUCCESS!=HM_PARAM_EXTRACT(pack,HM_PARAM_BUFFER(&out_param1, &out_param1_length), HM_PARAM_INT32(&out_param2), HM_PARAM_BUFFER_C(&out_param3, &out_param3_length))){
    testsuite_fail_if(true, "param pack extracting error");
    return -1;
  }
  if(param1_length!=out_param1_length){
    testsuite_fail_if(true, "in and out param equal");
    return -1;
  }
  if(0!=memcmp(param1, out_param1, param1_length)){
    testsuite_fail_if(true, "in and out param equal");
    return -1;
  }
  if(param2!=out_param2){
    testsuite_fail_if(true, "in and out param equal");
    return -1;
  }
  if(param3_length!=out_param3_length){
    testsuite_fail_if(true, "in and out param equal");
    return -1;
  }
  if(0!=memcmp(param3, out_param3, param3_length)){
    testsuite_fail_if(true, "in and out param equal");
    return -1;
  }

  hm_param_pack_destroy(&out_pack);
  hm_param_pack_destroy(&pack);
  free(param3);
  return 0;
}

void* client_func(void *param){
  uint8_t *param1=NULL, *param3=NULL;
  size_t param1_length=0, param3_length=0;
  int32_t param2=0;
  if(THEMIS_SUCCESS!=soter_rand((uint8_t*)(&param1_length), 1)){
    testsuite_fail_if(true, "param generation");
    return (void*)1;
  }
  if(THEMIS_SUCCESS!=soter_rand((uint8_t*)(&param2), sizeof(uint32_t))){
    testsuite_fail_if(true, "param generation");
    return (void*)1;
  }
  if(THEMIS_SUCCESS!=soter_rand((uint8_t*)(&param3_length), 1)){
    testsuite_fail_if(true, "param generation");
    return (void*)1;
  }
  param1 = malloc(param1_length);
  if(!param1){
    testsuite_fail_if(!param1, "memory allocation");
    return (void*)1;
  }
  param3 = malloc(param3_length);
  if(!param3){
    testsuite_fail_if(!param3, "memory allocation");
    return (void*)1;
  }
  hm_param_pack_t* pack=HM_PARAM_PACK(HM_PARAM_BUFFER(param1, param1_length), HM_PARAM_INT32(param2), HM_PARAM_BUFFER_C(param3, param3_length));
  if(!pack){
    testsuite_fail_if(!pack, "param pack object creation");
    return (void*)1;
  }
  hm_rpc_transport_t* transport = hm_test_transport_create(CS_PIPE_NAME, SC_PIPE_NAME, false);
  if(!transport){
    testsuite_fail_if(true, "server transport initializing");
    hm_param_pack_destroy(&pack);
    return (void*)1;
  }
  if(HM_SUCCESS!=hm_param_pack_send(pack, transport)){
    testsuite_fail_if(true, "param pack sending");
    hm_test_transport_destroy(transport);  
    hm_param_pack_destroy(&pack);
    return (void*)1;
  }
  hm_test_transport_destroy(transport);  
  hm_param_pack_destroy(&pack);
  free(param3);
  return NULL;
}

void* server_func(void *param){
  hm_rpc_transport_t* transport = hm_test_transport_create(SC_PIPE_NAME, CS_PIPE_NAME, true);
  if(!transport){
    testsuite_fail_if(true, "server transport initializing");
    return (void*)1;
  }
  hm_param_pack_t* pack=hm_param_pack_receive(transport);
  if(!pack){
    testsuite_fail_if(true, "param pack receiving");
    hm_test_transport_destroy(transport);  
    return (void*)1;
  }
  uint8_t *out_param1=NULL, *out_param3=NULL;
  size_t out_param1_length=0, out_param3_length=0;
  int32_t out_param2=0;
  if(HM_SUCCESS!=HM_PARAM_EXTRACT(pack,HM_PARAM_BUFFER(&out_param1, &out_param1_length), HM_PARAM_INT32(&out_param2), HM_PARAM_BUFFER(&out_param3, &out_param3_length))){
    testsuite_fail_if(true, "param pack extracting error");
    return (void*)1;
  }
  hm_test_transport_destroy(transport);
  hm_param_pack_destroy(&pack);
  return NULL;
}


static int param_pack_send_receive_test(){
  mkfifo(SC_PIPE_NAME, 0666);
  mkfifo(CS_PIPE_NAME, 0666);
  pthread_t client_thread;
  if(pthread_create(&client_thread, NULL, client_func, NULL)){
    testsuite_fail_if(true, "creating client thread");
    return -11;
  }
  pthread_t server_thread;
  if(pthread_create(&server_thread, NULL, server_func, NULL)){
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


void rpc_tests(){
  testsuite_fail_if(param_pack_general_flow(), "param pack general flow");
  testsuite_fail_if(param_pack_send_receive_test(), "param pack send/receive general flow");
}
