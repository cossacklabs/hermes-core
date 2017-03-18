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

#include "param_pack.h"
#include <hermes/common/errors.h>
#include <themis/themis.h>
#include <assert.h>

int main(int argc, char *argv[])
{
  uint8_t *param1=NULL, *param3=NULL;
  size_t param1_length=1024, param3_length=3462;
  int32_t param2=34567;
  param1 = malloc(param1_length);
  assert(param1);
  param3 = malloc(param3_length);
  assert(param3);
  hm_param_pack_t* pack=HM_PARAM_PACK(HM_PARAM_BUFFER(param1, param1_length), HM_PARAM_INT32(param2), HM_PARAM_BUFFER(param3, param3_length));
  assert(pack);
  size_t buf_len=0;
  assert(HM_BUFFER_TOO_SMALL == hm_param_pack_write(pack, NULL, &buf_len));
  uint8_t* buf=malloc(buf_len);
  assert(buf);
  assert(HM_SUCCESS == hm_param_pack_write(pack, buf, &buf_len));

  
  uint8_t *out_param1=NULL, *out_param3=NULL;
  size_t out_param1_length=0, out_param3_length=0;
  int32_t out_param2=0;
  hm_param_pack_t* out_pack=hm_param_pack_read(buf, buf_len);
  assert(out_pack);
  assert(HM_SUCCESS==HM_PARAM_EXTRACT(out_pack,HM_PARAM_BUFFER(&out_param1, &out_param1_length), HM_PARAM_INT32(&out_param2), HM_PARAM_BUFFER(&out_param3, &out_param3_length)));
  assert(param1_length==out_param1_length);
  assert(0==memcmp(param1, out_param1, param1_length));
  assert(param2==out_param2);
  assert(param3_length==out_param3_length);
  assert(0==memcmp(param3, out_param3, param3_length));
  hm_param_pack_destroy(&out_pack);
  hm_param_pack_destroy(&pack);
  return 0;
}
