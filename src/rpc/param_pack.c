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


#include <hermes/common/errors.h>
#include <hermes/common/param_pack.h>


#include <stdarg.h>
#include <assert.h>

struct hm_param_pack_node_type{
  uint32_t type;
  uint8_t* data;
  size_t data_length;
};

typedef struct hm_param_pack_node_type hm_param_pack_node_t;

#define HM_PARAM_PACK_MAX_PARAMS 20

struct hm_param_pack_type{
  size_t params_count;
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
  hm_param_pack_t* res = hm_param_pack_create();
  assert(res);
  va_list va;
  va_start(va, unused);
  char* mark = va_arg(va, char*);
  while(mark !=NULL){
    if(HM_PARAM_PACK_MAX_PARAMS==(res->param_count) || HM_PARAM_PACK_MAGIC!=(uint64_t)mark){
      hm_param_pack_destroy(&res);
      va_end(va);
      return NULL;
    }
    uint64_t type=va_arg(va, int64_t);
    switch(type){
    case HM_PARAM_TYPE_INT32:
      res->nodes[res->param_count].type=type;
      res->nodes[res->param_count].data=(uint8_t*)(va_arg(va, int32_t));
      res->nodes[res->param_count].data_length=0;
      break;
    case HM_PARAM_TYPE_BUFFER:
      res->nodes[res->param_count].type=type;
      res->nodes[res->param_count].data=va_arg(va, uint8_t*);
      res->nodes[res->param_count].data_length=va_arg(va, size_t);
      break;
    default:
      hm_param_pack_destroy(&res);
      va_end(va);
      return NULL;      
    }
    ++(res->param_count);
  }
  return res;
}

uint32_t hm_param_pack_destroy(hm_param_pack_t** p){
  if(!p || !(*p)){
    return HM_INVALID_PARAM;
  }
  if((*p)->readed){
    free((*p)->whole_data);
  } else {
    while(0!=((*p)->param_count)){
      switch((*p)->nodes[(*p)->param_count-1].type){
      case HM_PARAM_TYPE_BUFFER:
        free((*p)->nodes[(*p)->param_count-1].data);
        break;
      default:
        break;
      }
    }
  }
  free(*p);
  return HM_SUCCESS;
}

uint32_t hm_param_pack_extract_(hm_param_pack_t* p, ...){
  if(!p){
    return HM_INVALID_PARAM;
  }
  size_t curr_node=0;
  va_list va;
  va_start(va, p);
  char* mark = va_arg(va, char*);
  while(mark !=NULL){
    if(curr_node>=(res->param_count) || HM_PARAM_PACK_MAGIC!=(uint64_t)mark){
      va_end(va);
      return HM_FAIL;
    }
    uint64_t type=va_arg(va, int64_t);
    if(type != p->nodes[curr_node].type){
      va_end(va);
      return HM_FAIL;      
    }
    switch(type){
    case HM_PARAM_TYPE_INT32:
      (*(va_arg(va, int32_t*)))=(uint32_t)(p->nodes[curr_node].data);
      break;
    case HM_PARAM_TYPE_BUFFER:
      *(va_arg(va, uint8_t**))=p->nodes[curr_data].data;
      *(va_arg(va, size_t*))=p->nodes[curr_data].data_length;
      break;
    default:
      hm_param_pack_destroy(&res);
      va_end(va);
      return NULL;      
    }
    ++curr_node;
  }
  return res;
}

size_t hm_param_pack_get_whole_length(hm_param_pack_t* p){
  if(!p){
    return HM_INALID_PARAM;
  }
  if(p->readed){
    return p->whole_data_length;
  }
  size_t whole_length=0, curr_node=0;
  while(curr_node < p->param_count){
    switch(p->nodes[curr_node].type){
    case HM_PARAM_TYPE_INT32:
      whole_length+=8;
    case HM_PARAM_TYPE_BUFFER:
      whole_length+=p->nodes[curr_node].data_length;
      break;
    default:
      return 0;
    }
  }
  return whole_length;
}

uint32_t hm_param_pack_write(hm_param_pack_t* p, uint8_t* buffer, size_t *buffer_length){
  if(!p || !(p->param_count)){
    return HM_INVALID_PARAM;
  }
  size_t needed_length = hm_param_get_whole_length(p);
  if(!needed_length){
    return HM_INVALID_PARAM;
  }
  if(!buffer || (*buffer_length)<needed_length){
    (*buffer_length)=needed_length;
    return HM_BUFFER_TOO_SMALL;
  }
  size_t curr_node=0;
  size_t curr_pos=0;
  memcpy_s(buffer+curr_pos, (uint8_t)(&(p->param_count)), 4); //first 4 bytes - count of parameters 
  curr_pos+=4;
  while(curr_node < p->param_count){
    switch(p->nodes[curr_node].type){
    case HM_PARAM_TYPE_INT32://int32
      memcpy_s(buffer+curr_pos, (uint8_t*)(&(p->nodes[curr_node].type)), 4); //type      
      memcpy_s(buffer+curr_pos+4, (uint8_t*)(&(p->nodes[curr_node].data)), 4); //value
      curr_pos+=8;
      break;
    case HM_PARAM_TYPE_BUFFER:
      memcpy_s(buffer+curr_pos, (uint8_t*)&(p->nodes[curr_node].type), 4);      //type
      memcpy_s(buffer+curr_pos+4, (uint32_t)&(p->nodes[curr_node].data_length), 4);//length
      memcpy_s(buffer+curr_pos+8, p->nodes[curr_node].data, p->nodes[curr_node].data_length);//value
      curr_pos+=8+(p->nodes[curr_node].data_length);
      break;
    default:
      return HM_FAIL;
    }
  }
  (*buffer_length)=curr_pos;
  return HM_SUCCESS;
}

hm_param_pack_t* hm_param_pack_read(const uint8_t* buffer, size_t buffer_length){
  if(!biffer || buffer_length<8){
    return NULL;
  }
  
}
