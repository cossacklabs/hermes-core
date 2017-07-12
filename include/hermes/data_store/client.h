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


#ifndef HERMES_DATA_STORE_CLIENT_H
#define HERMES_DATA_STORE_CLIENT_H

#include <hermes/rpc/transport.h>

#include <stdint.h>
#include <stdlib.h>

typedef struct hm_data_store_client_sync_type hm_data_store_client_sync_t;

hm_data_store_client_sync_t *hm_data_store_client_sync_create(hm_rpc_transport_t *transport);

uint32_t hm_data_store_client_sync_destroy(hm_data_store_client_sync_t **client);

uint32_t hm_data_store_client_sync_call_create_block(
        hm_data_store_client_sync_t *client,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length,
        const uint8_t *mac, const size_t mac_length,
        uint8_t **id, size_t *id_length);

uint32_t hm_data_store_client_sync_call_create_block_with_id(
        hm_data_store_client_sync_t *client,
        const uint8_t *id, const size_t id_length,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length,
        const uint8_t *mac, const size_t mac_length);

uint32_t hm_data_store_client_sync_call_read_block(
        hm_data_store_client_sync_t *client,
        const uint8_t *id, const size_t id_length,
        uint8_t **block, size_t *block_length,
        uint8_t **meta, size_t *meta_length);

uint32_t hm_data_store_client_sync_call_update_block(
        hm_data_store_client_sync_t *client,
        const uint8_t *id, const size_t id_length,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length,
        const uint8_t *mac, const size_t mac_length,
        const uint8_t *old_mac, const size_t old_mac_length);

uint32_t hm_data_store_client_sync_call_delete_block(
        hm_data_store_client_sync_t *client,
        const uint8_t *id, const size_t id_length,
        const uint8_t *old_mac, const size_t old_mac_length);

#endif //HERMES_DATA_STORE_CLIENT_H
