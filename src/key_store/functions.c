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

#include <string.h>
#include <stdio.h>

// hm_key_store_check_backend return HM_SUCCESS if backend hasn't NULL on all functions that expected by interface
// otherwise HM_FAIL
uint32_t hm_key_store_check_backend(hm_ks_db_t *db){
    if(!(db && db->set_rtoken && db->set_wtoken && db->get_rtoken && db->get_wtoken && db->del_rtoken && db->del_wtoken && db->get_indexed_rights)){
        return false;
    }
    return true;
}

uint32_t hm_key_store_set_rtoken(
        hm_ks_db_t *db,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *owner_id, const size_t owner_id_length,
        const uint8_t *read_token, const size_t read_token_length) {
    if (!db || !(db->user_data) || !(db->get_rtoken) || !(db->set_rtoken) || !block_id || !block_id_length ||
        !user_id || !user_id_length || !owner_id || !owner_id_length || !read_token || !read_token_length) {
        return HM_INVALID_PARAMETER;
    }
    uint32_t res;
    if (user_id_length != owner_id_length || 0 != memcmp(user_id, owner_id, user_id_length)) {
        uint8_t *test_token = NULL, *test_id = NULL;
        size_t test_token_length = 0, test_id_length = 0;
        if (HM_SUCCESS != (res = db->get_rtoken(
                db->user_data, block_id, block_id_length, owner_id, owner_id_length, &test_token, &test_token_length,
                &test_id, &test_id_length))) {
            return HM_FAIL;
        }
        free(test_token);
        free(test_id);
    }
    return db->set_rtoken(
            db->user_data, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length,
            read_token, read_token_length);
}

uint32_t hm_key_store_set_wtoken(
        hm_ks_db_t *db,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *owner_id, const size_t owner_id_length,
        const uint8_t *wtoken, const size_t wtoken_length) {
    if (!db || !(db->user_data) || !(db->get_wtoken) || !(db->set_wtoken) || !block_id || !block_id_length ||
        !user_id || !user_id_length || !owner_id || !owner_id_length || !wtoken || !wtoken_length) {
        return HM_INVALID_PARAMETER;
    }
    uint32_t res;
    if (user_id_length != owner_id_length || 0 != memcmp(user_id, owner_id, user_id_length)) {
        uint8_t *test_token = NULL, *test_id = NULL;
        size_t test_token_length = 0, test_id_length = 0;
        if (HM_SUCCESS != (res = db->get_wtoken(
                db->user_data, block_id, block_id_length, owner_id, owner_id_length, &test_token, &test_token_length,
                &test_id, &test_id_length))) {
            return HM_FAIL;
        }
        free(test_token);
        free(test_id);
    }
    return db->set_wtoken(
            db->user_data, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length, wtoken,
            wtoken_length);
}

uint32_t hm_key_store_get_rtoken(
        hm_ks_db_t *db,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        uint8_t **read_token, size_t *read_token_length,
        uint8_t **owner_id, size_t *owner_id_length) {
    if (!db || !(db->user_data) || !(db->get_rtoken) || !block_id || !block_id_length || !user_id || !user_id_length ||
        !read_token || !owner_id) {
        return HM_INVALID_PARAMETER;
    }
    return db->get_rtoken(
            db->user_data, block_id, block_id_length, user_id, user_id_length, read_token, read_token_length,
            owner_id, owner_id_length);
}

uint32_t hm_key_store_get_wtoken(
        hm_ks_db_t *db,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        uint8_t **write_token, size_t *write_token_length,
        uint8_t **owner_id, size_t *owner_id_length) {
    if (!db || !(db->user_data) || !(db->get_wtoken) || !block_id || !block_id_length || !user_id || !user_id_length ||
        !write_token || !owner_id) {
        return HM_INVALID_PARAMETER;
    }
    return db->get_wtoken(
            db->user_data, block_id, block_id_length, user_id, user_id_length, write_token, write_token_length,
            owner_id, owner_id_length);
}

