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


#include <hermes/rpc/transport.h>
#include <hermes/common/errors.h>
#include <hermes/rpc/param_pack.h>
#include <hermes/rpc/buffers_list.h>

#include <stdarg.h>
#include <string.h>
#include <assert.h>

struct hm_param_pack_node_type{
  uint32_t type;
  union{
    uint8_t* buf_val;
    int32_t int_val;
  } data;
  size_t data_length;
};

typedef struct hm_param_pack_node_type hm_param_pack_node_t;

#define HM_PARAM_PACK_MAX_PARAMS 20

struct hm_param_pack_type{
  size_t param_count;
  bool readed;
  uint8_t* whole_data;
  size_t whole_data_length;
  hm_param_pack_node_t nodes[HM_PARAM_PACK_MAX_PARAMS];
};

hm_param_pack_t* hm_param_pack_create(){
  hm_param_pack_t* res = calloc(1, sizeof(hm_param_pack_t));
  assert(res);
  return res;
}

hm_param_pack_t* hm_param_pack_create_(void* unused, ...){
  hm_param_pack_t* result = hm_param_pack_create();
  assert(result);
  va_list va;
  va_start(va, unused);
  uint32_t mark = va_arg(va, uint32_t);
  while(mark){
    if(HM_PARAM_PACK_MAX_PARAMS==(result->param_count) || HM_PARAM_PACK_MAGIC!=mark){
      hm_param_pack_destroy(&result);
      va_end(va);
      return NULL;
    }
    uint32_t type=va_arg(va, int32_t);
    switch(type){
    case HM_PARAM_TYPE_INT32:
      result->nodes[result->param_count].type=type;
      result->nodes[result->param_count].data.int_val=va_arg(va, int32_t);
      result->nodes[result->param_count].data_length=0;
      break;
    case HM_PARAM_TYPE_BUFFER:
    case HM_PARAM_TYPE_BUFFER_C:
      result->nodes[result->param_count].type=type;
      result->nodes[result->param_count].data.buf_val=va_arg(va, uint8_t*);
      result->nodes[result->param_count].data_length=va_arg(va, size_t);
      break;
    case HM_PARAM_TYPE_BUFFERS_LIST:
      result->nodes[result->param_count].type=type;
      result->nodes[result->param_count].data_length=hm_buffers_list_to_buf(
              va_arg(va, hm_buffers_list_t*), &(result->nodes[result->param_count].data.buf_val));
      break;
    default:
      hm_param_pack_destroy(&result);
      va_end(va);
      return NULL;      
    }
    ++(result->param_count);
    mark = va_arg(va, uint32_t);
  }
  va_end(va);
  result->readed=false;
  return result;
}

uint32_t hm_param_pack_destroy(hm_param_pack_t** pack){
  if(!pack || !(*pack)){
    return HM_INVALID_PARAMETER;
  }
  if((*pack)->readed){
    free((*pack)->whole_data);
  } else {
    while((*pack)->param_count){
      switch((*pack)->nodes[(*pack)->param_count-1].type){
      case HM_PARAM_TYPE_BUFFERS_LIST:
      case HM_PARAM_TYPE_BUFFER:
        free((*pack)->nodes[(*pack)->param_count-1].data.buf_val);
        break;
      default:
        break;
      }
      --((*pack)->param_count);
    }
  }
  free(*pack);
  return HM_SUCCESS;
}

uint32_t hm_param_pack_extract_(hm_param_pack_t* pack, ...){
  if(!pack){
    return HM_INVALID_PARAMETER;
  }
  size_t current_node=0;
  va_list va;
  va_start(va, pack);
  uint32_t mark = va_arg(va, uint32_t);
  while(mark){
    if(current_node>=(pack->param_count) || HM_PARAM_PACK_MAGIC!=mark){
      va_end(va);
      return HM_FAIL;
    }
    uint32_t type=va_arg(va, int32_t);
    if(type != pack->nodes[current_node].type){
      va_end(va);
      return HM_FAIL;      
    }
    switch(type){
    case HM_PARAM_TYPE_INT32:
      (*(va_arg(va, int32_t*)))=pack->nodes[current_node].data.int_val;
      break;
    case HM_PARAM_TYPE_BUFFERS_LIST:
      *(va_arg(va, hm_buffers_list_t**))=hm_buffers_list_extract(
              pack->nodes[current_node].data.buf_val, pack->nodes[current_node].data_length);
      break;
    case HM_PARAM_TYPE_BUFFER:
    case HM_PARAM_TYPE_BUFFER_C:{
      uint8_t** a=va_arg(va, uint8_t**);
      *a=malloc(pack->nodes[current_node].data_length);
//
      memcpy(*a, pack->nodes[current_node].data.buf_val, pack->nodes[current_node].data_length);
//
      *(va_arg(va, size_t*))=pack->nodes[current_node].data_length;
    }
      break;
    default:
      va_end(va);
      return HM_FAIL;
    }
    ++current_node;
    mark = va_arg(va, uint32_t);
  }
  va_end(va); 
  return HM_SUCCESS;
}

