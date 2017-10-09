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



#include "functions.h"

#include <hermes/common/errors.h>

// hm_credential_store_check_backend return HM_SUCCESS if backend hasn't NULL on all functions that expected by interface
// otherwise HM_FAIL
uint32_t hm_credential_store_check_backend(hm_cs_db_t *db){
    if(!(db && db->get_pub)){
        return false;
    }
    return true;
}

uint32_t hm_credential_store_get_pub_key_by_id(
        hm_cs_db_t *db, const uint8_t *id, const size_t id_length, uint8_t **key, size_t *key_length) {
    if (!db || !(db->get_pub) || !id || !id_length || !key) {
        return HM_INVALID_PARAMETER;
    }
    return db->get_pub(db->user_data, id, id_length, key, key_length);
}

//proxies
uint32_t hm_credential_store_get_pub_key_by_id_sync_proxy(
        hm_rpc_client_sync_t *client, const uint8_t *id, const size_t id_length, uint8_t **key, size_t *key_length) {
    if (!client || !id || !id_length || !key) {
        return HM_INVALID_PARAMETER;
    }
    hm_param_pack_t *in = HM_PARAM_PACK(HM_PARAM_BUFFER_C(id, id_length));
    if (!in) {
        return HM_FAIL;
    }
    hm_param_pack_t *out = NULL;
    uint32_t status, res;
    if (HM_SUCCESS != (res = hm_rpc_client_sync_call(
            client,
            (const uint8_t *) hm_credential_store_get_pub_key_by_id_NAME,
            sizeof(hm_credential_store_get_pub_key_by_id_NAME),
            in, &status, &(out)))) {
        hm_param_pack_destroy(&in);
        return res;
    }
    hm_param_pack_destroy(&in);
    if (HM_SUCCESS != status) {
        return status;
    }
    if (HM_SUCCESS != (res = HM_PARAM_EXTRACT(out, HM_PARAM_BUFFER(key, key_length)))) {
        hm_param_pack_destroy(&out);
        return res;
    }
    hm_param_pack_destroy(&out);
    return HM_SUCCESS;
}

//stubs
uint32_t hm_credential_store_get_pub_key_by_id_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data) {
    if (!user_data || !in || !out) {
        return HM_INVALID_PARAMETER;
    }
    uint8_t *id = NULL, *key = NULL;
    size_t id_length = 0, key_length = 0;
    uint32_t res;
    if (HM_SUCCESS != (res = HM_PARAM_EXTRACT(in, HM_PARAM_BUFFER(&id, &id_length)))) {
        return res;
    }
    if (HM_SUCCESS !=
        (res = hm_credential_store_get_pub_key_by_id((hm_cs_db_t *) user_data, id, id_length, &key, &key_length))) {
        return res;
    }
    *out = HM_PARAM_PACK(HM_PARAM_BUFFER_C(key, key_length));
    if (!(*out)) {
        return HM_FAIL;
    }
    return HM_SUCCESS;
}

