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


#ifndef HERMES_KEY_STORE_DB_H
#define HERMES_KEY_STORE_DB_H


#include <stdint.h>
#include <stdlib.h>

#include <hermes/rpc/buffers_list.h>

typedef uint32_t(*hm_ks_db_set_token)(void* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length, const uint8_t* rtoken, const size_t rtoken_length);
typedef uint32_t(*hm_ks_db_get_token)(void* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, uint8_t** wtoken, size_t* wtoken_id_length, uint8_t** owner_id, size_t* owner_id_length);
typedef uint32_t(*hm_ks_db_del_token)(void* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length);
typedef uint32_t(*hm_ks_db_get_indexed_rights)(void* db, const uint8_t* block_id, const size_t block_id_length, const size_t index, uint8_t** user_id, size_t* user_id_length, uint32_t* rights_mask);

typedef struct hm_ks_db_type{
    void* user_data;
    hm_ks_db_set_token set_rtoken;
    hm_ks_db_set_token set_wtoken;
    hm_ks_db_get_token get_rtoken;
    hm_ks_db_get_token get_wtoken;
    hm_ks_db_get_indexed_rights get_indexed_rights;
    hm_ks_db_del_token del_rtoken;
    hm_ks_db_del_token del_wtoken;
}hm_ks_db_t;


#endif //HERMES_KEY_STORE_DB_H
