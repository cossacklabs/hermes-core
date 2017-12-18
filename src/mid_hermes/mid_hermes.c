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



#include <hermes/mid_hermes/mid_hermes.h>
#include <hermes/mid_hermes_ll/mid_hermes_ll.h>
#include <hermes/mid_hermes_ll/interfaces/key_store.h>
#include <hermes/mid_hermes_ll/interfaces/data_store.h>
#include <hermes/mid_hermes_ll/interfaces/credential_store.h>
#include "key_store_impl.h"
#include "data_store_impl.h"
#include "credential_store_impl.h"
#include <string.h>
#include <assert.h>

struct mid_hermes_type {
    hermes_key_store_t *ks;
    hermes_credential_store_t *cs;
    hermes_data_store_t *ds;
    mid_hermes_ll_user_t *user;
};


mid_hermes_t *mid_hermes_create(
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *private_key, const size_t private_key_length,
        hm_rpc_transport_t *key_store_transport,
        hm_rpc_transport_t *data_store_transport,
        hm_rpc_transport_t *credential_store_transport) {
    if (!user_id || !user_id_length || !private_key || !private_key_length || !key_store_transport ||
        !data_store_transport || !credential_store_transport) {
        return NULL;
    }
    mid_hermes_t *mid_hermes = calloc(1, sizeof(mid_hermes_t));
    assert(mid_hermes);
    if (!(mid_hermes->cs = hermes_credential_store_create(credential_store_transport))
        || !(mid_hermes->ks = hermes_key_store_create(key_store_transport))
        || !(mid_hermes->ds = hermes_data_store_create(data_store_transport))
        ||
        !(mid_hermes->user = mid_hermes_ll_local_user_load_c(user_id, user_id_length, private_key, private_key_length,
                                                             mid_hermes->cs))) {
        mid_hermes_destroy(&mid_hermes);
        return NULL;
    }
    return mid_hermes;
}

hermes_status_t mid_hermes_destroy(mid_hermes_t **mh) {
    if (!mh || !(*mh)) {
        return HM_INVALID_PARAMETER;
    }
    hermes_key_store_destroy(&((*mh)->ks));
    hermes_data_store_destroy(&((*mh)->ds));
    hermes_credential_store_destroy(&((*mh)->cs));
    mid_hermes_ll_user_destroy(&((*mh)->user));
    free(*mh);
    *mh = NULL;
    return HM_SUCCESS;
}

hermes_status_t mid_hermes_create_block(
        mid_hermes_t *mid_hermes,
        uint8_t **id, size_t *id_length,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length) {
    if (!mid_hermes || !id || !id_length || !block || !block_length || !meta || !meta_length) {
        return HM_INVALID_PARAMETER;
    }
    mid_hermes_ll_block_t *bl = NULL;
    mid_hermes_ll_buffer_t *bl_id = NULL;
    mid_hermes_ll_user_t *bl_user = mid_hermes_ll_user_copy(mid_hermes->user);
    if (*id) {
        bl_id = mid_hermes_ll_buffer_create(*id, *id_length);
    }
    mid_hermes_ll_buffer_t *bl_data = mid_hermes_ll_buffer_create(block, block_length);
    mid_hermes_ll_buffer_t *bl_meta = mid_hermes_ll_buffer_create(meta, meta_length);
    if (!bl_data
        || !bl_user
        || !bl_meta
        || !(bl = mid_hermes_ll_block_create(bl_user, bl_id, bl_data, bl_meta, NULL, NULL))) {
        mid_hermes_ll_buffer_destroy(&bl_id);
        mid_hermes_ll_buffer_destroy(&bl_data);
        mid_hermes_ll_buffer_destroy(&bl_meta);
        mid_hermes_ll_user_destroy(&bl_user);
        return HM_FAIL;
    }
    if (!(bl->save(bl, NULL, mid_hermes->ds, mid_hermes->ks))) {
        mid_hermes_ll_block_destroy(&bl);
        return HM_FAIL;

    }
    // if id was empty then copy generated id
    if(!(*id)){
        *id = malloc(bl->id->length);
        if (!*id){
            mid_hermes_ll_block_destroy(&bl);
            return HM_FAIL;
        }
        memcpy(*id, bl->id->data, bl->id->length);
        *id_length = bl->id->length;
    }
    mid_hermes_ll_block_destroy(&bl);
    return HM_SUCCESS;
}

