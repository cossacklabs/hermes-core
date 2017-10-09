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

// hm_data_store_check_backend return HM_SUCCESS if backend hasn't NULL on all functions that expected by interface
// otherwise HM_FAIL
uint32_t hm_data_store_check_backend(hm_ds_db_t *db){
    if(!(db && db->insert_block && db->insert_block_with_id && db->get_block && db->update_block && db->rem_block)){
        return false;
    }
    return true;
}

uint32_t hm_data_store_create_block(
        hm_ds_db_t *db,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length,
        const uint8_t *mac, const size_t mac_length,
        uint8_t **id, size_t *id_length) {
    if (!db || !(db->user_data) || !(db->insert_block) || !block || !block_length || !meta || !meta_length || !mac ||
        !mac_length || !id) {
        return HM_INVALID_PARAMETER;
    }
    return db->insert_block(db->user_data, block, block_length, meta, meta_length, mac, mac_length, id, id_length);
}

uint32_t hm_data_store_create_block_with_id(
        hm_ds_db_t *db,
        const uint8_t *id, const size_t id_length,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length,
        const uint8_t *mac, const size_t mac_length) {
    if (!db || !(db->user_data) || !(db->insert_block_with_id) || !block || !block_length || !meta || !meta_length ||
        !mac || !mac_length || !id || !id_length) {
        return HM_INVALID_PARAMETER;
    }
    return db->insert_block_with_id(
            db->user_data, id, id_length, block, block_length, meta, meta_length, mac, mac_length);
}

uint32_t hm_data_store_read_block(
        hm_ds_db_t *db,
        const uint8_t *id, const size_t id_length,
        uint8_t **block, size_t *block_length,
        uint8_t **meta, size_t *meta_length) {
    if (!db || !(db->user_data) || !(db->get_block) || !id || !id_length || !block || !meta) {
        return HM_INVALID_PARAMETER;
    }
    return db->get_block(db->user_data, id, id_length, block, block_length, meta, meta_length);
}

uint32_t hm_data_store_update_block(
        hm_ds_db_t *db,
        const uint8_t *id, const size_t id_length,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length,
        const uint8_t *mac, const size_t mac_length,
        const uint8_t *old_mac, const size_t old_mac_length) {
    if (!db || !(db->user_data) || !(db->update_block) || !id || !id_length || !block || !block_length || !meta ||
        !meta_length || !mac || !mac_length || !old_mac || !old_mac_length) {
        return HM_INVALID_PARAMETER;
    }
    return db->update_block(
            db->user_data, id, id_length, block, block_length, meta, meta_length, mac, mac_length,
            old_mac, old_mac_length);
}

uint32_t hm_data_store_delete_block(
        hm_ds_db_t *db,
        const uint8_t *id, const size_t id_length,
        const uint8_t *old_mac, const size_t old_mac_length) {
    if (!db || !(db->user_data) || !(db->rem_block) || !id || !id_length || !old_mac || !old_mac_length) {
        return HM_INVALID_PARAMETER;
    }
    return db->rem_block(db->user_data, id, id_length, old_mac, old_mac_length);
}


//proxies
uint32_t hm_data_store_create_block_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length,
        const uint8_t *mac, const size_t mac_length,
        uint8_t **id, size_t *id_length) {
    if (!client || !block || !block_length || !meta || !meta_length || !mac || !mac_length || !id) {
        return HM_INVALID_PARAMETER;
    }
    hm_param_pack_t *in = HM_PARAM_PACK(
            HM_PARAM_BUFFER_C(block, block_length), HM_PARAM_BUFFER_C(meta, meta_length),
            HM_PARAM_BUFFER_C(mac, mac_length));
    if (!in) {
        return HM_FAIL;
    }
    hm_param_pack_t *out = NULL;
    uint32_t status, res;
    if (HM_SUCCESS != (res = hm_rpc_client_sync_call(
            client, (const uint8_t *) hm_data_store_create_block_NAME, sizeof(hm_data_store_create_block_NAME),
            in, &status, &(out)))) {
        hm_param_pack_destroy(&in);
        return res;
    }
    hm_param_pack_destroy(&in);
    if (HM_SUCCESS != status) {
        return status;
    }
    if (HM_SUCCESS != (res = HM_PARAM_EXTRACT(out, HM_PARAM_BUFFER(id, id_length)))) {
        hm_param_pack_destroy(&out);
        return res;
    }
    hm_param_pack_destroy(&out);
    return HM_SUCCESS;
}

