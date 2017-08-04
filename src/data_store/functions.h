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



#ifndef HERMES_DATA_STORE_FUNCTION_H
#define HERMES_DATA_STORE_FUNCTION_H

#include <hermes/rpc/param_pack.h>
#include <hermes/rpc/client.h>
#include <hermes/data_store/db.h>

#include <stdint.h>
#include <stdlib.h>

#define hm_data_store_create_block_NAME "hm_data_store_create_block"

uint32_t hm_data_store_create_block(
        hm_ds_db_t *db,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length,
        const uint8_t *mac, const size_t mac_length,
        uint8_t **id, size_t *id_length);

#define hm_data_store_create_block_with_id_NAME "hm_data_store_create_block_with_id"

uint32_t hm_data_store_create_block_with_id(
        hm_ds_db_t *db,
        const uint8_t *id, const size_t id_length,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length,
        const uint8_t *mac, const size_t mac_length);

#define hm_data_store_read_block_NAME "hm_data_store_read_block"

uint32_t hm_data_store_read_block(
        hm_ds_db_t *db,
        const uint8_t *id, const size_t id_length,
        uint8_t **block, size_t *block_length,
        uint8_t **meta, size_t *meta_length);

#define hm_data_store_update_block_NAME "hm_data_store_update_block"

uint32_t hm_data_store_update_block(
        hm_ds_db_t *db,
        const uint8_t *id, const size_t id_length,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length,
        const uint8_t *mac, const size_t mac_length,
        const uint8_t *old_mac, const size_t old_mac_length);

#define hm_data_store_delete_block_NAME "hm_data_store_delete_block"

uint32_t hm_data_store_delete_block(
        hm_ds_db_t *db,
        const uint8_t *id, const size_t id_length,
        const uint8_t *old_mac, const size_t old_mac_length);

//proxies
uint32_t hm_data_store_create_block_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length,
        const uint8_t *mac, const size_t mac_length,
        uint8_t **id, size_t *id_length);

uint32_t hm_data_store_create_block_with_id_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *id, const size_t id_length,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length,
        const uint8_t *mac, const size_t mac_length);

uint32_t hm_data_store_read_block_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *id, const size_t id_length,
        uint8_t **block, size_t *block_length,
        uint8_t **meta, size_t *meta_length);

uint32_t hm_data_store_update_block_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *id, const size_t id_length,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length,
        const uint8_t *mac, const size_t mac_length,
        const uint8_t *old_mac, const size_t old_mac_length);

uint32_t hm_data_store_delete_block_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *id, const size_t id_length,
        const uint8_t *old_mac, const size_t old_mac_length);

//stubs
uint32_t hm_data_store_create_block_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data);

uint32_t hm_data_store_create_block_with_id_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data);

uint32_t hm_data_store_read_block_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data);

uint32_t hm_data_store_update_block_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data);

uint32_t hm_data_store_delete_block_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data);

#endif //HERMES_DATA_STORE_FUNCTION_H
