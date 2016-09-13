/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "frame.h" 

struct hframe_t_{
  uint32_t type_;
  uint32_t length_;
  union{
    uint8_t* d;
    const uint8_t* s;
  } data_;
};

hframe_t* hframe_create(){
  hframe_t* res=calloc(1, sizeof(hframe_t));
  assert(res);
  return res;
}

hframe_t* hframe_create_with_data(uint32_t type, const uint8_t* data, const size_t data_len){
  hframe_t* res=hframe_create();
  res->data_.d=malloc(data_len);
  assert(res->data_.d);
  memcpy(res->data_.d, data, data_len);
  res->length_=data_len;
  res->type_=type;
  return res;
}

void hframe_destroy(hframe_t **frame){
  assert(*frame);
  if((*frame)->data_.d!=NULL)
    free((*frame)->data_.d);
  free(*frame);
  *frame=NULL;
}

const uint8_t* hframe_get_data(hframe_t* frame){
  assert(frame);
  return frame->data_.s;
}

const size_t hframe_get_length(hframe_t* frame){
  assert(frame);
  return frame->length_;
}

const uint32_t hframe_get_type(hframe_t* frame){
  assert(frame);
  return frame->type_;
}

const char* hframe_get_str(hframe_t* frame){
  assert(frame);
  if(frame->type_=HFRAME_STR_TYPE)
    return (const char*)(frame->data_.s);
  return NULL;
}

const uint8_t* hframe_to_bytes(hframe_t* frame){

}

hframe_t* hframe_from_bytes(uint8_t* data, size_t data_len){

}
