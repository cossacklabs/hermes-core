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

#include <hermes/rpc/buffers_list.h>

#include <stdarg.h>
#include <assert.h>
#include <string.h>

hm_buffers_list_t* hm_buffers_list_create(){
  hm_buffers_list_t* l=calloc(1, sizeof(hm_buffers_list_t));
  assert(l);
  return l;
}

hm_buffers_list_t* hm_buffers_list_extract(const uint8_t* data, const size_t length){
  if(!data || !length){
    return NULL;
  }
  hm_buffers_list_t* bl=hm_buffers_list_create();
  const uint8_t* end=(data+length);
  while(data!=end){
    if((data+sizeof(uint32_t))>end){
      hm_buffers_list_destroy(&bl);
      return NULL;
    }
    uint32_t len=(*((uint32_t*)data));
    if((data+sizeof(uint32_t)+len)>end
       || (HM_SUCCESS!=hm_buffers_list_add_c(bl, data+sizeof(uint32_t), len))){
      hm_buffers_list_destroy(&bl);
      return NULL;
    }
    data+=sizeof(uint32_t)+len;    
  }
  return bl;
}

size_t hm_buffres_list_get_needed_buffer_size(hm_buffers_list_t* l){
  if(!l || !(l->first)){
    return 0;
  }
  hm_buffers_list_node_t* curr=l->first;
  size_t res=0;
  while(curr){
    res+=sizeof(uint32_t);
    res+=curr->length;
  }
  return res;
}

size_t hm_buffers_list_to_buf(hm_buffers_list_t* l, uint8_t** data){
  size_t res=0;
  if(!l || !data || !(res= hm_buffres_list_get_needed_buffer_size(l))){
    return 0;
  }
  *data=malloc(res);
  assert(*data);
  uint8_t* curr_pos=*data;
  hm_buffers_list_node_t* curr=l->first;
  while(curr){
    *((uint32_t*)curr_pos)=(uint32_t)(curr->length);
    memcpy(curr_pos+sizeof(uint32_t), curr->data, curr->length);
    curr_pos+=sizeof(uint32_t)+curr->length;
  }
  return res;
}


hm_buffers_list_node_t* hm_buffers_list_get_last(hm_buffers_list_t* l){
  if(!l || !(l->first)){
    return NULL;
  }
  hm_buffers_list_node_t* last=l->first;
  while(last->next){
    last=last->next;
  }
  return last;
}

hermes_status_t hm_buffers_list_add(hm_buffers_list_t* l, uint8_t* data, size_t length){
  if(!l || !data || !length){
    return HM_INVALID_PARAMETER;
  }
  hm_buffers_list_node_t* new=calloc(1, sizeof(hm_buffers_list_node_t));
  assert(new);
  new->data=data;
  new->length=length;
  hm_buffers_list_node_t* last=hm_buffers_list_get_last(l);
  if(!last){
    l->first=new;
  }else{
    last->next=new;
  }
  return HM_SUCCESS;
}

hermes_status_t hm_buffers_list_add_c(hm_buffers_list_t* l, const uint8_t* data, const size_t length){
  if(!l || !data || !length){
    return HM_INVALID_PARAMETER;
  }
  hm_buffers_list_node_t* new=calloc(1, sizeof(hm_buffers_list_node_t));
  assert(new);
  new->data=malloc(length);
  assert(new->data);
  memcpy(new->data, data, length);
  new->length=length;
  hm_buffers_list_node_t* last=hm_buffers_list_get_last(l);
  if(!last){
    l->first=new;
  }else{
    last->next=new;
  }
  return HM_SUCCESS;
}

hermes_status_t hm_buffers_list_destroy(hm_buffers_list_t** l){
  if(!l || !(*l)){
    return HM_INVALID_PARAMETER;
  }
  int i=0;
  hm_buffers_list_node_t* curr=(*l)->first;
  while(curr){
    free(curr->data);
    hm_buffers_list_node_t* tmp=curr->next;
    free(curr);
    curr=tmp;
  }
  free(*l);
  *l=NULL;
  return HM_SUCCESS;
}

const uint8_t* hm_buffers_list_iterator_get_data(hm_buffers_list_iterator_t* i){
  if(!i || !(i->curr)){
    return NULL;
  }
  return i->curr->data;
}

const size_t hm_buffers_list_iterator_get_size(hm_buffers_list_iterator_t* i){
  if(!i || !(i->curr)){
    return 0;
  }
  return i->curr->length;
}

bool hm_buffers_list_iterator_next(hm_buffers_list_iterator_t* i){
  if(!i || !(i->curr)){
    return false;
  }
  i->curr=i->curr->next;
  return true;
}

hm_buffers_list_iterator_t* hm_buffers_list_iterator_create(hm_buffers_list_t* l){
  if(!l || !(l->first)){
    return NULL;
  }
  hm_buffers_list_iterator_t* i=calloc(1, sizeof(hm_buffers_list_iterator_t));
  assert(i);
  i->curr=l->first;
  i->data=hm_buffers_list_iterator_get_data;
  i->size=hm_buffers_list_iterator_get_size;
  i->next=hm_buffers_list_iterator_next;
  return i;
}

hermes_status_t hm_buffers_list_iterator_destroy(hm_buffers_list_iterator_t** i){
  if(!i || !(*i)){
    return HM_INVALID_PARAMETER;
  }
  free(*i);
  *i=NULL;
  return HM_SUCCESS;
}