uint32_t hm_key_store_del_rtoken(
        hm_ks_db_t *db,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *owner_id, const size_t owner_id_length) {
    if (!db || !(db->user_data) || !(db->get_rtoken) || (!(db->del_rtoken)) || !block_id || !block_id_length ||
        !user_id || !user_id_length || !owner_id || !owner_id_length) {
        return HM_INVALID_PARAMETER;
    }
    uint8_t *test_token = NULL, *test_id = NULL;
    size_t test_token_length = 0, test_id_length = 0;
    if (HM_SUCCESS != db->get_wtoken(
            db->user_data, block_id, block_id_length, owner_id, owner_id_length, &test_token, &test_token_length,
            &test_id, &test_id_length)) {
        return HM_FAIL;
    }
    return db->del_rtoken(db->user_data, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length);
}

uint32_t hm_key_store_del_wtoken(
        hm_ks_db_t *db,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *owner_id, const size_t owner_id_length) {
    if (!db || !(db->user_data) || !(db->get_wtoken) || !(db->del_wtoken) || !block_id || !block_id_length ||
        !user_id || !user_id_length || !owner_id || !owner_id_length) {
        return HM_INVALID_PARAMETER;
    }
    uint8_t *test_token = NULL, *test_id = NULL;
    size_t test_token_length = 0, test_id_length = 0;
    if (HM_SUCCESS != db->get_wtoken(
            db->user_data, block_id, block_id_length, owner_id, owner_id_length, &test_token, &test_token_length,
            &test_id, &test_id_length)) {
        return HM_FAIL;
    }
    return db->del_wtoken(db->user_data, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length);
}

uint32_t hm_key_store_get_indexed_rights(
        hm_ks_db_t *db,
        const uint8_t *block_id, const size_t block_id_length,
        const size_t index,
        uint8_t **user_id, size_t *user_id_length,
        uint32_t *rights_mask) {
    if (!db || !(db->user_data) || !(db->get_wtoken) || !(db->del_wtoken) || !block_id || !block_id_length ||
        !user_id || !user_id_length) {
        return HM_INVALID_PARAMETER;
    }
    return db->get_indexed_rights(db->user_data, block_id, block_id_length, index, user_id, user_id_length,
                                  rights_mask);
}

//proxies

uint32_t hm_key_store_set_rtoken_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *owner_id, const size_t owner_id_length,
        const uint8_t *read_token, const size_t read_token_length) {
    if (!client || !block_id || !block_id_length || !user_id || !user_id_length || !owner_id || !owner_id_length ||
        !read_token || !read_token_length) {
        return HM_INVALID_PARAMETER;
    }
    hm_param_pack_t *in = HM_PARAM_PACK(
            HM_PARAM_BUFFER_C(block_id, block_id_length), HM_PARAM_BUFFER_C(user_id, user_id_length),
            HM_PARAM_BUFFER_C(owner_id, owner_id_length), HM_PARAM_BUFFER_C(read_token, read_token_length));
    if (!in) {
        return HM_FAIL;
    }
    uint32_t status, res;
    if (HM_SUCCESS != (res = hm_rpc_client_sync_call(
            client, (const uint8_t *) hm_key_store_set_rtoken_NAME, sizeof(hm_key_store_set_rtoken_NAME), in, &status,
            NULL))) {
        hm_param_pack_destroy(&in);
        return res;
    }
    hm_param_pack_destroy(&in);
    return status;
}

uint32_t hm_key_store_set_wtoken_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *owner_id, const size_t owner_id_length,
        const uint8_t *write_token, const size_t write_token_length) {
    if (!client || !block_id || !block_id_length || !user_id || !user_id_length || !owner_id || !owner_id_length ||
        !write_token || !write_token_length) {
        return HM_INVALID_PARAMETER;
    }
    hm_param_pack_t *in = HM_PARAM_PACK(
            HM_PARAM_BUFFER_C(block_id, block_id_length), HM_PARAM_BUFFER_C(user_id, user_id_length),
            HM_PARAM_BUFFER_C(owner_id, owner_id_length), HM_PARAM_BUFFER_C(write_token, write_token_length));
    if (!in) {
        return HM_FAIL;
    }
    uint32_t status, res;
    if (HM_SUCCESS != (res = hm_rpc_client_sync_call(
            client, (const uint8_t *) hm_key_store_set_wtoken_NAME, sizeof(hm_key_store_set_wtoken_NAME), in, &status,
            NULL))) {
        hm_param_pack_destroy(&in);
        return res;
    }
    hm_param_pack_destroy(&in);
    return status;
}

