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

#include <hermes/mid_hermes_ll/mid_hermes_ll_buffer.h>

#include <assert.h>
#include <string.h>

mid_hermes_ll_buffer_t* mid_hermes_ll_buffer_create(const uint8_t* data, const size_t length){
  mid_hermes_ll_buffer_t *b=calloc(1, sizeof(mid_hermes_ll_buffer_t));
  assert(b);
  if(!data && !length){
    return b;
  }
  b->data=malloc(length);
  assert(b->data);
  if(data){
    memcpy(b->data, data, length);
  }
  b->length=length;
  return b;
}

mid_hermes_ll_buffer_t* mid_hermes_ll_buffer_copy(mid_hermes_ll_buffer_t* src, mid_hermes_ll_buffer_t* dst){
  HERMES_CHECK_IN_PARAM_RET_NULL(src);
  HERMES_CHECK_IN_PARAM_RET_NULL(dst);
  if(HM_SUCCESS!=mid_hermes_ll_buffer_reset(dst, src->data, src->length)){
    return NULL;
  }
  return dst;
}

bool mid_hermes_ll_buffer_is_empty(mid_hermes_ll_buffer_t* b){
  assert(b);
  if(!(b->data) || !(b->length)){
    return true;
  }
  return false;
}

bool mid_hermes_ll_buffer_is_equal(mid_hermes_ll_buffer_t* b, mid_hermes_ll_buffer_t* c){
  assert(b);
  assert(c);
  if((c->length)!=(b->length) || 0!=memcmp(b->data, c->data, b->length)){
    return false;
  }
  return true;
}

hermes_status_t mid_hermes_ll_buffer_reset(mid_hermes_ll_buffer_t* b, const uint8_t* data, const size_t length){
  HERMES_CHECK_IN_PARAM(b);
  if(b->data){
    if(data){
      if(b->length<length){
        b->data=realloc(b->data, length);
        assert(b->data);
      }
      memcpy(b->data, data, length);
      b->length=length;
      return HM_SUCCESS;
    }
    free(b->data);
    b->data=NULL;
    b->length=0;
    return HM_SUCCESS;
  }
  if(data){
    b->data=malloc(length);
    b->length=length;
    return HM_SUCCESS;
  }
  b->data=NULL;
  b->length=0;
  return HM_SUCCESS;
}

hermes_status_t mid_hermes_ll_buffer_destroy(mid_hermes_ll_buffer_t** b){
  HERMES_CHECK_IN_PARAM(b);
  HERMES_CHECK_IN_PARAM(*b);
  free((*b)->data);
  free(*b);
  *b=NULL;
  return HM_SUCCESS;
}