uint32_t hm_data_store_create_block_with_id_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *id, const size_t id_length,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length,
        const uint8_t *mac, const size_t mac_length) {
    if (!client || !block || !block_length || !meta || !meta_length || !mac || !mac_length || !id || !id_length) {
        return HM_INVALID_PARAMETER;
    }
    hm_param_pack_t *in = HM_PARAM_PACK(
            HM_PARAM_BUFFER_C(id, id_length), HM_PARAM_BUFFER_C(block, block_length),
            HM_PARAM_BUFFER_C(meta, meta_length), HM_PARAM_BUFFER_C(mac, mac_length));
    if (!in) {
        return HM_FAIL;
    }
    uint32_t status, res;
    if (HM_SUCCESS != (res = hm_rpc_client_sync_call(
            client,
            (const uint8_t *) hm_data_store_create_block_with_id_NAME, sizeof(hm_data_store_create_block_with_id_NAME),
            in, &status, NULL))) {
        hm_param_pack_destroy(&in);
        return res;
    }
    hm_param_pack_destroy(&in);
    if (HM_SUCCESS != status) {
        return status;
    }
    return HM_SUCCESS;
}


uint32_t hm_data_store_read_block_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *id, const size_t id_length,
        uint8_t **block, size_t *block_length,
        uint8_t **meta, size_t *meta_length) {
    if (!client || !id || !id_length || !block || !meta) {
        return HM_INVALID_PARAMETER;
    }
    hm_param_pack_t *in = HM_PARAM_PACK(HM_PARAM_BUFFER_C(id, id_length));
    if (!in) {
        return HM_FAIL;
    }
    hm_param_pack_t *out = NULL;
    uint32_t status, res;
    if (HM_SUCCESS != (res = hm_rpc_client_sync_call(
            client, (const uint8_t *) hm_data_store_read_block_NAME, sizeof(hm_data_store_read_block_NAME), in, &status,
            &(out)))) {
        hm_param_pack_destroy(&in);
        return res;
    }
    hm_param_pack_destroy(&in);
    if (HM_SUCCESS != status) {
        return status;
    }
    if (HM_SUCCESS !=
        (res = HM_PARAM_EXTRACT(out, HM_PARAM_BUFFER(block, block_length), HM_PARAM_BUFFER(meta, meta_length)))) {
        hm_param_pack_destroy(&out);
        return res;
    }
    hm_param_pack_destroy(&out);
    return HM_SUCCESS;
}

uint32_t hm_data_store_update_block_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *id, const size_t id_length,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length,
        const uint8_t *mac, const size_t mac_length,
        const uint8_t *old_mac, const size_t old_mac_length) {
    if (!client || !id || !id_length || !block || !block_length || !meta || !meta_length || !mac || !mac_length ||
        !old_mac || !old_mac_length) {
        return HM_INVALID_PARAMETER;
    }
    hm_param_pack_t *in = HM_PARAM_PACK(
            HM_PARAM_BUFFER_C(id, id_length), HM_PARAM_BUFFER_C(block, block_length),
            HM_PARAM_BUFFER_C(meta, meta_length), HM_PARAM_BUFFER_C(mac, mac_length),
            HM_PARAM_BUFFER_C(old_mac, old_mac_length));
    if (!in) {
        return HM_FAIL;
    }
    uint32_t status, res;
    if (HM_SUCCESS != (res = hm_rpc_client_sync_call(
            client, (const uint8_t *) hm_data_store_update_block_NAME, sizeof(hm_data_store_update_block_NAME),
            in, &status, NULL))) {
        hm_param_pack_destroy(&in);
        return res;
    }
    hm_param_pack_destroy(&in);
    if (HM_SUCCESS != status) {
        return status;
    }
    return HM_SUCCESS;
}

uint32_t hm_data_store_delete_block_sync_proxy(
        hm_rpc_client_sync_t *client,
        const uint8_t *id, const size_t id_length,
        const uint8_t *old_mac, const size_t old_mac_length) {
    if (!client || !id || !id_length || !old_mac || !old_mac_length) {
        return HM_INVALID_PARAMETER;
    }
    hm_param_pack_t *in = HM_PARAM_PACK(HM_PARAM_BUFFER_C(id, id_length), HM_PARAM_BUFFER_C(old_mac, old_mac_length));
    if (!in) {
        return HM_FAIL;
    }
    uint32_t status, res;
    if (HM_SUCCESS != (res = hm_rpc_client_sync_call(
            client, (const uint8_t *) hm_data_store_delete_block_NAME, sizeof(hm_data_store_delete_block_NAME),
            in, &status, NULL))) {
        hm_param_pack_destroy(&in);
        return res;
    }
    hm_param_pack_destroy(&in);
    if (HM_SUCCESS != status) {
        return status;
    }
    return HM_SUCCESS;
}

