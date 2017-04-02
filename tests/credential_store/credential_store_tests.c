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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>

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
  hm_test_transport_destroy(transport);
  return NULL;
}

void* client(void* param){
  hm_rpc_transport_t* transport = hm_test_transport_create(CS_PIPE_NAME, SC_PIPE_NAME, false);
  if(!transport){
    testsuite_fail_if(true, "client transport initializing");
    return (void*)1;
  }
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
  testsuite_start_testing();
  testsuite_enter_suite("credential_store test");

  testsuite_run_test(credential_store_tests);

  testsuite_finish_testing();
  return testsuite_get_return_value();
}

