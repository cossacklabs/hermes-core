/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef COMMON_FRAME_H_
#define COMMON_FRAME_H_

#include <string.h>
#include <stddef.h>
#include <stdint.h>

#define HFRAME_BIN_TYPE 0
#define HFRAME_STR_TYPE 1
#define HFRAME_INT_TYPE 2


typedef struct hframe_t_ hframe_t;

hframe_t* hframe_create_with_data(uint32_t type, const uint8_t* data, const size_t data_len);
void hframe_destroy(hframe_t **frame);

const uint8_t* hframe_get_data(hframe_t* frame);
const size_t hframe_get_length(hframe_t* frame);

const uint32_t hframe_get_type(hframe_t* frame);
const char* hframe_get_str(hframe_t* frame);

const uint8_t* hframe_to_bytes(hframe_t* frame);
hframe_t* hframe_from_bytes(uint8_t* data, size_t data_len);

#define HFRAME_BIN(data, data_len) hframe_create_with_data(HFRAME_BIN_TYPE, data, data_len)
#define HFRAME_STR(str) hframe_create_with_data(HFRAME_STR_TYPE, str, strlen(str)+1)
#define HFAME_INT(val) hframe_create_with_data(HFRAME_STR_TYPE, (uint8_t*)(&val), 8);

#endif /* COMMON_FRAME_H_ */