hermes_status_t mid_hermes_read_block(
        mid_hermes_t *mid_hermes,
        const uint8_t *block_id, const size_t block_id_length,
        uint8_t **block, size_t *block_length,
        uint8_t **meta, size_t *meta_length) {
    if (!mid_hermes || !block_id || !block_id_length || !block || !block_length || !meta || !meta_length) {
        return HM_INVALID_PARAMETER;
    }
    mid_hermes_ll_user_t *bl_user = mid_hermes_ll_user_copy(mid_hermes->user);
    mid_hermes_ll_buffer_t *bl_id = mid_hermes_ll_buffer_create(block_id, block_id_length);
    mid_hermes_ll_block_t *bl = NULL;
    if (!bl_user
        || !bl_id
        || !(bl = mid_hermes_ll_block_create_empty(bl_user))) {
        mid_hermes_ll_user_destroy(&bl_user);
        mid_hermes_ll_buffer_destroy(&bl_id);
        return HM_FAIL;
    }
    if (!(bl->load(bl, bl_id, mid_hermes->ds, mid_hermes->ks, mid_hermes->cs))) {
        mid_hermes_ll_block_destroy(&bl);
        mid_hermes_ll_buffer_destroy(&bl_id);
        return HM_FAIL;
    }
    *block = bl->data->data;
    *block_length = bl->data->length;
    *meta = bl->meta->data;
    *meta_length = bl->meta->length;
    bl->data->data = NULL;
    bl->data->length = 0;
    bl->meta->data = NULL;
    bl->meta->length = 0;
    mid_hermes_ll_block_destroy(&bl);
    return HM_SUCCESS;
}

hermes_status_t mid_hermes_update_block(
        mid_hermes_t *mid_hermes,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *block, const size_t block_length,
        const uint8_t *meta, const size_t meta_length) {
    if (!mid_hermes || !block_id || !block_id_length || !block || !block_length || !meta || !meta_length) {
        return HM_INVALID_PARAMETER;
    }
    mid_hermes_ll_user_t *bl_user = mid_hermes_ll_user_copy(mid_hermes->user);
    mid_hermes_ll_block_t *bl = NULL;
    if (!bl_user
        || !(bl = mid_hermes_ll_block_create_empty(bl_user))) {
        mid_hermes_ll_user_destroy(&bl_user);
        return HM_FAIL;
    }
    mid_hermes_ll_buffer_t *bl_id = mid_hermes_ll_buffer_create(block_id, block_id_length);
    mid_hermes_ll_buffer_t *new_meta = mid_hermes_ll_buffer_create(meta, meta_length);
    mid_hermes_ll_buffer_t *new_data = mid_hermes_ll_buffer_create(block, block_length);
    if (!bl_id
        || !new_meta
        || !new_data
        || !(bl->load(bl, bl_id, mid_hermes->ds, mid_hermes->ks, mid_hermes->cs))) {
        mid_hermes_ll_buffer_destroy(&bl_id);
        mid_hermes_ll_buffer_destroy(&new_data);
        mid_hermes_ll_buffer_destroy(&new_meta);
        mid_hermes_ll_block_destroy(&bl);
        return 1;
    }
    if (!(bl->update(bl, new_data, new_meta))) {
        mid_hermes_ll_buffer_destroy(&new_data);
        mid_hermes_ll_buffer_destroy(&new_meta);
        mid_hermes_ll_block_destroy(&bl);
        return 1;
    }
    if (!bl->save(bl, NULL, mid_hermes->ds, mid_hermes->ks)) {
        mid_hermes_ll_block_destroy(&bl);
        return 1;
    }
    mid_hermes_ll_block_destroy(&bl);
    return 0;
}

