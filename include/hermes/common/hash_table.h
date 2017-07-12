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


#ifndef HM_COMMON_HASH_TABLE_H
#define HM_COMMON_HASH_TABLE_H

#include <stdint.h>
#include <stdlib.h>

typedef struct hm_hash_table_type hm_hash_table_t;

hm_hash_table_t* hm_hash_table_create();
uint32_t hm_hash_table_destroy(hm_hash_table_t** table);

uint32_t hm_hash_table_set(hm_hash_table_t* table, const uint8_t* key, const size_t key_length, const uint8_t* val, const size_t val_length);
uint32_t hm_hash_table_get(hm_hash_table_t* table, const uint8_t* key, const size_t key_length, uint8_t** val, size_t* val_length);

#endif //HM_COMMON_HASH_TABLE_H