uint32_t hm_param_pack_get_whole_length_(const hm_param_pack_t* pack){
  if(!pack){
    return HM_INVALID_PARAMETER;
  }
  if(pack->readed){
    return pack->whole_data_length;
  }
  uint32_t whole_length=sizeof(uint32_t), current_node=0;
  while(current_node < pack->param_count){
    switch(pack->nodes[current_node].type){
    case HM_PARAM_TYPE_INT32:
      whole_length+=2*sizeof(uint32_t);
      break;
    case HM_PARAM_TYPE_BUFFERS_LIST:
    case HM_PARAM_TYPE_BUFFER:
    case HM_PARAM_TYPE_BUFFER_C:
      whole_length+=2*sizeof(uint32_t);
      whole_length+=pack->nodes[current_node].data_length;
      break;
    default:
      return 0;
    }
    ++current_node;
  }
  return whole_length;
}

uint32_t hm_param_pack_write(hm_param_pack_t* pack, uint8_t* buffer, size_t *buffer_length){
  if(!pack || !(pack->param_count)){
    return HM_INVALID_PARAMETER;
  }
  size_t needed_length = hm_param_pack_get_whole_length_(pack);
  if(!needed_length){
    return HM_INVALID_PARAMETER;
  }
  if(!buffer || (*buffer_length)<needed_length){
    (*buffer_length)=needed_length;
    return HM_BUFFER_TOO_SMALL;
  }
  size_t current_node=0;
  size_t current_position=0;
  //first 4 bytes - count of parameters
  memcpy(buffer+current_position, (uint8_t*)(&(pack->param_count)), sizeof(uint32_t));
  current_position+=sizeof(uint32_t);
  while(current_node < pack->param_count){
    switch(pack->nodes[current_node].type){
    case HM_PARAM_TYPE_INT32:
      //type
      memcpy(buffer+current_position, (uint8_t*)(&(pack->nodes[current_node].type)), sizeof(uint32_t));
      //value
      memcpy(buffer+current_position+sizeof(uint32_t),
             (uint8_t*)(&(pack->nodes[current_node].data.int_val)),
             sizeof(uint32_t));
      current_position+=2*sizeof(uint32_t);
      break;
    case HM_PARAM_TYPE_BUFFERS_LIST:
    case HM_PARAM_TYPE_BUFFER:
    case HM_PARAM_TYPE_BUFFER_C:{
      uint32_t t=HM_PARAM_TYPE_BUFFER;
      //type
      memcpy(buffer+current_position, (uint8_t*)&t, sizeof(uint32_t));
      //length
      memcpy(buffer+current_position+sizeof(uint32_t),
             (uint8_t*)&(pack->nodes[current_node].data_length),
             sizeof(uint32_t));
      //value
      memcpy(buffer+current_position+2*sizeof(uint32_t),
             pack->nodes[current_node].data.buf_val,
             pack->nodes[current_node].data_length);
      current_position+=2*sizeof(uint32_t)+(pack->nodes[current_node].data_length);
    }
      break;
    default:
      return HM_FAIL;
    }
    ++current_node;
  }
  (*buffer_length)=current_position;
  return HM_SUCCESS;
}

uint32_t hm_param_pack_send(const hm_param_pack_t* pack, hm_rpc_transport_t* transport){
  if(!pack || !(pack->param_count) || !transport || !(transport->send)){
    return HM_INVALID_PARAMETER;
  }
  uint32_t needed_length = hm_param_pack_get_whole_length_(pack);
  if(!needed_length){
    return HM_INVALID_PARAMETER;
  }
  uint32_t res = HM_SUCCESS;
  if(HM_SUCCESS!=(res=transport->send(transport->user_data, (uint8_t*)&needed_length, sizeof(uint32_t)))){
    return res;
  }
  if(HM_SUCCESS!=(res=transport->send(transport->user_data, (uint8_t*)&(pack->param_count), sizeof(uint32_t)))){
    return res;
  }
  size_t curr_node=0;
  while(curr_node<(pack->param_count)){
    switch(pack->nodes[curr_node].type){
    case HM_PARAM_TYPE_INT32://int32
      if(HM_SUCCESS!=(res=transport->send(
              transport->user_data, (uint8_t*)&(pack->nodes[curr_node].type), sizeof(uint32_t)))){
        return res;
      }
      if(HM_SUCCESS!=(res=transport->send(
              transport->user_data, (uint8_t*)&(pack->nodes[curr_node].data.int_val), sizeof(uint32_t)))){
        return res;
      }
      break;
    case HM_PARAM_TYPE_BUFFERS_LIST:
    case HM_PARAM_TYPE_BUFFER:
    case HM_PARAM_TYPE_BUFFER_C:{
      uint32_t t=HM_PARAM_TYPE_BUFFER;
      if(HM_SUCCESS!=(res=transport->send(
              transport->user_data, (uint8_t*)&t, sizeof(uint32_t)))){
        return res;
      }
      if(HM_SUCCESS!=(res=transport->send(
              transport->user_data, (uint8_t*)&(pack->nodes[curr_node].data_length), sizeof(uint32_t)))){
        return res;
      }
      if(HM_SUCCESS!=(res=transport->send(
              transport->user_data,
              (uint8_t*)(pack->nodes[curr_node].data.buf_val), pack->nodes[curr_node].data_length))){
        return res;
      }
    }
      break;
    default:
      return HM_INVALID_PARAMETER;
    }
    ++curr_node;    
  }
  return HM_SUCCESS;
}