hermes_status_t mid_hermes_delete_block(
        mid_hermes_t *mid_hermes, const uint8_t *block_id, const size_t block_id_length) {
    if (!mid_hermes || !block_id || !block_id_length) {
        return HM_INVALID_PARAMETER;
    }
    mid_hermes_ll_user_t *bl_user = mid_hermes_ll_user_copy(mid_hermes->user);
    mid_hermes_ll_block_t *bl = NULL;
    if (!bl_user
        || !(bl = mid_hermes_ll_block_create_empty(bl_user))) {
        mid_hermes_ll_user_destroy(&bl_user);
        return HM_FAIL;
    }
    mid_hermes_ll_buffer_t *bl_id = mid_hermes_ll_buffer_create(block_id, block_id_length);
    if (!bl_id
        || !(bl->load(bl, bl_id, mid_hermes->ds, mid_hermes->ks, mid_hermes->cs))) {
        mid_hermes_ll_buffer_destroy(&bl_id);
        mid_hermes_ll_block_destroy(&bl);
        return 1;
    }
    mid_hermes_ll_rights_list_t *rights_list = bl->access_rights(bl, mid_hermes->ks, mid_hermes->cs);
    if (!bl->delete(bl, rights_list, mid_hermes->ds, mid_hermes->ks)) {
        mid_hermes_ll_block_destroy(&bl);
        return HM_FAIL;
    }
    mid_hermes_ll_block_destroy(&bl);
    return HM_SUCCESS;
}

hermes_status_t mid_hermes_rotate_block(
        mid_hermes_t *mid_hermes, const uint8_t *block_id, const size_t block_id_length) {
    if (!mid_hermes || !block_id || !block_id_length) {
        return HM_INVALID_PARAMETER;
    }
    mid_hermes_ll_user_t *bl_user = mid_hermes_ll_user_copy(mid_hermes->user);
    mid_hermes_ll_block_t *bl = NULL;
    if (!bl_user
        || !(bl = mid_hermes_ll_block_create_empty(bl_user))) {
        mid_hermes_ll_user_destroy(&bl_user);
        return HM_FAIL;
    }
    mid_hermes_ll_buffer_t *bl_id = mid_hermes_ll_buffer_create(block_id, block_id_length);
    if (!bl_id
        || !(bl->load(bl, bl_id, mid_hermes->ds, mid_hermes->ks, mid_hermes->cs))) {
        mid_hermes_ll_buffer_destroy(&bl_id);
        mid_hermes_ll_block_destroy(&bl);
        return HM_FAIL;
    }
    mid_hermes_ll_rights_list_t *rights_list = bl->access_rights(bl, mid_hermes->ks, mid_hermes->cs);
    if (!rights_list || !(bl->rotate(bl, rights_list)) ||
        !(bl->save(bl, rights_list, mid_hermes->ds, mid_hermes->ks))) {
        mid_hermes_ll_rights_list_destroy(&rights_list);
        mid_hermes_ll_block_destroy(&bl);
        return HM_FAIL;
    }
    mid_hermes_ll_rights_list_destroy(&rights_list);
    mid_hermes_ll_block_destroy(&bl);
    return HM_SUCCESS;
}

