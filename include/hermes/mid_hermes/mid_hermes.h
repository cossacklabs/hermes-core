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



#ifndef MID_HERMES_H
#define MID_HERMES_H

#include <hermes/common/errors.h>
#include <hermes/rpc/transport.h>
#include <hermes/credential_store/db.h>
#include <hermes/mid_hermes_ll/interfaces/key_store.h>
#include <hermes/mid_hermes_ll/interfaces/data_store.h>
#include <hermes/mid_hermes_ll/interfaces/credential_store.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct mid_hermes_type mid_hermes_t;

mid_hermes_t *mid_hermes_create(
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *private_key, const size_t private_key_length,
        hm_rpc_transport_t *key_store_transport,
        hm_rpc_transport_t *data_store_transport,
        hm_rpc_transport_t *credential_store_transport);

mid_hermes_t* mid_hermes_create_with_services(
        const uint8_t *user_id, size_t user_id_length,
        const uint8_t *private_key, size_t private_key_length,
        hermes_key_store_t* key_store,
        hermes_data_store_t *data_store,
        hermes_credential_store_t *credential_store);

hermes_status_t mid_hermes_destroy(mid_hermes_t **mh);

hermes_status_t mid_hermes_create_block(
        mid_hermes_t *mid_hermes,
        uint8_t **id, size_t *id_length,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length);

hermes_status_t mid_hermes_read_block(
        mid_hermes_t *mid_hermes,
        const uint8_t *block_id, const size_t block_id_length,
        uint8_t **block, size_t *block_length,
        uint8_t **meta, size_t *meta_length);

hermes_status_t mid_hermes_update_block(
        mid_hermes_t *mid_hermes,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length);

hermes_status_t mid_hermes_delete_block(
        mid_hermes_t *mid_hermes, const uint8_t *block_id, const size_t block_id_length);

hermes_status_t mid_hermes_rotate_block(
        mid_hermes_t *mid_hermes, const uint8_t *block_id, const size_t block_id_length);

hermes_status_t mid_hermes_grant_read_access(
        mid_hermes_t *mid_hermes,
        const uint8_t *block_id, const size_t bloc_id_length,
        const uint8_t *user_id, const size_t user_id_length);

hermes_status_t mid_hermes_grant_update_access(
        mid_hermes_t *mid_hermes,
        const uint8_t *block_id, const size_t bloc_id_length,
        const uint8_t *user_id, const size_t user_id_length);

hermes_status_t mid_hermes_deny_read_access(
        mid_hermes_t *mid_hermes,
        const uint8_t *block_id, const size_t bloc_id_length,
        const uint8_t *user_id, const size_t user_id_length);

hermes_status_t mid_hermes_deny_update_access(
        mid_hermes_t *mid_hermes,
        const uint8_t *block_id, const size_t bloc_id_length,
        const uint8_t *user_id, const size_t user_id_length);

#endif //MID_HERMES_H
