/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef COMMON_HBUFFER_H_
#define COMMON_HBUFFER_H_

#include "frame.h"

typedef struct hbuffer_t_ hbuffer_t;

hbuffer_t* hbuffer_create();
void hbuffer_destroy(hbuffer_t** buf);

size_t hbuffer_push(hbuffer_t* buf, hframe_t* frame);
hframe_t* hbuffer_pop(hbuffer_t* buf);

size_t hbuffer_append(hbuffer_t* buf, hframe_t* frame);
hframe_t* hbuffer_retrieve(hbuffer_t* buf);

void hbuffer_print(hbuffer_t* buf);

#endif
