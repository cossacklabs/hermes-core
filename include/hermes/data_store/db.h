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


#ifndef HERMES_DATA_STORE_DB_H
#define HERMES_DATA_STORE_DB_H


#include <stdint.h>
#include <stdlib.h>

typedef uint32_t(*hm_ds_db_insert_block)(
        void* db, const uint8_t* block, const size_t block_length,
        const uint8_t* meta, const size_t meta_length,
        const uint8_t* mac, const size_t mac_length,
        uint8_t** id, size_t* id_length);
typedef uint32_t(*hm_ds_db_insert_block_with_id)(
        void* db, const uint8_t* id, const size_t id_length,
        const uint8_t* block, const size_t block_length,
        const uint8_t* meta, const size_t meta_length,
        const uint8_t* mac, const size_t mac_length);
typedef uint32_t(*hm_ds_db_get_block)(
        void* db, const uint8_t* id, const size_t id_length,
        uint8_t** block, size_t*  block_length,
        uint8_t** meta, size_t*  meta_length);
typedef uint32_t(*hm_ds_db_update_block)(
        void* db, const uint8_t* id, const size_t id_length,
        const uint8_t* block, const size_t block_length,
        const uint8_t* meta, const size_t meta_length,
        const uint8_t* mac, const size_t mac_length,
        const uint8_t* old_mac, const size_t old_mac_length);
typedef uint32_t(*hm_ds_db_rem_block)(
        void* db, const uint8_t* id, const size_t id_length,
        const uint8_t* old_mac, const size_t old_mac_length);

typedef struct hm_ds_db_type{
    void* user_data;
    hm_ds_db_insert_block insert_block;
    hm_ds_db_insert_block_with_id insert_block_with_id;
    hm_ds_db_get_block get_block;
    hm_ds_db_update_block update_block;
    hm_ds_db_rem_block rem_block;
}hm_ds_db_t;
#endif //HERMES_DATA_STORE_DB_H
