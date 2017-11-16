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



#ifndef COMMON_H
#define COMMON_H

#define BLOCK_ID_LENGTH 32
#define USER_ID_LENGTH 32
#define MAX_MAC_LENGTH 256
#define MAX_TOKEN_LENGTH 256
#define MAX_DATA_LENGTH 1024
#define MAX_BLOCK_LENGTH 2048
#define MAX_META_LENGTH 1024

#define MAX_BLOCKS_COUNT 1024
#define MAX_USERS_PER_BLOCK 32
#define MAX_USERS 32

#define TEST_SUCCESS 0
#define TEST_FAIL 1

#define UNUSED(expr) (void)(expr)

#include <stdint.h>
#include <stdlib.h>

// this function is not available on MacOS
#ifndef _GNU_SOURCE
	void tdestroy(void *root, void (*free_node)(void *nodep));
#endif //_GNU_SOURCE

void bin_array_to_hexdecimal_string(const uint8_t* in, const size_t in_length, char* out, size_t out_length);
void hexdecimal_string_to_bin_array(const char* in, const size_t in_length, uint8_t* out, size_t out_length);

#endif //COMMON_H
