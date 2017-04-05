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

typedef struct hm_ds_db_type hm_ds_db_t;

uint32_t hm_ds_db_insert_block(hm_ds_db_t* db, const uint8_t* block, const size_t block_length, const uint8_t* mac, const size_t mac_length, uint8_t** id, size_t* id_length);
uint32_t hm_ds_db_read_block(hm_ds_db_t* db, const uint8_t* id, const size_t id_length, uint8_t** block, size_t*  block_length);
uint32_t hm_ds_db_read_block_mac(hm_ds_db_t* db, const uint8_t* id, const size_t id_length, uint8_t** mac, size_t*  mac_length);
uint32_t hm_ds_db_update_block(hm_ds_db_t* db, const uint8_t* id, const size_t id_length, const uint8_t* block, const size_t block_length, const uint8_t* mac, const size_t mac_length);
uint32_t hm_ds_db_delete_block(hm_ds_db_t* db, const uint8_t* id, const size_t id_length);

#endif //HERMES_DATA_STORE_DB_H
