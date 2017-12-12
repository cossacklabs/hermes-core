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



#include <hermes/mid_hermes_ll/mid_hermes_ll_buffer.h>

#include <assert.h>
#include <string.h>

mid_hermes_ll_buffer_t* mid_hermes_ll_buffer_create(const uint8_t* data, const size_t length){
  mid_hermes_ll_buffer_t *buffer=calloc(1, sizeof(mid_hermes_ll_buffer_t));
  assert(buffer);
  if(!data && !length){
    return buffer;
  }
  buffer->data=malloc(length);
  assert(buffer->data);
  if(data){
    memcpy(buffer->data, data, length);
  }
  buffer->length=length;
  return buffer;
}

mid_hermes_ll_buffer_t* mid_hermes_ll_buffer_copy(mid_hermes_ll_buffer_t* src, mid_hermes_ll_buffer_t* dst){
  HERMES_CHECK_IN_PARAM_RET_NULL(src);
  HERMES_CHECK_IN_PARAM_RET_NULL(dst);
  if(HM_SUCCESS!=mid_hermes_ll_buffer_reset(dst, src->data, src->length)){
    return NULL;
  }
  return dst;
}

bool mid_hermes_ll_buffer_is_empty(mid_hermes_ll_buffer_t* buffer){
  assert(buffer);
  if(!(buffer->data) || !(buffer->length)){
    return true;
  }
  return false;
}

bool mid_hermes_ll_buffer_is_equal(mid_hermes_ll_buffer_t* buffer1, mid_hermes_ll_buffer_t* buffer2){
  assert(buffer1);
  assert(buffer2);
  if((buffer2->length)!=(buffer1->length) || 0!=cst_time_memcmp(buffer1->data, buffer2->data, buffer1->length)){
    return false;
  }
  return true;
}

// mid_hermes_ll_buffer_reset if data is null then buffer->data will be freed (if not null) and set null, length=0
// otherwise data will be copied to buffer->data (buffer->data will be reallocated if buffer->length != length)
// and buffer->length=length
hermes_status_t mid_hermes_ll_buffer_reset(mid_hermes_ll_buffer_t* buffer, const uint8_t* data, const size_t length){
  HERMES_CHECK_IN_PARAM(buffer);
  if(buffer->data){
    if(data){
      if(buffer->length<length){
        buffer->data=realloc(buffer->data, length);
        assert(buffer->data);
      }
      memcpy(buffer->data, data, length);
      buffer->length=length;
      return HM_SUCCESS;
    }
    free(buffer->data);
    buffer->data=NULL;
    buffer->length=0;
    return HM_SUCCESS;
  }
  if(data){
    buffer->data=malloc(length);
    buffer->length=length;
    memcpy(buffer->data, data, length);
    return HM_SUCCESS;
  }
  buffer->data=NULL;
  buffer->length=0;
  return HM_SUCCESS;
}

hermes_status_t mid_hermes_ll_buffer_destroy(mid_hermes_ll_buffer_t** buffer){
  HERMES_CHECK_IN_PARAM(buffer);
  HERMES_CHECK_IN_PARAM(*buffer);
  free((*buffer)->data);
  (*buffer)->data=NULL;
  free(*buffer);
  *buffer=NULL;
  return HM_SUCCESS;
}

hermes_status_t mid_hermes_ll_buffer_destroy_secure(mid_hermes_ll_buffer_t** buffer){
  HERMES_CHECK_IN_PARAM(buffer);
  HERMES_CHECK_IN_PARAM(*buffer);
  memset(((*buffer)->data), 0, (*buffer)->length);
  (*buffer)->length=0;
  free((*buffer)->data);
  (*buffer)->data=NULL;
  free(*buffer);
  *buffer=NULL;
  return HM_SUCCESS;
}