uint32_t hm_key_store_get_rtoken_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        uint8_t **read_token, size_t *read_token_length,
        uint8_t **owner_id, size_t *owner_id_length) {
    if (!client || !block_id || !block_id_length || !user_id || !user_id_length) {
        return HM_INVALID_PARAMETER;
    }
    hm_param_pack_t *in = HM_PARAM_PACK(
            HM_PARAM_BUFFER_C(block_id, block_id_length), HM_PARAM_BUFFER_C(user_id, user_id_length));
    if (!in) {
        return HM_FAIL;
    }
    hm_param_pack_t *out = NULL;
    uint32_t status, res;
    if (HM_SUCCESS != (res = hm_rpc_client_sync_call
            (client, (const uint8_t *) hm_key_store_get_rtoken_NAME, sizeof(hm_key_store_get_rtoken_NAME), in, &status,
             &(out)))) {
        hm_param_pack_destroy(&in);
        return res;
    }
    hm_param_pack_destroy(&in);
    if (HM_SUCCESS != status) {
        return status;
    }
    if (HM_SUCCESS != (res = HM_PARAM_EXTRACT(
            out, HM_PARAM_BUFFER(read_token, read_token_length), HM_PARAM_BUFFER(owner_id, owner_id_length)))) {
        hm_param_pack_destroy(&out);
        return res;
    }
    hm_param_pack_destroy(&out);
    return HM_SUCCESS;
}

uint32_t hm_key_store_get_wtoken_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        uint8_t **write_token, size_t *write_token_length,
        uint8_t **owner_id, size_t *owner_id_length) {
    if (!client || !block_id || !block_id_length || !user_id || !user_id_length) {
        return HM_INVALID_PARAMETER;
    }
    hm_param_pack_t *in = HM_PARAM_PACK(
            HM_PARAM_BUFFER_C(block_id, block_id_length), HM_PARAM_BUFFER_C(user_id, user_id_length));
    if (!in) {
        return HM_FAIL;
    }
    hm_param_pack_t *out = NULL;
    uint32_t status, res;
    if (HM_SUCCESS != (res = hm_rpc_client_sync_call(
            client, (const uint8_t *) hm_key_store_get_wtoken_NAME, sizeof(hm_key_store_get_wtoken_NAME), in, &status,
            &(out)))) {
        hm_param_pack_destroy(&in);
        return res;
    }
    hm_param_pack_destroy(&in);
    if (HM_SUCCESS != status) {
        return status;
    }
    if (HM_SUCCESS != (res = HM_PARAM_EXTRACT(
            out, HM_PARAM_BUFFER(write_token, write_token_length), HM_PARAM_BUFFER(owner_id, owner_id_length)))) {
        hm_param_pack_destroy(&out);
        return res;
    }
    hm_param_pack_destroy(&out);
    return HM_SUCCESS;
}

uint32_t hm_key_store_del_rtoken_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *owner_id, const size_t owner_id_length) {
    if (!client || !block_id || !block_id_length || !user_id || !user_id_length || !owner_id || !owner_id_length) {
        return HM_INVALID_PARAMETER;
    }
    hm_param_pack_t *in = HM_PARAM_PACK(
            HM_PARAM_BUFFER_C(block_id, block_id_length), HM_PARAM_BUFFER_C(user_id, user_id_length),
            HM_PARAM_BUFFER_C(owner_id, owner_id_length));
    if (!in) {
        return HM_FAIL;
    }
    uint32_t status, res;
    if (HM_SUCCESS != (res = hm_rpc_client_sync_call(
            client, (const uint8_t *) hm_key_store_del_rtoken_NAME, sizeof(hm_key_store_del_rtoken_NAME), in, &status,
            NULL))) {
        hm_param_pack_destroy(&in);
        return res;
    }
    hm_param_pack_destroy(&in);
    return status;
}

uint32_t hm_key_store_del_wtoken_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *owner_id, const size_t owner_id_length) {
    if (!client || !block_id || !block_id_length || !user_id || !user_id_length || !owner_id || !owner_id_length) {
        return HM_INVALID_PARAMETER;
    }
    hm_param_pack_t *in = HM_PARAM_PACK(
            HM_PARAM_BUFFER_C(block_id, block_id_length), HM_PARAM_BUFFER_C(user_id, user_id_length),
            HM_PARAM_BUFFER_C(owner_id, owner_id_length));
    if (!in) {
        return HM_FAIL;
    }
    uint32_t status, res;
    if (HM_SUCCESS != (res = hm_rpc_client_sync_call(
            client, (const uint8_t *) hm_key_store_del_wtoken_NAME, sizeof(hm_key_store_del_wtoken_NAME), in, &status,
            NULL))) {
        hm_param_pack_destroy(&in);
        return res;
    }
    hm_param_pack_destroy(&in);
    return status;
}

