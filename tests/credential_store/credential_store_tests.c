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

#include <hermes/credential_store/server.h>
#include <hermes/credential_store/client.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include <pthread.h>
#include <assert.h>

#include <themis/themis.h>
#include "../common/common.h"
#include "../common/test_transport.h"
#include "../common/test_credential_store_db.h"  

#define CS_PIPE_NAME "/tmp/hermes_core_test_cs_pipe" 
#define SC_PIPE_NAME "/tmp/hermes_core_test_sc_pipe"

#define MAX_COMMAND_LENGTH 256

void* server(void* param){
  UNUSED(param);
  hm_rpc_transport_t* transport = hm_test_transport_create(SC_PIPE_NAME, CS_PIPE_NAME, true);
  if(!transport){
    testsuite_fail_if(true, "server transport initializing");
    return (void*)TEST_FAIL;
  }
  hm_cs_db_t* db=hm_test_cs_db_create();
  if(!db){
    hm_test_transport_destroy(transport);
    return (void*)TEST_FAIL;
  }
  hm_credential_store_server_t* s=hm_credential_store_server_create(transport, db);
  if(!s){
    hm_test_cs_db_destroy(&db);
    hm_test_transport_destroy(transport);
    testsuite_fail_if(true, "credential store server creation");
    return (void*)TEST_FAIL;
  }
  for(;;){
    if(HM_SUCCESS!=hm_credential_store_server_call(s)){
      testsuite_fail_if(true, "credential store server calling");
    }
  }
  hm_credential_store_server_destroy(&s);
  hm_test_cs_db_destroy(&db);
  hm_test_transport_destroy(transport);
  return (void*)TEST_SUCCESS;
}

void* client(void* param){
  UNUSED(param);
  hm_rpc_transport_t* transport = hm_test_transport_create(CS_PIPE_NAME, SC_PIPE_NAME, false);
  if(!transport){
    testsuite_fail_if(true, "client transport initializing");
    return (void*)TEST_FAIL;
  }
  hm_credential_store_client_sync_t* c=hm_credential_store_client_sync_create(transport);
  if(!c){
    hm_test_transport_destroy(transport);
    testsuite_fail_if(true, "credential store client sync creation");
    return (void*)TEST_FAIL;
  }
  sleep(1);
  uint8_t* key=NULL;
  size_t key_length=0;
  int i=1;
  while(i<=MAX_USERS){
    uint8_t user_id[USER_ID_LENGTH];
    assert(SOTER_SUCCESS==soter_rand(user_id, sizeof(user_id)));
    hermes_status_t res=hm_credential_store_client_sync_call_get_pub_key_by_id(c, user_id, USER_ID_LENGTH, &key, &key_length);
    testsuite_fail_if(HM_SUCCESS==res, "credential store client sync calling"); 
    if(HM_SUCCESS==res){
      free(key);
    }
    ++i;
  }
  i=1;
  while(i<=MAX_USERS){
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "find . -maxdepth 1 -name \"*.priv\" | sed '%iq;d' | awk '{print substr($0, 3, 2*%i)}'", i, USER_ID_LENGTH);
    FILE* f=popen(command, "r");
    if(f){
      if(fgets(command, sizeof(command), f)){
        uint8_t user_id[USER_ID_LENGTH];
        hexdecimal_string_to_bin_array(command, 2*USER_ID_LENGTH, user_id, USER_ID_LENGTH);
        hermes_status_t res=hm_credential_store_client_sync_call_get_pub_key_by_id(c, user_id, USER_ID_LENGTH, &key, &key_length);
        testsuite_fail_if(HM_SUCCESS!=res, "credential store client sync calling"); 
        if(HM_SUCCESS==res){
          free(key);
        }
      }
      pclose(f);
    }
    ++i;
  }
  i=1;
  while(i<=MAX_USERS){
    uint8_t user_id[USER_ID_LENGTH];
    assert(SOTER_SUCCESS==soter_rand(user_id, sizeof(user_id)));
    hermes_status_t res=hm_credential_store_client_sync_call_get_pub_key_by_id(c, user_id, USER_ID_LENGTH, &key, &key_length);
    testsuite_fail_if(HM_SUCCESS==res, "credential store client sync calling"); 
    if(HM_SUCCESS==res){
      free(key);
    }
    ++i;
  }
  hm_credential_store_client_sync_destroy(&c);
  hm_test_transport_destroy(transport);
  return (void*)TEST_SUCCESS;
}

int credential_store_general_flow(void){
  mkfifo(SC_PIPE_NAME, 0666);
  mkfifo(CS_PIPE_NAME, 0666);
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
  unlink(SC_PIPE_NAME);
  unlink(CS_PIPE_NAME);
  return res;
}

void credential_store_tests(void){
  testsuite_fail_if(credential_store_general_flow() != TEST_SUCCESS, "credential store general flow");
}

int main(int argc, char *argv[]){
  UNUSED(argc);
  UNUSED(argv);
  system("rm *.priv");
  testsuite_start_testing();
  testsuite_enter_suite("credential_store test");

  testsuite_run_test(credential_store_tests);

  testsuite_finish_testing();
  return testsuite_get_return_value();
}

