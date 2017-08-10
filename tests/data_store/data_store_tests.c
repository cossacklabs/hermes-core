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


#include <common/test_utils.h>

#include <common/test_utils.h>
#include <hermes/common/errors.h>

#include <hermes/data_store/server.h>
#include <hermes/data_store/client.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

#include <themis/themis.h>
#include "../common/common.h"
#include "../common/test_transport.h"
#include "../common/test_data_store_db.h"  

#define CS_PIPE_NAME "/tmp/hermes_core_test_ds_pipe" 
#define SC_PIPE_NAME "/tmp/hermes_core_test_sc_pipe"

void* server(void* param){
  hm_rpc_transport_t* transport = hm_test_transport_create(SC_PIPE_NAME, CS_PIPE_NAME, true);
  if(!transport){
    testsuite_fail_if(true, "server transport initializing");
    return (void*)TEST_FAIL;
  }
  hm_ds_db_t* db=hm_test_ds_db_create();
  if(!db){
    hm_test_transport_destroy(transport);
    return (void*)TEST_FAIL;
  }
  hm_data_store_server_t* s=hm_data_store_server_create(transport, db);
  if(!s){
    hm_test_ds_db_destroy(&db);
    hm_test_transport_destroy(transport);
    testsuite_fail_if(true, "data store server creation");
    return (void*)TEST_FAIL;
  }
  for(;;){
    if(HM_SUCCESS!=hm_data_store_server_call(s)){
      hm_data_store_server_destroy(&s);
      hm_test_ds_db_destroy(&db);
      hm_test_transport_destroy(transport);
      testsuite_fail_if(true, "data store server calling");
      return (void*)TEST_FAIL;
    }
  }
  hm_data_store_server_destroy(&s);
  hm_test_ds_db_destroy(&db);
  hm_test_transport_destroy(transport);
  return (void*)TEST_SUCCESS;
}

typedef struct block_type{
  uint8_t id[BLOCK_ID_LENGTH];
  uint8_t block[MAX_BLOCK_LENGTH];
  size_t block_length;
  uint8_t meta[MAX_META_LENGTH];
  size_t meta_length;
  uint8_t mac[MAX_MAC_LENGTH];
  size_t mac_length;
}block_t;

void gen_new_block(block_t* block){
  assert(SOTER_SUCCESS==soter_rand(block->id, BLOCK_ID_LENGTH));  
  block->block_length=sizeof(block->block);
  assert(SOTER_SUCCESS==soter_rand(block->block, block->block_length));
  block->meta_length=sizeof(block->meta);
  assert(SOTER_SUCCESS==soter_rand(block->meta, block->meta_length));
  block->mac_length=sizeof(block->mac);
  assert(SOTER_SUCCESS==soter_rand(block->mac, block->mac_length));
}

void* client(void* param){
  hm_rpc_transport_t* transport = hm_test_transport_create(CS_PIPE_NAME, SC_PIPE_NAME, false);
  if(!transport){
    testsuite_fail_if(true, "client transport initializing");
    return (void*)TEST_FAIL;
  }
  hm_data_store_client_sync_t* c=hm_data_store_client_sync_create(transport);
  if(!c){
    hm_test_transport_destroy(transport);
    testsuite_fail_if(true, "data store client sync creation");
    return (void*)TEST_FAIL;
  }
  block_t block1, block2;
  gen_new_block(&block1);
  gen_new_block(&block2);
  testsuite_fail_if(HM_SUCCESS!=hm_data_store_client_sync_call_create_block_with_id(c, block1.id, BLOCK_ID_LENGTH, block1.block, block1.block_length, block1.meta, block1.meta_length, block1.mac, block1.mac_length), "data store client sync calling");
  testsuite_fail_if(HM_SUCCESS!=hm_data_store_client_sync_call_create_block_with_id(c, block2.id, BLOCK_ID_LENGTH, block2.block, block2.block_length, block2.meta, block2.meta_length, block2.mac, block2.mac_length), "data store client sync calling");
  testsuite_fail_if(HM_SUCCESS!=hm_data_store_client_sync_call_update_block(c, block1.id, BLOCK_ID_LENGTH, block1.block, block1.block_length, block1.meta, block1.meta_length, block1.mac, block1.mac_length, block1.mac, block1.mac_length), "data store client sync calling");
  testsuite_fail_if(HM_SUCCESS==hm_data_store_client_sync_call_update_block(c, block1.id, BLOCK_ID_LENGTH, block1.block, block1.block_length, block1.meta, block1.meta_length, block1.mac, block1.mac_length, block2.mac, block2.mac_length), "data store client sync calling"); 
  testsuite_fail_if(HM_SUCCESS==hm_data_store_client_sync_call_delete_block(c, block1.id, BLOCK_ID_LENGTH, block2.mac, block2.mac_length), "data store client sync calling"); 
  testsuite_fail_if(HM_SUCCESS!=hm_data_store_client_sync_call_delete_block(c, block1.id, BLOCK_ID_LENGTH, block1.mac, block1.mac_length), "data store client sync calling"); 
  hm_data_store_client_sync_destroy(&c);
  hm_test_transport_destroy(transport);
  return (void*)TEST_SUCCESS;
}

int data_store_general_flow(){
  mkfifo(CS_PIPE_NAME, 0666);
  mkfifo(SC_PIPE_NAME, 0666);
  pthread_t client_thread;
  if(pthread_create(&client_thread, NULL, client, NULL)){
    testsuite_fail_if(true, "creating client thread");
    return TEST_FAIL;
  }
  pthread_t server_thread;
  if(pthread_create(&server_thread, NULL, server, NULL)){
    testsuite_fail_if(true, "creating server thread");
    return TEST_FAIL;
  }
  int res;
  pthread_join(client_thread, (void**)(&res));
  unlink(CS_PIPE_NAME);
  unlink(SC_PIPE_NAME);
  return res;
}

void data_store_tests(){
  testsuite_fail_if(data_store_general_flow(), "data store general flow");
}

int main(int argc, char *argv[]){
  testsuite_start_testing();
  testsuite_enter_suite("data_store test");

  testsuite_run_test(data_store_tests);

  testsuite_finish_testing();
  return testsuite_get_return_value();
}