uint32_t hm_key_store_get_indexed_rights_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *block_id, const size_t block_id_length,
        const size_t index, uint8_t **user_id,
        size_t *user_id_length, uint32_t *rights_mask) {
    if (!client || !block_id || !block_id_length || !user_id || !user_id_length || !rights_mask) {
        return HM_INVALID_PARAMETER;
    }
    hm_param_pack_t *in = HM_PARAM_PACK(HM_PARAM_BUFFER_C(block_id, block_id_length), HM_PARAM_INT32(index));
    if (!in) {
        return HM_FAIL;
    }
    uint32_t status, res;
    hm_param_pack_t *out = NULL;
    if (HM_SUCCESS != (res = hm_rpc_client_sync_call(
            client, (const uint8_t *) hm_key_store_get_indexed_rights_NAME,
            sizeof(hm_key_store_get_indexed_rights_NAME),
            in, &status, &(out)))) {
        hm_param_pack_destroy(&in);
        return res;
    }
    hm_param_pack_destroy(&in);
    if (HM_SUCCESS != status) {
        return status;
    }
    if (HM_SUCCESS !=
        (res = HM_PARAM_EXTRACT(out, HM_PARAM_BUFFER(user_id, user_id_length), HM_PARAM_INT32(rights_mask)))) {
        hm_param_pack_destroy(&out);
        return res;
    }
    hm_param_pack_destroy(&out);
    return HM_SUCCESS;
}

//stubs
uint32_t hm_key_store_set_rtoken_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data) {
    if (!user_data || !in || !out) {
        return HM_INVALID_PARAMETER;
    }
    uint8_t *block_id = NULL, *user_id = NULL, *owner_id = NULL, *read_token = NULL;
    size_t block_id_length = 0, user_id_length = 0, owner_id_length = 0, read_token_length = 0;
    uint32_t res;
    if (HM_SUCCESS != (res = HM_PARAM_EXTRACT(
            in, HM_PARAM_BUFFER(&block_id, &block_id_length), HM_PARAM_BUFFER(&user_id, &user_id_length),
            HM_PARAM_BUFFER(&owner_id, &owner_id_length), HM_PARAM_BUFFER(&read_token, &read_token_length)))) {
        return res;
    }
    return hm_key_store_set_rtoken(
            (hm_ks_db_t *) user_data, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length,
            read_token, read_token_length);
}

uint32_t hm_key_store_set_wtoken_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data) {
    if (!user_data || !in || !out) {
        return HM_INVALID_PARAMETER;
    }
    uint8_t *block_id = NULL, *user_id = NULL, *owner_id = NULL, *write_token = NULL;
    size_t block_id_length = 0, user_id_length = 0, owner_id_length = 0, write_token_length = 0;
    uint32_t res;
    if (HM_SUCCESS != (res = HM_PARAM_EXTRACT(
            in, HM_PARAM_BUFFER(&block_id, &block_id_length), HM_PARAM_BUFFER(&user_id, &user_id_length),
            HM_PARAM_BUFFER(&owner_id, &owner_id_length), HM_PARAM_BUFFER(&write_token, &write_token_length)))) {
        return res;
    }
    return hm_key_store_set_wtoken(
            (hm_ks_db_t *) user_data, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length,
            write_token, write_token_length);
}

uint32_t hm_key_store_get_rtoken_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data) {
    if (!user_data || !in || !out) {
        return HM_INVALID_PARAMETER;
    }
    uint8_t *block_id = NULL, *user_id = NULL, *owner_id = NULL, *read_token = NULL;
    size_t block_id_length = 0, user_id_length = 0, owner_id_length = 0, read_token_length = 0;
    uint32_t res;
    if (HM_SUCCESS != (res = HM_PARAM_EXTRACT(
            in, HM_PARAM_BUFFER(&block_id, &block_id_length), HM_PARAM_BUFFER(&user_id, &user_id_length)))) {
        return res;
    }
    if (HM_SUCCESS != (res = hm_key_store_get_rtoken(
            (hm_ks_db_t *) user_data, block_id, block_id_length, user_id, user_id_length, &read_token,
            &read_token_length,
            &owner_id, &owner_id_length))) {
        return res;
    }
    *out = HM_PARAM_PACK(HM_PARAM_BUFFER(read_token, read_token_length), HM_PARAM_BUFFER(owner_id, owner_id_length));
    if (!(*out)) {
        return HM_FAIL;
    }
    return HM_SUCCESS;
}

