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


#ifndef HERMES_KEY_STORE_FUNCTION_H
#define HERMES_KEY_STORE_FUNCTION_H

#include <hermes/rpc/param_pack.h>
#include <hermes/rpc/client.h>
#include <hermes/key_store/db.h>

#include <stdint.h>
#include <stdlib.h>

#define hm_key_store_set_rtoken_NAME "hm_key_store_set_rtoken"
uint32_t hm_key_store_set_rtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length, const uint8_t* rtoken, const size_t rtoken_length);

#define hm_key_store_set_wtoken_NAME "hm_key_store_set_wtoken"
uint32_t hm_key_store_set_wtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length, const uint8_t* wtoken, const size_t wtoken_length);

#define hm_key_store_get_rtoken_NAME "hm_key_store_get_rtoken"
uint32_t hm_key_store_get_rtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, uint8_t** rtoken, size_t* rtoken_length, uint8_t** owner_id, size_t* owner_id_length);

#define hm_key_store_get_wtoken_NAME "hm_key_store_get_wtoken"
uint32_t hm_key_store_get_wtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, uint8_t** wtoken, size_t* wtoken_length, uint8_t** owner_id, size_t* owner_id_length);

#define hm_key_store_del_rtoken_NAME "hm_key_store_del_rtoken"
uint32_t hm_key_store_del_rtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length);

#define hm_key_store_del_wtoken_NAME "hm_key_store_del_wtoken"
uint32_t hm_key_store_del_wtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length);

//proxies
uint32_t hm_key_store_set_rtoken_sync_proxy(hm_rpc_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length, const uint8_t* rtoken, const size_t rtoken_length);
uint32_t hm_key_store_set_wtoken_sync_proxy(hm_rpc_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length, const uint8_t* wtoken, const size_t wtoken_length);
uint32_t hm_key_store_get_rtoken_sync_proxy(hm_rpc_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, uint8_t** rtoken, size_t* rtoken_length, uint8_t** owner_id, size_t* owner_id_length);
uint32_t hm_key_store_get_wtoken_sync_proxy(hm_rpc_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, uint8_t** wtoken, size_t* wtoken_length, uint8_t** owner_id, size_t* owner_id_length);
uint32_t hm_key_store_del_rtoken_sync_proxy(hm_rpc_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length);
uint32_t hm_key_store_del_wtoken_sync_proxy(hm_rpc_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length);

//stubs
uint32_t hm_key_store_set_rtoken_stub(hm_param_pack_t* in, hm_param_pack_t** out, void* user_data);
uint32_t hm_key_store_set_wtoken_stub(hm_param_pack_t* in, hm_param_pack_t** out, void* user_data);
uint32_t hm_key_store_get_rtoken_stub(hm_param_pack_t* in, hm_param_pack_t** out, void* user_data);
uint32_t hm_key_store_get_wtoken_stub(hm_param_pack_t* in, hm_param_pack_t** out, void* user_data);
uint32_t hm_key_store_del_rtoken_stub(hm_param_pack_t* in, hm_param_pack_t** out, void* user_data);
uint32_t hm_key_store_del_wtoken_stub(hm_param_pack_t* in, hm_param_pack_t** out, void* user_data);

#endif //HERMES_KEY_STORE_FUNCTION_H