hermes_status_t mid_hermes_grant_read_access(
        mid_hermes_t *mid_hermes,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *for_user_id, const size_t for_user_id_length) {
    if (!mid_hermes || !block_id || !block_id_length || !for_user_id || !for_user_id_length) {
        return HM_INVALID_PARAMETER;
    }
    mid_hermes_ll_user_t *bl_user = mid_hermes_ll_user_copy(mid_hermes->user);
    mid_hermes_ll_block_t *bl = NULL;
    if (!bl_user
        || !(bl = mid_hermes_ll_block_create_empty(bl_user))) {
        mid_hermes_ll_user_destroy(&bl_user);
        return HM_FAIL;
    }
    mid_hermes_ll_buffer_t *bl_id = mid_hermes_ll_buffer_create(block_id, block_id_length);
    if (!bl_id
        || !(bl->load(bl, bl_id, mid_hermes->ds, mid_hermes->ks, mid_hermes->cs))) {
        mid_hermes_ll_block_destroy(&bl);
        mid_hermes_ll_buffer_destroy(&bl_id);
        return HM_FAIL;
    }
    mid_hermes_ll_user_t *for_user = mid_hermes_ll_user_load_c(for_user_id, for_user_id_length, mid_hermes->cs);
    mid_hermes_ll_token_t *read_token = NULL;
    if (!for_user
        || !(read_token = bl->rtoken_for(bl, for_user))) {
        mid_hermes_ll_user_destroy(&for_user);
        mid_hermes_ll_block_destroy(&bl);
        return HM_FAIL;
    }
    if (HM_SUCCESS != mid_hermes_ll_token_save(bl->user, bl->id, read_token, mid_hermes->ks, false)) {
        mid_hermes_ll_block_destroy(&bl);
        mid_hermes_ll_token_destroy(&read_token);
        return HM_FAIL;
    }
    mid_hermes_ll_block_destroy(&bl);
    mid_hermes_ll_token_destroy(&read_token);
    return HM_SUCCESS;
}

hermes_status_t mid_hermes_grant_update_access(
        mid_hermes_t *mid_hermes,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *for_user_id, const size_t for_user_id_length) {
    if (!mid_hermes || !block_id || !block_id_length || !for_user_id || !for_user_id_length) {
        return HM_INVALID_PARAMETER;
    }
    mid_hermes_ll_user_t *bl_user = mid_hermes_ll_user_copy(mid_hermes->user);
    mid_hermes_ll_block_t *bl = NULL;
    if (!bl_user
        || !(bl = mid_hermes_ll_block_create_empty(bl_user))) {
        mid_hermes_ll_user_destroy(&bl_user);
        return HM_FAIL;
    }
    mid_hermes_ll_buffer_t *bl_id = mid_hermes_ll_buffer_create(block_id, block_id_length);
    if (!bl_id
        || !(bl->load(bl, bl_id, mid_hermes->ds, mid_hermes->ks, mid_hermes->cs))) {
        mid_hermes_ll_block_destroy(&bl);
        mid_hermes_ll_buffer_destroy(&bl_id);
        return HM_FAIL;
    }
    mid_hermes_ll_user_t *for_user = mid_hermes_ll_user_load_c(for_user_id,
                                                               for_user_id_length,
                                                               mid_hermes->cs);
    mid_hermes_ll_token_t *write_token = NULL;
    if (!for_user
        || !(write_token = bl->wtoken_for(bl, for_user))) {
        mid_hermes_ll_user_destroy(&for_user);
        mid_hermes_ll_block_destroy(&bl);
        return HM_FAIL;
    }
    if (HM_SUCCESS != mid_hermes_ll_token_save(bl->user, bl->id, write_token, mid_hermes->ks, true)) {
        mid_hermes_ll_block_destroy(&bl);
        mid_hermes_ll_token_destroy(&write_token);
        return HM_FAIL;
    }
    mid_hermes_ll_block_destroy(&bl);
    mid_hermes_ll_token_destroy(&write_token);
    return HM_SUCCESS;
}

