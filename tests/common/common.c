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

#include "common.h"
#include <stdio.h>
#include <assert.h>

#ifndef _GNU_SOURCE
  // empty implementation of tdestroy
  void tdestroy(void *root, void (*free_node)(void *nodep)) {UNUSED(root);UNUSED(free_node);}
#endif //_GNU_SOURCE


void bin_array_to_hexdecimal_string(const uint8_t* in, const size_t in_length, char* out, size_t out_length){
  assert(in);
  assert(in_length);
  assert(out);
  assert(out_length==(2*in_length+1));
  size_t j;
  out[0]=0;
  for(j=0;j<in_length;++j){
    sprintf(out, "%s%02x", out, in[j]);
  }
}

void hexdecimal_string_to_bin_array(const char* in, const size_t in_length, uint8_t* out, size_t out_length){
  assert(in);
  assert(0==(in_length%2));
  assert(out);
  assert(out_length==(in_length/2));
  size_t j;
  for(j=0; j<(in_length/2); ++j) {
    unsigned int tmp;
    assert(1==sscanf(in+2*j, "%02x", &tmp));
    out[j]=tmp;
  }
}
