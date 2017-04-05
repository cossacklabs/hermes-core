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

#include <common/test_utils.h>

#include <common/test_utils.h>
#include <hermes/common/errors.h>

#include <hermes/key_store/server.h>
#include <hermes/key_store/client.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

#include "../common/test_transport.h"
#include "../common/test_key_store_db.h"  

#define CS_PIPE_NAME "/tmp/hermes_core_test_ds_pipe" 
#define SC_PIPE_NAME "/tmp/hermes_core_test_sc_pipe"
#define KS_TEST_DB_FILE_NAME "tests/ds_test_db"

void* server(void* param){
  hm_rpc_transport_t* transport = hm_test_transport_create(SC_PIPE_NAME, CS_PIPE_NAME, true);
  if(!transport){
    testsuite_fail_if(true, "server transport initializing");
    return (void*)1;
  }
  hm_ks_db_t* db=hm_test_ks_db_create(KS_TEST_DB_FILE_NAME);
  if(!db){
    hm_test_transport_destroy(transport);
    return (void*)1;
  }
  hm_key_store_server_t* s=hm_key_store_server_create(transport, db);
  if(!s){
    hm_test_ks_db_destroy(&db);
    hm_test_transport_destroy(transport);
    testsuite_fail_if(true, "data store server creation");
    return (void*)1;
  }
  int i=0;
  for(;i<1;++i){
    if(HM_SUCCESS!=hm_key_store_server_call(s)){
      hm_key_store_server_destroy(&s);
      hm_test_ks_db_destroy(&db);
      hm_test_transport_destroy(transport);
      testsuite_fail_if(true, "data store server calling");
      return (void*)1;
    }
  }
  hm_key_store_server_destroy(&s);
  hm_test_ks_db_destroy(&db);
  hm_test_transport_destroy(transport);
  return NULL;
}

uint32_t gen_new_block(uint8_t** block, size_t* block_length, uint8_t** mac, size_t* mac_length){
  *block_length=2556;
  *block=malloc(*block_length);
  assert(*block);
  *mac_length=32;
  *mac=malloc(*mac_length);
  assert(*mac);
  return HM_SUCCESS;
}

void* client(void* param){
  hm_rpc_transport_t* transport = hm_test_transport_create(CS_PIPE_NAME, SC_PIPE_NAME, false);
  if(!transport){
    testsuite_fail_if(true, "client transport initializing");
    return (void*)1;
  }
  hm_key_store_client_sync_t* c=hm_key_store_client_sync_create(transport);
  if(!c){
    hm_test_transport_destroy(transport);
    testsuite_fail_if(true, "key store client sync creation");
    return (void*)1;
  }
  uint8_t* key=NULL;
  size_t key_length=0;
  uint8_t* block=NULL, *mac=NULL, *id=NULL;
  size_t block_length=0, mac_length=0, id_length=0;
/*   if(HM_SUCCESS!=gen_new_block(&block, &block_length, &mac, &mac_length) || HM_SUCCESS!=hm_key_store_client_sync_call_set_rtoken(c, block, block_length, mac, mac_length, &id, &id_length)){ */
/*     free(block); */
/*     free(mac); */
/*     hm_key_store_client_sync_destroy(&c); */
/*     hm_test_transport_destroy(transport); */
/*     testsuite_fail_if(true, "key store client sync calling"); */
/*     return (void*)1; */
/*   } */
/*   free(block); */
/* free(mac); */
/* if(HM_SUCCESS!=gen_new_block(&block, &block_length, &mac, &mac_length) || HM_SUCCESS!=hm_key_store_client_sync_call_create_block(c, block, block_length, mac, mac_length, &id, &id_length)){ */
/*     free(block); */
/* free(mac); */
/* hm_key_store_client_sync_destroy(&c); */
/* hm_test_transport_destroy(transport); */
/* testsuite_fail_if(true, "key store client sync calling"); */
/* return (void*)1; */
/* } */
/*   //  free(key); */
  hm_key_store_client_sync_destroy(&c);
  hm_test_transport_destroy(transport);
  return NULL;
}

int key_store_general_flow(){
  mkfifo(CS_PIPE_NAME, 0666);
  mkfifo(SC_PIPE_NAME, 0666);
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
  unlink(CS_PIPE_NAME);
  unlink(SC_PIPE_NAME);
  if(res1 || res2){
    return -1;
  }
  return 0;
}

void key_store_tests(){
  testsuite_fail_if(key_store_general_flow(), "key store general flow");
}

int main(int argc, char *argv[]){
  testsuite_start_testing();
  testsuite_enter_suite("key_store test");

  testsuite_run_test(key_store_tests);

  testsuite_finish_testing();
  return testsuite_get_return_value();
}

