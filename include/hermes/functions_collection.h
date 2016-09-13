/*
 * Copyright (c) 2016 Cossack Labs Limited
 *
 * This file is part of Hermes.
 *
 * Hermes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hermes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hermes.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SRPC_SERVER_FUNCTION_COLLECTION_H_
#define SRPC_SERVER_FUNCTION_COLLECTION_H_

#include <stddef.h>
#include <stdint.h>

typedef int functions_collection_status_t;

#define FUNCTIONS_COLLECTION_SUCCESS 0
#define FUNCTIONS_COLLECTION_FAIL -1
#define FUNCTIONS_COLLECTION_INVALID_PARAM -2
#define FUNCTIONS_COLLECTION_BUFFER_TOO_SMALL -3

typedef struct{
  const char name[256]; //function name
  functions_collection_status_t (*func)(const char* id, const uint8_t* packed_params, const size_t packed_params_length, uint8_t** packed_result, size_t* packed_result_length, void* call_ctx);
} function_t;


typedef struct{
  size_t count;
  const function_t* functions;
} functions_collection_t;

functions_collection_t* functions_collection_create(const function_t* funcs, const size_t count);
void functions_collection_destroy(functions_collection_t* coll);
functions_collection_status_t functions_collection_exec(functions_collection_t* coll, const char* id, const uint8_t* params, const size_t params_length, uint8_t** result, size_t* result_length, void* call_ctx);
#endif /* SRPC_SERVER_FUNCTION_COLLECTION_H_ */
