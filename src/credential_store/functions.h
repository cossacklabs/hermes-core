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



#ifndef HERMES_CREDENTIAL_STORE_FUNCTION_H
#define HERMES_CREDENTIAL_STORE_FUNCTION_H

#include <hermes/rpc/param_pack.h>
#include <hermes/rpc/client.h>
#include <hermes/credential_store/db.h>

#include <stdint.h>
#include <stdlib.h>

uint32_t hm_credential_store_check_backend(hm_cs_db_t *db);

#define hm_credential_store_get_pub_key_by_id_NAME "hm_credential_store_get_pub_key_by_id"

uint32_t hm_credential_store_get_pub_key_by_id(
        hm_cs_db_t *db, const uint8_t *id, const size_t id_length, uint8_t **key, size_t *key_length);

//proxies
uint32_t hm_credential_store_get_pub_key_by_id_sync_proxy(
        hm_rpc_client_sync_t *client, const uint8_t *id, const size_t id_length, uint8_t **key, size_t *key_length);

//stubs
uint32_t hm_credential_store_get_pub_key_by_id_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data);

#endif //HERMES_CREDENTIAL_STORE_FUNCTION_H