bool hm_param_pack_check_buf_(const uint8_t* buffer, size_t buffer_length){
  const uint8_t* buffer_end = buffer+buffer_length; 
  if((buffer+sizeof(uint32_t))>buffer_end){
    return false;
  }
  uint32_t param_count=*((uint32_t*)buffer);
  buffer+=sizeof(uint32_t);
  while(param_count){
    if((buffer+sizeof(uint32_t))>buffer_end){
      return false;
    }
    uint32_t param_type = *((uint32_t*)buffer);
    buffer+=sizeof(uint32_t);
    switch(param_type){
    case HM_PARAM_TYPE_INT32:
      if((buffer+sizeof(uint32_t))>buffer_end){
        return false;
      }
      buffer+=sizeof(int32_t);
      break;
    case HM_PARAM_TYPE_BUFFERS_LIST:      
    case HM_PARAM_TYPE_BUFFER_C:
    case HM_PARAM_TYPE_BUFFER:{
      if((buffer+sizeof(uint32_t))>buffer_end){
        return false;
      }
      uint32_t buf_length=*((uint32_t*)buffer);
      buffer+=sizeof(int32_t);
      if((buffer+buf_length)>buffer_end){
        return false;
      }
      buffer+=buf_length;
    }
      break;
    default:
      return false;
    }
    --param_count;
  }
  return true;
}

hm_param_pack_t* hm_param_pack_read(uint8_t* buffer, size_t buffer_length){
  if(!buffer || !(hm_param_pack_check_buf_(buffer, buffer_length))){
    return NULL;
  }
  uint8_t *buf = buffer;
  size_t param_count=(*(uint32_t*)buf);
  if(param_count>HM_PARAM_PACK_MAX_PARAMS){
    return NULL;
  }
  buf+=sizeof(uint32_t);
  size_t curr_param=0;
  hm_param_pack_t* res = hm_param_pack_create();
  assert(res);
  while(curr_param<param_count){
    res->nodes[curr_param].type=*((uint32_t*)buf);
    buf+=sizeof(uint32_t);
    switch(res->nodes[curr_param].type){
    case HM_PARAM_TYPE_INT32:
      res->nodes[curr_param].data_length=sizeof(uint32_t);
      memcpy(&(res->nodes[curr_param].data.int_val), buf, sizeof(int32_t));
      buf+=sizeof(int32_t);
      break;
    case HM_PARAM_TYPE_BUFFERS_LIST:      
    case HM_PARAM_TYPE_BUFFER:
    case HM_PARAM_TYPE_BUFFER_C:
      res->nodes[curr_param].data_length=*((uint32_t*)buf);
      buf+=sizeof(uint32_t);
      res->nodes[curr_param].data.buf_val=buf;
      buf+=res->nodes[curr_param].data_length;
      break;
    }
    ++curr_param;
  }
  res->param_count=param_count;
  res->whole_data=buffer;
  res->whole_data_length=buffer_length;
  res->readed=true;
  return res;
}

hm_param_pack_t* hm_param_pack_receive(hm_rpc_transport_t* transport){
  if(!transport || !(transport->recv)){
    return NULL;
  }
  uint32_t length_to_read=0;
  uint32_t result=HM_SUCCESS;
  if(HM_SUCCESS!=(result=transport->recv(transport->user_data, (uint8_t*)&length_to_read, sizeof(uint32_t)))){
    return NULL;
  }
  uint8_t *buffer=malloc(length_to_read);
  assert(buffer);
  if(HM_SUCCESS!=(result=transport->recv(transport->user_data, buffer, length_to_read))){
    free(buffer);
    return NULL;
  }
  hm_param_pack_t* pack=hm_param_pack_read(buffer, length_to_read);
  if(!pack){
    free(buffer);
  }
  return pack;
}
