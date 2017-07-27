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
#include <signal.h>


#include <themis/themis.h>
#include "../common/common.h"
#include "../common/test_transport.h"
#include "../common/test_key_store_db.h"  

#define CS_PIPE_NAME "/tmp/hermes_core_test_ds_pipe" 
#define SC_PIPE_NAME "/tmp/hermes_core_test_sc_pipe"

void* server(void* param){
  hm_rpc_transport_t* transport = hm_test_transport_create(SC_PIPE_NAME, CS_PIPE_NAME, true);
  if(!transport){
    testsuite_fail_if(true, "server transport initializing");
    return (void*)1;
  }
  hm_ks_db_t* db=hm_test_ks_db_create();
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
  for(;i<20;++i){
    if(HM_SUCCESS!=hm_key_store_server_call(s)){
      testsuite_fail_if(true, "data store server calling");
    }
  }
  hm_key_store_server_destroy(&s);
  hm_test_ks_db_destroy(&db);
  hm_test_transport_destroy(transport);
  return NULL;
}

typedef struct token_type{
  uint8_t block_id[BLOCK_ID_LENGTH];
  uint8_t user_id[USER_ID_LENGTH];
  uint8_t owner_id[USER_ID_LENGTH];
  uint8_t token[MAX_TOKEN_LENGTH];
}token_t;