hermes_status_t mid_hermes_deny_read_access(
        mid_hermes_t *mid_hermes,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *for_user_id, const size_t for_user_id_length) {
    if (!mid_hermes || !block_id || !block_id_length || !for_user_id || !for_user_id_length) {
        return HM_INVALID_PARAMETER;
    }
    mid_hermes_ll_user_t *bl_user = mid_hermes_ll_user_copy(mid_hermes->user);
    mid_hermes_ll_block_t *bl = NULL;
    if (!bl_user
        || !(bl = mid_hermes_ll_block_create_empty(bl_user))) {
        mid_hermes_ll_user_destroy(&bl_user);
        return HM_FAIL;
    }
    mid_hermes_ll_buffer_t *bl_id = mid_hermes_ll_buffer_create(block_id, block_id_length);
    // load block with user's keys that has access
    if (!bl_id
        || !(bl->load(bl, bl_id, mid_hermes->ds, mid_hermes->ks, mid_hermes->cs))) {
        mid_hermes_ll_block_destroy(&bl);
        mid_hermes_ll_buffer_destroy(&bl_id);
        return HM_FAIL;
    }
    // load user which access will be revoked
    mid_hermes_ll_user_t *for_user = mid_hermes_ll_user_load_c(for_user_id, for_user_id_length, mid_hermes->cs);
    mid_hermes_ll_token_t *wtoken = NULL;

    if (!for_user
        || !(wtoken = bl->wtoken_for(bl, for_user))) {
        mid_hermes_ll_user_destroy(&for_user);
        mid_hermes_ll_block_destroy(&bl);
        return HM_FAIL;
    }
    if (HM_SUCCESS != mid_hermes_ll_token_del(for_user, bl->user, bl->id, mid_hermes->ks, false)) {
        mid_hermes_ll_block_destroy(&bl);
        mid_hermes_ll_token_destroy(&wtoken);
        return HM_FAIL;
    }
    mid_hermes_ll_block_destroy(&bl);
    mid_hermes_ll_token_destroy(&wtoken);
    return HM_SUCCESS;
}

hermes_status_t mid_hermes_deny_update_access(
        mid_hermes_t *mid_hermes,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *for_user_id, const size_t for_user_id_length) {
    if (!mid_hermes || !block_id || !block_id_length || !for_user_id || !for_user_id_length) {
        return HM_INVALID_PARAMETER;
    }
    mid_hermes_ll_user_t *bl_user = mid_hermes_ll_user_copy(mid_hermes->user);
    mid_hermes_ll_block_t *bl = NULL;
    if (!bl_user
        || !(bl = mid_hermes_ll_block_create_empty(bl_user))) {
        mid_hermes_ll_user_destroy(&bl_user);
        return HM_FAIL;
    }

    mid_hermes_ll_buffer_t *bl_id = mid_hermes_ll_buffer_create(block_id, block_id_length);
    if (!bl_id
        || !(bl->load(bl, bl_id, mid_hermes->ds, mid_hermes->ks, mid_hermes->cs))) {
        mid_hermes_ll_block_destroy(&bl);
        mid_hermes_ll_buffer_destroy(&bl_id);
        return HM_FAIL;
    }

    mid_hermes_ll_user_t *for_user = mid_hermes_ll_user_load_c(for_user_id, for_user_id_length, mid_hermes->cs);

    mid_hermes_ll_token_t *write_token = NULL;
    if (!for_user
        || !(write_token = bl->wtoken_for(bl, for_user))) {
        mid_hermes_ll_user_destroy(&for_user);
        mid_hermes_ll_block_destroy(&bl);
        return HM_FAIL;
    }
    if (HM_SUCCESS != mid_hermes_ll_token_del(for_user, bl->user, bl->id, mid_hermes->ks, true)) {
        mid_hermes_ll_block_destroy(&bl);
        mid_hermes_ll_token_destroy(&write_token);
        return HM_FAIL;
    }
    mid_hermes_ll_block_destroy(&bl);
    mid_hermes_ll_token_destroy(&write_token);
    return HM_SUCCESS;

}

