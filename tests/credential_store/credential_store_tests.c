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

void* server(void* param){
  hm_rpc_transport_t* transport = hm_test_transport_create(SC_PIPE_NAME, CS_PIPE_NAME, true);
  if(!transport){
    testsuite_fail_if(true, "server transport initializing");
    return (void*)1;
  }
  hm_cs_db_t* db=hm_test_cs_db_create();
  if(!db){
    hm_test_transport_destroy(transport);
    return (void*)1;
  }
  hm_credential_store_server_t* s=hm_credential_store_server_create(transport, db);
  if(!s){
    hm_test_cs_db_destroy(&db);
    hm_test_transport_destroy(transport);
    testsuite_fail_if(true, "credential store server creation");
    return (void*)1;
  }
  int i=0;
  for(;i<3*MAX_USERS;++i){
    if(HM_SUCCESS!=hm_credential_store_server_call(s)){
      testsuite_fail_if(true, "credential store server calling");
    }
  }
  hm_credential_store_server_destroy(&s);
  hm_test_cs_db_destroy(&db);
  hm_test_transport_destroy(transport);
  return NULL;
}

void* client(void* param){
  hm_rpc_transport_t* transport = hm_test_transport_create(CS_PIPE_NAME, SC_PIPE_NAME, false);
  if(!transport){
    testsuite_fail_if(true, "client transport initializing");
    return (void*)1;
  }
  hm_credential_store_client_sync_t* c=hm_credential_store_client_sync_create(transport);
  if(!c){
    hm_test_transport_destroy(transport);
    testsuite_fail_if(true, "credential store client sync creation");
    return (void*)1;
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
    char user_id[USER_ID_LENGTH];
    char command[256];
    sprintf(command, "find . -maxdepth 1 -name \"*.priv\" | sed '%iq;d'", i);
    FILE* f=popen(command, "r");
    if(f){
      if(fgets(command, sizeof(command), f)){
        uint8_t user_id[USER_ID_LENGTH];
        int j=0;
	//convert hexdecimal string to binary array of USER_ID_LENGTH bytes
        for(; j<USER_ID_LENGTH; ++j) {
	  //by default path returned by "find" begins with "./", we need to skip them.
          sscanf(command+2+2*j, "%02x", (unsigned int*)(&(user_id[j])));
        }
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
  return NULL;
}

int credential_store_general_flow(){
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

void credential_store_tests(){
  testsuite_fail_if(credential_store_general_flow(), "credential store general flow");
}

int main(int argc, char *argv[]){
  system("rm *.priv");
  testsuite_start_testing();
  testsuite_enter_suite("credential_store test");

  testsuite_run_test(credential_store_tests);

  testsuite_finish_testing();
  return testsuite_get_return_value();
}

