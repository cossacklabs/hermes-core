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


#ifndef HERMES_COMMON_BUFFER_H_
#define HERMES_COMMON_BUFFER_H_

#include <stdint.h>
#include <stddef.h>
#include <hermes/common/errors.h>


#define BUFFER_FAIL HM_FAIL
#define BUFFER_SUCCESS HM_SUCCESS
#define BUFFER_INVALID_PARAM HM_INVALID_PARAMETER
#define BUFFER_BAD_ALLOC HM_BAD_ALLOC
#define BUFFER_AT_END -4
#define BUFFER_INCORRECT_BLOCK_TYPE -5
#define BUFFER_CORRUPTED -6
#define BUFFER_WRITE_READ_ONLY -7

typedef struct buffer_t_ buffer_t;

buffer_t *buffer_create();

buffer_t *buffer_create_with_(void *unused, ...);

buffer_t *buffer_create_with_data(const uint8_t *data, const size_t data_length);

int buffer_init_with_data(buffer_t *buffer, const uint8_t *data, const size_t data_length);

int buffer_destroy(buffer_t **buffer);

int buffer_push_status(buffer_t *buffer, const int status);

int buffer_push_data(buffer_t *buffer, const uint8_t *data, const size_t data_length);

int buffer_push_string(buffer_t *buffer, const char *str);

int buffer_pop_status(buffer_t *buffer, int *status);

int buffer_pop_data(buffer_t *buffer, const uint8_t **data, size_t *data_length);

int buffer_pop_string(buffer_t *buffer, const char **str);

size_t buffer_get_size(buffer_t *buffer);

const uint8_t *buffer_get_data(buffer_t *buffer);

#define HERMES_BUFFER_MAGIC 0x0000000026041900
#define HERMES_BUFFER_NODE_TYPE_STR 1
#define HERMES_BUFFER_NODE_TYPE_BIN 2
#define HERMES_BUFFER_NODE_TYPE_INT 3

#define BUFFER_NEW(...)                \
  buffer_create_with_((void*)NULL, __VA_ARGS__, (void*)NULL)

#define BUFFER_EXTRACT(data, data_length)            \
  buffer_t *tmp_buffer=buffer_create_with_data((data, data_length)

#define BUFFER_NODE_STR(str)                    \
  HERMES_BUFFER_MAGIC, HERMES_BUFFER_NODE_TYPE_STR, (str)

#define BUFFER_NODE_BIN(data, data_length)                    \
  HERMES_BUFFER_MAGIC, HERMES_BUFFER_NODE_TYPE_BIN, (data), (data_length)

#endif /* HERMES_BUFFER_H_ */