uint32_t hm_key_store_get_wtoken_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data) {
    if (!user_data || !in || !out) {
        return HM_INVALID_PARAMETER;
    }
    uint8_t *block_id = NULL, *user_id = NULL, *owner_id = NULL, *write_token = NULL;
    size_t block_id_length = 0, user_id_length = 0, owner_id_length = 0, write_token_length = 0;
    uint32_t res;
    if (HM_SUCCESS != (res = HM_PARAM_EXTRACT(
            in, HM_PARAM_BUFFER(&block_id, &block_id_length), HM_PARAM_BUFFER(&user_id, &user_id_length)))) {
        return res;
    }
    if (HM_SUCCESS != (res = hm_key_store_get_wtoken(
            (hm_ks_db_t *) user_data, block_id, block_id_length, user_id, user_id_length, &write_token,
            &write_token_length,
            &owner_id, &owner_id_length))) {
        return res;
    }
    *out = HM_PARAM_PACK(HM_PARAM_BUFFER(write_token, write_token_length), HM_PARAM_BUFFER(owner_id, owner_id_length));
    if (!(*out)) {
        return HM_FAIL;
    }
    return HM_SUCCESS;
}

uint32_t hm_key_store_del_rtoken_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data) {
    if (!user_data || !in || !out) {
        return HM_INVALID_PARAMETER;
    }
    uint8_t *block_id = NULL, *user_id = NULL, *owner_id = NULL;
    size_t block_id_length = 0, user_id_length = 0, owner_id_length = 0;
    uint32_t res;
    if (HM_SUCCESS != (res = HM_PARAM_EXTRACT(
            in, HM_PARAM_BUFFER(&block_id, &block_id_length), HM_PARAM_BUFFER(&user_id, &user_id_length),
            HM_PARAM_BUFFER(&owner_id, &owner_id_length)))) {
        return res;
    }
    return hm_key_store_del_rtoken((hm_ks_db_t *) user_data, block_id, block_id_length, user_id, user_id_length,
                                   owner_id, owner_id_length);
}

uint32_t hm_key_store_del_wtoken_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data) {
    if (!user_data || !in || !out) {
        return HM_INVALID_PARAMETER;
    }
    uint8_t *block_id = NULL, *user_id = NULL, *owner_id = NULL;
    size_t block_id_length = 0, user_id_length = 0, owner_id_length = 0;
    uint32_t res;
    if (HM_SUCCESS != (res = HM_PARAM_EXTRACT(
            in, HM_PARAM_BUFFER(&block_id, &block_id_length), HM_PARAM_BUFFER(&user_id, &user_id_length),
            HM_PARAM_BUFFER(&owner_id, &owner_id_length)))) {
        return res;
    }
    return hm_key_store_del_wtoken(
            (hm_ks_db_t *) user_data, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length);
}

uint32_t hm_key_store_get_indexed_rights_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data) {
    if (!user_data || !in || !out) {
        return HM_INVALID_PARAMETER;
    }
    uint8_t *block_id = NULL, *user_id = NULL;
    size_t block_id_length = 0, user_id_length = 0;
    uint32_t res, rights_mask;
    int32_t index;
    if (HM_SUCCESS !=
        (res = HM_PARAM_EXTRACT(in, HM_PARAM_BUFFER(&block_id, &block_id_length), HM_PARAM_INT32(&index)))) {
        return res;
    }
    if (HM_SUCCESS != (res = hm_key_store_get_indexed_rights(
            (hm_ks_db_t *) user_data, block_id, block_id_length, index, &user_id, &user_id_length, &rights_mask))) {
        return res;
    }
    *out = HM_PARAM_PACK(HM_PARAM_BUFFER(user_id, user_id_length), HM_PARAM_INT32(rights_mask));
    if (!(*out)) {
        return HM_FAIL;
    }
    return HM_SUCCESS;
}