void gen_new_token(token_t* token){
  assert(token);
  assert(SOTER_SUCCESS==soter_rand(token->block_id, sizeof(token->block_id)));
  assert(SOTER_SUCCESS==soter_rand(token->user_id, sizeof(token->user_id)));
  assert(SOTER_SUCCESS==soter_rand(token->owner_id, sizeof(token->owner_id)));
  assert(SOTER_SUCCESS==soter_rand(token->token, sizeof(token->token)));
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
  token_t rtoken, wtoken;
  gen_new_token(&rtoken);
  gen_new_token(&wtoken);
  //try to add self owned wtoken without rtoken added
  if(HM_SUCCESS==hm_key_store_client_sync_call_set_wtoken(c,
                                                          rtoken.block_id, sizeof(rtoken.block_id),
                                                          rtoken.user_id, sizeof(rtoken.user_id),
                                                          rtoken.user_id, sizeof(rtoken.user_id),
                                                          rtoken.token, sizeof(rtoken.token))){ 
    testsuite_fail_if(true, "key store client sync calling"); 
  } 
  //try to add not owned token for new block 
  if(HM_SUCCESS==hm_key_store_client_sync_call_set_rtoken(c,
                                                          rtoken.block_id, sizeof(rtoken.block_id),
                                                          rtoken.user_id, sizeof(rtoken.user_id),
                                                          rtoken.owner_id, sizeof(rtoken.owner_id),
                                                          rtoken.token, sizeof(rtoken.token))){ 
     testsuite_fail_if(true, "key store client sync calling"); 
  }
  //add owned new rtoken for new block
  if(HM_SUCCESS!=hm_key_store_client_sync_call_set_rtoken(c,
                                                          rtoken.block_id, sizeof(rtoken.block_id),
                                                          rtoken.user_id, sizeof(rtoken.user_id),
                                                          rtoken.user_id, sizeof(rtoken.user_id),
                                                          rtoken.token, sizeof(rtoken.token))){ 
    testsuite_fail_if(true, "key store client sync calling"); 
  } 
  //try to add owned wtoken for different user, owner have not update rights  
  if(HM_SUCCESS==hm_key_store_client_sync_call_set_wtoken(c,
                                                          rtoken.block_id, sizeof(rtoken.block_id),
                                                          wtoken.user_id, sizeof(wtoken.user_id),
                                                          rtoken.user_id, sizeof(rtoken.user_id),
                                                          rtoken.token, sizeof(rtoken.token))){ 
    testsuite_fail_if(true, "key store client sync calling"); 
  } 
  //add self owned wtoken
  if(HM_SUCCESS!=hm_key_store_client_sync_call_set_wtoken(c,
                                                          rtoken.block_id, sizeof(rtoken.block_id),
                                                          rtoken.user_id, sizeof(rtoken.user_id),
                                                          rtoken.user_id, sizeof(rtoken.user_id),
                                                          rtoken.token, sizeof(rtoken.token))){ 
    testsuite_fail_if(true, "key store client sync calling"); 
  }

  //try to grant access to unaccessed token
  if(HM_SUCCESS==hm_key_store_client_sync_call_set_rtoken(c,
                                                          wtoken.block_id, sizeof(wtoken.block_id),
                                                          rtoken.user_id, sizeof(rtoken.user_id),
                                                          wtoken.user_id, sizeof(wtoken.user_id),
                                                          rtoken.token, sizeof(rtoken.token))){ 
    testsuite_fail_if(true, "key store client sync calling"); 
  }

  //grant read access
  if(HM_SUCCESS!=hm_key_store_client_sync_call_set_rtoken(c,
                                                          rtoken.block_id, sizeof(rtoken.block_id),
                                                          wtoken.user_id, sizeof(wtoken.user_id),
                                                          rtoken.user_id, sizeof(rtoken.user_id),
                                                          rtoken.token, sizeof(rtoken.token))){ 
    testsuite_fail_if(true, "key store client sync calling"); 
  }

  //grant read access only having read access
  if(HM_SUCCESS!=hm_key_store_client_sync_call_set_rtoken(c,
                                                          rtoken.block_id, sizeof(rtoken.block_id),
                                                          rtoken.user_id, sizeof(rtoken.user_id),
                                                          wtoken.user_id, sizeof(wtoken.user_id),
                                                          rtoken.token, sizeof(rtoken.token))){ 
    testsuite_fail_if(true, "key store client sync calling"); 
  }
  //try to grant update access only having read access
  if(HM_SUCCESS==hm_key_store_client_sync_call_set_wtoken(c,
                                                          rtoken.block_id, sizeof(rtoken.block_id),
                                                          rtoken.user_id, sizeof(rtoken.user_id),
                                                          wtoken.user_id, sizeof(wtoken.user_id),
                                                          rtoken.token, sizeof(rtoken.token))){ 
    testsuite_fail_if(true, "key store client sync calling"); 
  }
  //try to revoke update access only having read access
  if(HM_SUCCESS==hm_key_store_client_sync_call_del_wtoken(c,
                                                          rtoken.block_id, sizeof(rtoken.block_id),
                                                          rtoken.user_id, sizeof(rtoken.user_id),
                                                          wtoken.user_id, sizeof(wtoken.user_id))){
    testsuite_fail_if(true, "key store client sync calling"); 
  }
  //revoke read access 
  if(HM_SUCCESS!=hm_key_store_client_sync_call_del_rtoken(c,
                                                          rtoken.block_id, sizeof(rtoken.block_id),
                                                          wtoken.user_id, sizeof(wtoken.user_id),
                                                          rtoken.user_id, sizeof(rtoken.user_id))){
    testsuite_fail_if(true, "key store client sync calling"); 
  }
  //grant read access
  if(HM_SUCCESS!=hm_key_store_client_sync_call_set_rtoken(c,
                                                          rtoken.block_id, sizeof(rtoken.block_id),
                                                          wtoken.user_id, sizeof(wtoken.user_id),
                                                          rtoken.user_id, sizeof(rtoken.user_id),
                                                          rtoken.token, sizeof(rtoken.token))){ 
    testsuite_fail_if(true, "key store client sync calling"); 
  }
  //grant update access
  if(HM_SUCCESS!=hm_key_store_client_sync_call_set_rtoken(c,
                                                          rtoken.block_id, sizeof(rtoken.block_id),
                                                          wtoken.user_id, sizeof(wtoken.user_id),
                                                          rtoken.user_id, sizeof(rtoken.user_id),
                                                          rtoken.token, sizeof(rtoken.token))){ 
    testsuite_fail_if(true, "key store client sync calling"); 
  }


  
  

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
  pthread_join(client_thread, (void**)(&res2));
  pthread_join(server_thread, (void**)(&res1));
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

