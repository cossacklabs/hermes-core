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


static void param_pack_general_flow(){
  uint8_t *param1=NULL, *param3=NULL;
  size_t param1_length=0, param3_length=0;
  int32_t param2=0;
  testsuite_fail_if(THEMIS_SUCCESS!=soter_rand((uint8_t*)(&param1_length), 2), "param generation");
  testsuite_fail_if(THEMIS_SUCCESS!=soter_rand((uint8_t*)(&param2), sizeof(uint32_t)), "param generation");
  testsuite_fail_if(THEMIS_SUCCESS!=soter_rand((uint8_t*)(&param3_length), 2), "param generation");
  param1 = malloc(param1_length);
  testsuite_fail_if(!param1, "memory allocation");
  param3 = malloc(param3_length);
  testsuite_fail_if(!param3, "memory allocation");
  hm_param_pack_t* pack=HM_PARAM_PACK(HM_PARAM_BUFFER(param1, param1_length), HM_PARAM_INT32(param2), HM_PARAM_BUFFER(param3, param3_length));
  testsuite_fail_if(!pack, "param pack object creation");
  size_t buf_len=0;
  if(HM_BUFFER_TOO_SMALL != hm_param_pack_write(pack, NULL, &buf_len)){
    testsuite_fail_if(true, "param pack writing buffer (length determination)");
    return;
  }
  uint8_t* buf=malloc(buf_len);
  testsuite_fail_if(!buf, "not enough memeory");
  if(HM_SUCCESS != hm_param_pack_write(pack, buf, &buf_len)){
    testsuite_fail_if(true, "param pack writing");
    return;
  }
  uint8_t *out_param1=NULL, *out_param3=NULL;
  size_t out_param1_length=0, out_param3_length=0;
  int32_t out_param2=0;
  hm_param_pack_t* out_pack=hm_param_pack_read(buf, buf_len);
  testsuite_fail_if(!out_pack, "param pack reading");
  testsuite_fail_if(HM_SUCCESS!=HM_PARAM_EXTRACT(out_pack,HM_PARAM_BUFFER(&out_param1, &out_param1_length), HM_PARAM_INT32(&out_param2), HM_PARAM_BUFFER(&out_param3, &out_param3_length)), "param pack extracting error");
  testsuite_fail_if(param1_length!=out_param1_length, "in and out param equal");
  testsuite_fail_if(0!=memcmp(param1, out_param1, param1_length), "in and out param equal");
  testsuite_fail_if(param2!=out_param2, "in and out param equal");
  testsuite_fail_if(param3_length!=out_param3_length, "in and out param equal");
  testsuite_fail_if(0!=memcmp(param3, out_param3, param3_length), "in and out param equal");
  hm_param_pack_destroy(&out_pack);
  hm_param_pack_destroy(&pack);
}


int main(int argc, char *argv[]){
  testsuite_start_testing();
  testsuite_enter_suite("param pack test");
  testsuite_run_test(param_pack_general_flow);
  testsuite_finish_testing();
  return testsuite_get_return_value();
}
