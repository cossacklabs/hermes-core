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

typedef struct hm_ks_db_type hm_ks_db_t;

uint32_t hm_ks_db_set_rtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length, const uint8_t* rtoken, const size_t rtoken_length);
uint32_t hm_ks_db_set_wtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length, const uint8_t* wtoken, const size_t wtoken_length);
uint32_t hm_ks_db_get_rtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, uint8_t** rtoken, size_t* rtoken_id_length, uint8_t** owner_id, size_t* owner_id_length);
uint32_t hm_ks_db_get_wtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, uint8_t** wtoken, size_t* wtoken_id_length, uint8_t** owner_id, size_t* owner_id_length);
uint32_t hm_ks_db_del_rtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length);
uint32_t hm_ks_db_del_wtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length);


#endif //HERMES_KEY_STORE_DB_H