//stubs
uint32_t hm_data_store_create_block_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data) {
    if (!user_data || !in || !out) {
        return HM_INVALID_PARAMETER;
    }
    uint8_t *id = NULL, *block = NULL, *meta = NULL, *mac = NULL;
    size_t id_length = 0, block_length = 0, meta_length = 0, mac_length = 0;
    uint32_t res;
    if (HM_SUCCESS != (res = HM_PARAM_EXTRACT(
            in, HM_PARAM_BUFFER(&block, &block_length), HM_PARAM_BUFFER(&meta, &meta_length),
            HM_PARAM_BUFFER(&mac, &mac_length)))) {
        return res;
    }
    if (HM_SUCCESS != (res = hm_data_store_create_block(
            (hm_ds_db_t *) user_data, block, block_length, meta, meta_length, mac, mac_length, &id, &id_length))) {
        return res;
    }
    *out = HM_PARAM_PACK(HM_PARAM_BUFFER(id, id_length));
    if (!(*out)) {
        return HM_FAIL;
    }
    return HM_SUCCESS;
}

uint32_t hm_data_store_create_block_with_id_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data) {
    if (!user_data || !in || !out) {
        return HM_INVALID_PARAMETER;
    }
    uint8_t *id = NULL, *block = NULL, *meta = NULL, *mac = NULL;
    size_t id_length = 0, block_length = 0, meta_length = 0, mac_length = 0;
    uint32_t res;
    if (HM_SUCCESS != (res = HM_PARAM_EXTRACT(
            in, HM_PARAM_BUFFER(&id, &id_length), HM_PARAM_BUFFER(&block, &block_length),
            HM_PARAM_BUFFER(&meta, &meta_length), HM_PARAM_BUFFER(&mac, &mac_length)))) {
        return res;
    }
    return hm_data_store_create_block_with_id(
            (hm_ds_db_t *) user_data, id, id_length, block, block_length, meta, meta_length, mac, mac_length);
}

uint32_t hm_data_store_read_block_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data) {
    if (!user_data || !in || !out) {
        return HM_INVALID_PARAMETER;
    }
    uint8_t *id = NULL, *block = NULL, *meta = NULL;
    size_t id_length = 0, block_length = 0, meta_length = 0;
    uint32_t res;
    if (HM_SUCCESS != (res = HM_PARAM_EXTRACT(in, HM_PARAM_BUFFER(&id, &id_length)))) {
        return res;
    }
    if (HM_SUCCESS != (res = hm_data_store_read_block(
            (hm_ds_db_t *) user_data, id, id_length, &block, &block_length, &meta, &meta_length))) {
        return res;
    }
    *out = HM_PARAM_PACK(HM_PARAM_BUFFER(block, block_length), HM_PARAM_BUFFER(meta, meta_length));
    if (!(*out)) {
        return HM_FAIL;
    }
    return HM_SUCCESS;
}

uint32_t hm_data_store_update_block_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data) {
    if (!user_data || !in || !out) {
        return HM_INVALID_PARAMETER;
    }
    uint8_t *id = NULL, *block = NULL, *meta = NULL, *mac = NULL, *old_mac = NULL;
    size_t id_length = 0, block_length = 0, meta_length = 0, mac_length = 0, old_mac_length = 0;
    uint32_t res;
    if (HM_SUCCESS != (res = HM_PARAM_EXTRACT(
            in, HM_PARAM_BUFFER(&id, &id_length), HM_PARAM_BUFFER(&block, &block_length),
            HM_PARAM_BUFFER(&meta, &meta_length), HM_PARAM_BUFFER(&mac, &mac_length),
            HM_PARAM_BUFFER(&old_mac, &old_mac_length)))) {
        return res;
    }
    return hm_data_store_update_block(
            (hm_ds_db_t *) user_data, id, id_length, block, block_length, meta, meta_length, mac, mac_length,
            old_mac, old_mac_length);
}

uint32_t hm_data_store_delete_block_stub(hm_param_pack_t *in, hm_param_pack_t **out, void *user_data) {
    if (!user_data || !in || !out) {
        return HM_INVALID_PARAMETER;
    }
    uint8_t *id = NULL, *old_mac = NULL;
    size_t id_length = 0, old_mac_length = 0;
    uint32_t res;
    if (HM_SUCCESS != (res = HM_PARAM_EXTRACT(
            in, HM_PARAM_BUFFER(&id, &id_length), HM_PARAM_BUFFER(&old_mac, &old_mac_length)))) {
        return res;
    }
    return hm_data_store_delete_block((hm_ds_db_t *) user_data, id, id_length, old_mac, old_mac_length);
}

