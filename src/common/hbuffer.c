/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "hbuffer.h"

typedef struct hbuffer_node_t_ hbuffer_node_t;

struct hbuffer_node_t_{
  hframe_t* data_;
  hbuffer_node_t* next_;
  hbuffer_node_t* prev_;
};

hbuffer_node_t* hbuffer_node_create(hframe_t* data){
  assert(data);
  hbuffer_node_t* res=calloc(1, sizeof(hbuffer_node_t));
  assert(res);
  res->data_=data;
  return res;
}

hframe_t* hbuffer_node_destroy(hbuffer_node_t** node){
  assert(*node);
  assert((*node)->data_);
  hframe_t* res=(*node)->data_;
  free(*node);
  *node=NULL;
  return res;
}

void hbuffer_node_destroy_with_data(hbuffer_node_t** node){
  assert(*node);
  assert((*node)->data_);
  hframe_destroy(&((*node)->data_));
  free(*node);
  *node=NULL;
}



struct hbuffer_t_{
  size_t length_;
  hbuffer_node_t* first_;
  hbuffer_node_t* last_;
};

hbuffer_t* hbuffer_create(){
  hbuffer_t* res=calloc(1, sizeof(hbuffer_t));
  assert(res);
  return res;
}

void hbuffer_destroy(hbuffer_t** buf){
  assert(*buf);
  hbuffer_node_t* curr_node=(*buf)->first_;
  while(curr_node){
    hbuffer_node_t* next_node=curr_node->next_;
    hbuffer_node_destroy_with_data(&curr_node);
    curr_node=next_node;
  }
  free(*buf);
}

size_t hbuffer_push(hbuffer_t* buf, hframe_t* frame){
  assert(buf);
  assert(frame);
  hbuffer_node_t* new_node=hbuffer_node_create(frame);
  new_node->next_=buf->first_;
  if(buf->first_)
    buf->first_->prev_=new_node;
  buf->first_=new_node;
  if(buf->length_==0)
    buf->last_=buf->first_;
  ++(buf->length_);
  return buf->length_;
}

hframe_t* hbuffer_pop(hbuffer_t* buf){
  assert(buf);
  hbuffer_node_t* node_to_return = buf->first_;
  if(node_to_return){
    buf->first_=node_to_return->next_;
    if(!(buf->first_))
      buf->last_=NULL;
    hframe_t* frame_to_return = hbuffer_node_destroy(&node_to_return);
    --(buf->length_);    
    return frame_to_return;
  }
  return NULL;
}

size_t hbuffer_append(hbuffer_t* buf, hframe_t* frame){
  assert(buf);
  assert(frame);
  hbuffer_node_t* new_node=hbuffer_node_create(frame);
  new_node->prev_=buf->last_;
  if(buf->last_)
    buf->last_->next_=new_node;
  buf->last_=new_node;
  if(buf->length_==0)
    buf->first_=buf->last_;
  ++(buf->length_);
  return buf->length_;
}

hframe_t* hbuffer_retrieve(hbuffer_t* buf){
  assert(buf);
  hbuffer_node_t* node_to_return = buf->last_;
  if(node_to_return){
    buf->last_=node_to_return->prev_;
    buf->last_->next_=NULL;
    hframe_t* frame_to_return = hbuffer_node_destroy(&node_to_return);
    --(buf->length_);
    return frame_to_return;
  }
  return NULL;  
}

void hbuffer_print(hbuffer_t* buf){
  assert(buf);
  hbuffer_node_t* curr_node=buf->first_;
  while(curr_node){
    fprintf(stderr, "\"%s\" ", (const char*)(hframe_get_data(curr_node->data_)));
    curr_node=curr_node->next_;
  }
  fprintf(stderr, "\n");
}

