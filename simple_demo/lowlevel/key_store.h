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


#ifndef FILE_KEY_STORE_H
#define FILE_KEY_STORE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define KS_SUCCESS 0
#define KS_FAIL 1

typedef struct key_store_type key_store_t;

key_store_t* key_store_create();

uint32_t key_store_get_rtoken(key_store_t* ks,
                              const uint8_t* user_id,
                              const size_t user_id_length,
                              const uint8_t* block_id,
                              const size_t block_id_length,
                              uint8_t** token,
                              size_t* token_length,
                              uint8_t** owner_id,
                              size_t* owner_id_length);

uint32_t key_store_get_wtoken(key_store_t* ks,
                              const uint8_t* user_id,
                              const size_t user_id_length,
                              const uint8_t* block_id,
                              const size_t block_id_length,
                              uint8_t** token,
                              size_t* token_length,
                              uint8_t** owner_id,
                              size_t* owner_id_length);

uint32_t key_store_set_rtoken(key_store_t* ks,
                              const uint8_t* user_id,
                              const size_t user_id_length,
                              const uint8_t* block_id,
                              const size_t block_id_length,
                              const uint8_t* token,
                              const size_t token_length,
                              const uint8_t* owner_id,
                              const size_t owner_id_length);

uint32_t key_store_set_wtoken(key_store_t* ks,
                              const uint8_t* user_id,
                              const size_t user_id_length,
                              const uint8_t* block_id,
                              const size_t block_id_length,
                              const uint8_t* token,
                              const size_t token_length,
                              const uint8_t* owner_id,
                              const size_t owner_id_length);

uint32_t key_store_destroy(key_store_t** ks);

typedef struct key_store_iterator_type key_store_iterator_t;

key_store_iterator_t* key_store_iterator_create(key_store_t* ks, const uint8_t* block_id, const size_t block_id_length);

uint32_t key_store_iterator_next(key_store_iterator_t* i);

uint8_t* key_store_iterator_get_user_id(key_store_iterator_t* i);
size_t key_store_iterator_get_user_id_length(key_store_iterator_t* i);
uint8_t* key_store_iterator_get_token(key_store_iterator_t* i);
size_t key_store_iterator_get_token_length(key_store_iterator_t* i);
bool key_store_iterator_get_token_type(key_store_iterator_t* i);

uint32_t key_store_iterator_destroy(key_store_iterator_t** i);


#endif //FILE_KEY_STORE_H
