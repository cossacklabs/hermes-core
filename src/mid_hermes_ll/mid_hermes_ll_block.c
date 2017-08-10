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



#include <hermes/mid_hermes_ll/mid_hermes_ll.h>
#include <hermes/mid_hermes_ll/mid_hermes_ll_buffer.h>
#include <hermes/mid_hermes_ll/mid_hermes_ll_block.h>
#include <hermes/mid_hermes_ll/mid_hermes_ll_rights_list.h>
#include <hermes/mid_hermes_ll/interfaces/key_store.h>

#include <hermes/common/errors.h>

#include "utils.h"
#include <assert.h>
#include <string.h>

mid_hermes_ll_block_t *mid_hermes_ll_block_init_empty(mid_hermes_ll_block_t *block, mid_hermes_ll_user_t *user) {
    block->user = user;
    return block;
}

mid_hermes_ll_block_t *mid_hermes_ll_block_init(
        mid_hermes_ll_block_t *block, mid_hermes_ll_user_t *user, mid_hermes_ll_buffer_t *id,
        mid_hermes_ll_buffer_t *block_data, mid_hermes_ll_buffer_t *meta,
        mid_hermes_ll_token_t *read_token, mid_hermes_ll_token_t *write_token) {
    block->user = user;
    if (!read_token) {
        block->rtoken = mid_hermes_ll_token_generate(mid_hermes_ll_user_copy(user));
        block->wtoken = mid_hermes_ll_token_generate(mid_hermes_ll_user_copy(user));
        if (!(block->rtoken) || !(block->wtoken)) {
            return NULL;
        }
    }
    mid_hermes_ll_buffer_t *rt = mid_hermes_ll_token_get_data(read_token ? read_token : (block->rtoken));
    if (!rt) {
        return NULL;
    }
    if (read_token) {
        block->data = mid_hermes_ll_buffer_create(NULL, 0);
        if (!(block->data) || 0 != hm_decrypt(
                rt->data, rt->length, block_data->data, block_data->length, meta->data, meta->length,
                &(block->data->data), &(block->data->length))) {
            mid_hermes_ll_buffer_destroy(&rt);
            return NULL;
        }
    } else {
        block->block = mid_hermes_ll_buffer_create(NULL, 0);
        if (!(block->block) || HM_SUCCESS != hm_encrypt(
                rt->data, rt->length, block_data->data, block_data->length, meta->data, meta->length,
                &(block->block->data), &(block->block->length))) {
            mid_hermes_ll_buffer_destroy(&rt);
            return NULL;
        }
    }
    mid_hermes_ll_buffer_destroy(&rt);
    if (write_token || block->wtoken) {
        mid_hermes_ll_buffer_t *wt = mid_hermes_ll_token_get_data(write_token ? write_token : (block->wtoken));
        if (!wt) {
            return NULL;
        }
        block->mac = mid_hermes_ll_buffer_create(NULL, 0);
        if (!(block->mac) || HM_SUCCESS != hm_mac_create(
                wt->data, wt->length,
                read_token ? (block->data->data) : block_data->data,
                read_token ? (block->data->length) : block_data->length,
                meta->data, meta->length, &(block->mac->data), &(block->mac->length))) {
            mid_hermes_ll_buffer_destroy(&wt);
            return NULL;
        }
        mid_hermes_ll_buffer_destroy(&wt);
    }
    block->id = id;
    block->meta = meta;
    if (read_token) {
        block->block = block_data;
        block->rtoken = read_token;
        block->wtoken = write_token;
    } else {
        block->data = block_data;
    }
    return block;
}

mid_hermes_ll_block_t *mid_hermes_ll_block_update(
        mid_hermes_ll_block_t *bl, mid_hermes_ll_buffer_t *data, mid_hermes_ll_buffer_t *meta) {
    if (!bl || !data || !meta || !(bl->rtoken) || !(bl->wtoken)) {
        return NULL;
    }
    mid_hermes_ll_buffer_t *rt = mid_hermes_ll_token_get_data(bl->rtoken);
    mid_hermes_ll_buffer_t *new_block = mid_hermes_ll_buffer_create(NULL, 0);
    mid_hermes_ll_buffer_t *wt = mid_hermes_ll_token_get_data(bl->wtoken);
    mid_hermes_ll_buffer_t *new_mac = mid_hermes_ll_buffer_create(NULL, 0);
    if (!rt
        || !wt
        || !new_block
        || !new_mac
        || (HM_SUCCESS != hm_encrypt(
            rt->data, rt->length, data->data, data->length, meta->data, meta->length,
            &(new_block->data), &(new_block->length)))
        || (HM_SUCCESS != hm_mac_create(
            wt->data, wt->length, data->data, data->length, meta->data, meta->length,
            &(new_mac->data), &(new_mac->length)))) {
        mid_hermes_ll_buffer_destroy(&rt);
        mid_hermes_ll_buffer_destroy(&new_block);
        mid_hermes_ll_buffer_destroy(&wt);
        mid_hermes_ll_buffer_destroy(&new_mac);
        return NULL;
    }
    mid_hermes_ll_buffer_destroy(&rt);
    mid_hermes_ll_buffer_destroy(&wt);
    mid_hermes_ll_buffer_destroy(&(bl->data));
    mid_hermes_ll_buffer_destroy(&(bl->meta));
    mid_hermes_ll_buffer_destroy(&(bl->block));
    bl->meta = meta;
    bl->data = data;
    bl->block = new_block;
    if (bl->old_mac) {
        mid_hermes_ll_buffer_destroy(&(bl->mac));
    } else {
        bl->old_mac = bl->mac;
    }
    bl->mac = new_mac;
    return bl;
}

mid_hermes_ll_block_t *mid_hermes_ll_block_rotate(mid_hermes_ll_block_t *bl, mid_hermes_ll_rights_list_t *rights) {
    if (!bl || !rights || !(bl->rtoken) || !(bl->wtoken)) {
        return NULL;
    }
    mid_hermes_ll_token_t *new_rtoken = mid_hermes_ll_token_generate(mid_hermes_ll_user_copy(bl->user));
    mid_hermes_ll_token_t *new_wtoken = mid_hermes_ll_token_generate(mid_hermes_ll_user_copy(bl->user));
    mid_hermes_ll_buffer_t *new_block = mid_hermes_ll_buffer_create(NULL, 0);
    mid_hermes_ll_buffer_t *new_mac = mid_hermes_ll_buffer_create(NULL, 0);
    mid_hermes_ll_buffer_t *rt = mid_hermes_ll_token_get_data(new_rtoken);
    mid_hermes_ll_buffer_t *wt = mid_hermes_ll_token_get_data(new_wtoken);
    if (!new_rtoken
        || !new_wtoken
        || !new_block
        || !new_mac
        || !rt
        || !wt
        || (HM_SUCCESS != hm_encrypt(
            rt->data, rt->length, bl->data->data, bl->data->length, bl->meta->data, bl->meta->length,
            &(new_block->data), &(new_block->length)))
        || (HM_SUCCESS != hm_mac_create(
            wt->data, wt->length, bl->data->data, bl->data->length, bl->meta->data, bl->meta->length,
            &(new_mac->data), &(new_mac->length)))) {
        mid_hermes_ll_token_destroy(&new_rtoken);
        mid_hermes_ll_token_destroy(&new_wtoken);
        mid_hermes_ll_buffer_destroy(&new_block);
        mid_hermes_ll_buffer_destroy(&new_mac);
        mid_hermes_ll_buffer_destroy(&rt);
        mid_hermes_ll_buffer_destroy(&wt);
        return NULL;
    }
    mid_hermes_ll_buffer_destroy(&rt);
    mid_hermes_ll_buffer_destroy(&wt);
    mid_hermes_ll_token_destroy(&(bl->rtoken));
    mid_hermes_ll_token_destroy(&(bl->wtoken));
    mid_hermes_ll_buffer_destroy(&(bl->block));
    bl->rtoken = new_rtoken;
    bl->wtoken = new_wtoken;
    bl->block = new_block;
    if (bl->old_mac) {
        mid_hermes_ll_buffer_destroy(&(bl->mac));
    } else {
        bl->old_mac = bl->mac;
    }
    bl->mac = new_mac;

    mid_hermes_ll_rights_list_node_t *node = NULL;
    mid_hermes_ll_rights_list_iterator_t *i = mid_hermes_ll_rights_list_iterator_create(rights);
    if (!i) {
        return NULL;
    }
    while ((node = mid_hermes_ll_rights_list_iterator_next(i))) {
        mid_hermes_ll_token_t *nrt = mid_hermes_ll_token_get_token_for_user(new_rtoken,
                                                                            mid_hermes_ll_user_copy(node->user));
        mid_hermes_ll_token_t *nwt = NULL;
        if (node->wtoken) {
            nwt = mid_hermes_ll_token_get_token_for_user(new_wtoken, mid_hermes_ll_user_copy(node->user));
        }
        if (!nrt) {
            mid_hermes_ll_rights_list_iterator_destroy(&i);
            mid_hermes_ll_token_destroy(&nrt);
            mid_hermes_ll_token_destroy(&nwt);
            return NULL;
        }
        mid_hermes_ll_token_destroy(&(node->rtoken));
        mid_hermes_ll_token_destroy(&(node->wtoken));
        node->rtoken = nrt;
        node->wtoken = nwt;
    }
    mid_hermes_ll_rights_list_iterator_destroy(&i);
    return bl;
}

mid_hermes_ll_token_t *mid_hermes_ll_block_rtoken_for(mid_hermes_ll_block_t *bl, mid_hermes_ll_user_t *for_user) {
    if (!bl || !(bl->rtoken) || !for_user) {
        return NULL;
    }
    return mid_hermes_ll_token_get_token_for_user(bl->rtoken, for_user);
}

mid_hermes_ll_token_t *mid_hermes_ll_block_wtoken_for(mid_hermes_ll_block_t *bl, mid_hermes_ll_user_t *for_user) {
    if (!bl || !(bl->wtoken) || !for_user) {
        return NULL;
    }
    return mid_hermes_ll_token_get_token_for_user(bl->wtoken, for_user);
}

//store dependent functions
mid_hermes_ll_rights_list_t *mid_hermes_ll_block_access_rights(
        mid_hermes_ll_block_t *bl, hermes_key_store_t *key_store, hermes_credential_store_t *credential_store) {
    if (!bl || !(bl->id) || !(bl->mac) || !key_store || !credential_store) {
        return NULL;
    }
    mid_hermes_ll_rights_list_t *rights = mid_hermes_ll_rights_list_create();
    assert(rights);
    hermes_key_store_iterator_t *i = hermes_key_store_iterator_create(key_store, bl->id->data, bl->id->length);
    assert(i);
    do {
        mid_hermes_ll_user_t *user = mid_hermes_ll_user_load_c(
                hermes_key_store_iterator_get_user_id(i), hermes_key_store_iterator_get_user_id_length(i),
                credential_store);
        if (user && mid_hermes_ll_user_is_equal(user, bl->user)) {
            mid_hermes_ll_user_destroy(&user);
            continue;
        }
        mid_hermes_ll_token_t *rtoken = mid_hermes_ll_rtoken_load_c(
                hermes_key_store_iterator_get_user_id(i), hermes_key_store_iterator_get_user_id_length(i),
                bl->id->data, bl->id->length, key_store, credential_store);
        mid_hermes_ll_token_t *wtoken = mid_hermes_ll_wtoken_load_c(
                hermes_key_store_iterator_get_user_id(i), hermes_key_store_iterator_get_user_id_length(i),
                bl->id->data, bl->id->length, key_store, credential_store);
        if (!user
            || !rtoken
            || (HM_SUCCESS != mid_hermes_ll_rights_list_rpush_val(rights, user, rtoken, wtoken))) {
            mid_hermes_ll_user_destroy(&user);
            mid_hermes_ll_token_destroy(&rtoken);
            mid_hermes_ll_token_destroy(&wtoken);
            mid_hermes_ll_rights_list_destroy(&rights);
            hermes_key_store_iterator_destroy(&i);
            return NULL;
        }
    } while (HM_SUCCESS == hermes_key_store_iterator_next(i));
    hermes_key_store_iterator_destroy(&i);
    return rights;
}

mid_hermes_ll_token_t *mid_hermes_ll_token_load(
        mid_hermes_ll_user_t *user, mid_hermes_ll_buffer_t *block_id,
        hermes_key_store_t *key_store, hermes_credential_store_t *credential_store, bool is_update) {
    if (!user || !block_id || !key_store || !credential_store) {
        return NULL;
    }
    mid_hermes_ll_buffer_t *owner_id = mid_hermes_ll_buffer_create(NULL, 0);
    mid_hermes_ll_buffer_t *owner_pub_key = mid_hermes_ll_buffer_create(NULL, 0);
    mid_hermes_ll_buffer_t *token = mid_hermes_ll_buffer_create(NULL, 0);
    mid_hermes_ll_user_t *owner = NULL;
    mid_hermes_ll_token_t *read_token = NULL;
    if (!owner_id
        || !owner_pub_key
        || !token
        || (HM_SUCCESS != (is_update ? hermes_key_store_get_wtoken : hermes_key_store_get_rtoken)(
            key_store, user->id->data, user->id->length, block_id->data, block_id->length,
            &(token->data), &(token->length), &(owner_id->data), &owner_id->length))
        || (HM_SUCCESS != hermes_credential_store_get_public_key(
            credential_store, owner_id->data, owner_id->length, &(owner_pub_key->data), &(owner_pub_key->length)))
        || !(owner = mid_hermes_ll_user_create(owner_id, owner_pub_key))) {
        mid_hermes_ll_buffer_destroy(&owner_id);
        mid_hermes_ll_buffer_destroy(&owner_pub_key);
        mid_hermes_ll_buffer_destroy(&token);
        return NULL;
    }
    if (!(read_token = mid_hermes_ll_token_create(user, owner, token))) {
        mid_hermes_ll_user_destroy(&owner);
        mid_hermes_ll_buffer_destroy(&token);
        return NULL;
    }
    return read_token;
}

mid_hermes_ll_block_t *mid_hermes_ll_block_load(
        mid_hermes_ll_block_t *bl, mid_hermes_ll_buffer_t *id,
        hermes_data_store_t *data_store,
        hermes_key_store_t *key_store, hermes_credential_store_t *credential_store) {
    if (!bl || !id || !data_store || !key_store || !credential_store) {
        return NULL;
    }
    mid_hermes_ll_buffer_t *data = mid_hermes_ll_buffer_create(NULL, 0);
    mid_hermes_ll_buffer_t *meta = mid_hermes_ll_buffer_create(NULL, 0);
    mid_hermes_ll_user_t *read_token_user = mid_hermes_ll_user_copy(bl->user);
    mid_hermes_ll_user_t *write_token_user = mid_hermes_ll_user_copy(bl->user);
    mid_hermes_ll_token_t *read_token = mid_hermes_ll_token_load(read_token_user, id, key_store, credential_store,
                                                                 false);
    if (!read_token) {
        mid_hermes_ll_user_destroy(&read_token_user);
    }
    mid_hermes_ll_token_t *write_token = mid_hermes_ll_token_load(write_token_user, id, key_store, credential_store,
                                                                  true);
    if (!write_token) {
        mid_hermes_ll_user_destroy(&write_token_user);
    }
    if (!data
        || !meta
        || !read_token
        || (HM_SUCCESS != hermes_data_store_get_block(
            data_store, id->data, id->length, &(data->data), &(data->length), &(meta->data), &(meta->length)))
        || !(bl->init(bl, bl->user, id, data, meta, read_token, write_token))) {
        mid_hermes_ll_buffer_destroy(&data);
        mid_hermes_ll_buffer_destroy(&meta);
        mid_hermes_ll_token_destroy(&read_token);
        mid_hermes_ll_token_destroy(&write_token);
        return NULL;
    }
    return bl;
}

hermes_status_t mid_hermes_ll_rtoken_save(
        mid_hermes_ll_block_t *bl, mid_hermes_ll_token_t *token, hermes_key_store_t *key_store) {
    if (!token || !key_store) {
        return HM_FAIL;
    }
    return hermes_key_store_set_rtoken(
            key_store, token->user->id->data, token->user->id->length, bl->id->data, bl->id->length,
            token->token->data, token->token->length, token->owner->id->data, token->owner->id->length);
}

hermes_status_t mid_hermes_ll_wtoken_save(
        mid_hermes_ll_block_t *bl, mid_hermes_ll_token_t *token, hermes_key_store_t *key_store) {
    if (!token || !key_store) {
        return HM_FAIL;
    }
    return hermes_key_store_set_wtoken(
            key_store, token->user->id->data, token->user->id->length, bl->id->data, bl->id->length,
            token->token->data, token->token->length, token->owner->id->data, token->owner->id->length);
}

mid_hermes_ll_block_t *mid_hermes_ll_block_save(
        mid_hermes_ll_block_t *bl, mid_hermes_ll_rights_list_t *rights,
        hermes_data_store_t *data_store, hermes_key_store_t *key_store) {
    if (!bl || (!data_store && !key_store)) {
        return NULL;
    }
    if (data_store) {
        if (!(bl->id)) {
            bl->id = mid_hermes_ll_buffer_create(NULL, 0);
            if (!(bl->id)) {
                return NULL;
            }
        }
        if (HM_SUCCESS != hermes_data_store_set_block(
                data_store, &(bl->id->data), &(bl->id->length), bl->block->data, bl->block->length,
                bl->meta->data, bl->meta->length, bl->mac->data, bl->mac->length,
                (bl->old_mac) ? bl->old_mac->data : NULL, (bl->old_mac) ? bl->old_mac->length : 0)) {
            return NULL;
        }
        mid_hermes_ll_buffer_destroy(&(bl->old_mac));
    }
    if (key_store) {
        if (HM_SUCCESS != mid_hermes_ll_rtoken_save(bl, bl->rtoken, key_store)) {
            return NULL;
        }
        if (HM_SUCCESS != mid_hermes_ll_wtoken_save(bl, bl->wtoken, key_store)) {
            return NULL;
        }
    }
    if (rights && key_store) {
        mid_hermes_ll_rights_list_node_t *node;
        mid_hermes_ll_rights_list_iterator_t *iterator = mid_hermes_ll_rights_list_iterator_create(rights);
        if (!iterator) {
            return NULL;
        }
        while ((node = mid_hermes_ll_rights_list_iterator_next(iterator))) {
            if (HM_SUCCESS != mid_hermes_ll_rtoken_save(bl, node->rtoken, key_store)) {
                mid_hermes_ll_rights_list_iterator_destroy(&iterator);
                return NULL;
            }
            if (node->wtoken && HM_SUCCESS != mid_hermes_ll_wtoken_save(bl, node->wtoken, key_store)) {
                mid_hermes_ll_rights_list_iterator_destroy(&iterator);
                return NULL;
            }
        }
        mid_hermes_ll_rights_list_iterator_destroy(&iterator);
    }
    return bl;
}

hermes_status_t mid_hermes_ll_rtoken_delete(
        mid_hermes_ll_block_t *bl, mid_hermes_ll_token_t *token, hermes_key_store_t *key_store) {
    if (!token || !key_store) {
        return HM_FAIL;
    }
    return hermes_key_store_set_rtoken(
            key_store, token->user->id->data, token->user->id->length, bl->id->data, bl->id->length, NULL, 0,
            token->owner->id->data, token->owner->id->length);
}

hermes_status_t mid_hermes_ll_wtoken_delete(
        mid_hermes_ll_block_t *bl, mid_hermes_ll_token_t *token, hermes_key_store_t *key_store) {
    if (!token || !key_store) {
        return HM_FAIL;
    }
    return hermes_key_store_set_wtoken(
            key_store, token->user->id->data, token->user->id->length, bl->id->data, bl->id->length, NULL, 0,
            token->owner->id->data, token->owner->id->length);
}

mid_hermes_ll_block_t *mid_hermes_ll_block_delete(
        mid_hermes_ll_block_t *bl, mid_hermes_ll_rights_list_t *rights,
        hermes_data_store_t *data_store, hermes_key_store_t *key_store) {
    if (!bl || !(bl->id) || !(bl->mac) || (!data_store && !key_store)) {
        return NULL;
    }
    if (data_store
        && HM_SUCCESS != hermes_data_store_rem_block(
            data_store, bl->id->data, bl->id->length,
            bl->old_mac ? bl->old_mac->data : bl->mac->data, bl->old_mac ? bl->old_mac->length : bl->mac->length)) {
        return NULL;
    }
    if (key_store){
        if(HM_SUCCESS != mid_hermes_ll_rtoken_delete(bl, bl->rtoken, key_store)){
            return NULL;
        }
    }
    if (rights && key_store) {
        mid_hermes_ll_rights_list_node_t *node;
        mid_hermes_ll_rights_list_iterator_t *iterator = mid_hermes_ll_rights_list_iterator_create(rights);
        if (!iterator) {
            return NULL;
        }
        while ((node = mid_hermes_ll_rights_list_iterator_next(iterator))) {
            if (HM_SUCCESS != mid_hermes_ll_rtoken_delete(bl, node->rtoken, key_store)) {
                mid_hermes_ll_rights_list_iterator_destroy(&iterator);
                return NULL;
            }
            if (node->wtoken && HM_SUCCESS != mid_hermes_ll_wtoken_delete(bl, node->wtoken, key_store)) {
                mid_hermes_ll_rights_list_iterator_destroy(&iterator);
                return NULL;
            }
        }
        mid_hermes_ll_rights_list_iterator_destroy(&iterator);
    }
    return bl;
}

//

mid_hermes_ll_block_t *mid_hermes_ll_block_create_empty(mid_hermes_ll_user_t *user) {
    mid_hermes_ll_block_t *bl = calloc(1, sizeof(mid_hermes_ll_block_t));
    assert(bl);
    bl->init_empty = mid_hermes_ll_block_init_empty;
    bl->init = mid_hermes_ll_block_init;
    bl->update = mid_hermes_ll_block_update;
    bl->rotate = mid_hermes_ll_block_rotate;
    bl->access_rights = mid_hermes_ll_block_access_rights;
    bl->load = mid_hermes_ll_block_load;
    bl->save = mid_hermes_ll_block_save;
    bl->delete = mid_hermes_ll_block_delete;
    bl->rtoken_for = mid_hermes_ll_block_rtoken_for;
    bl->wtoken_for = mid_hermes_ll_block_wtoken_for;
    if (!(bl->init_empty(bl, user))) {
        mid_hermes_ll_block_destroy(&bl);
    }
    return bl;
}

mid_hermes_ll_block_t *mid_hermes_ll_block_create(
        mid_hermes_ll_user_t *user, mid_hermes_ll_buffer_t *id, mid_hermes_ll_buffer_t *block,
        mid_hermes_ll_buffer_t *meta, mid_hermes_ll_token_t *read_token, mid_hermes_ll_token_t *write_token) {
    mid_hermes_ll_block_t *bl = calloc(1, sizeof(mid_hermes_ll_block_t));
    assert(bl);
    bl->init_empty = mid_hermes_ll_block_init_empty;
    bl->init = mid_hermes_ll_block_init;
    bl->update = mid_hermes_ll_block_update;
    bl->rotate = mid_hermes_ll_block_rotate;
    bl->access_rights = mid_hermes_ll_block_access_rights;
    bl->load = mid_hermes_ll_block_load;
    bl->save = mid_hermes_ll_block_save;
    bl->delete = mid_hermes_ll_block_delete;
    bl->rtoken_for = mid_hermes_ll_block_rtoken_for;
    bl->wtoken_for = mid_hermes_ll_block_wtoken_for;
    if (!(bl->init(bl, user, id, block, meta, read_token, write_token))) {
        mid_hermes_ll_block_destroy(&bl);
    }
    return bl;
}

hermes_status_t mid_hermes_ll_block_destroy(mid_hermes_ll_block_t **b) {
    if (!b || !(*b)) {
        return HM_FAIL;
    }
    mid_hermes_ll_buffer_destroy(&((*b)->id));
    mid_hermes_ll_buffer_destroy(&((*b)->data));
    mid_hermes_ll_buffer_destroy(&((*b)->block));
    mid_hermes_ll_buffer_destroy(&((*b)->meta));
    mid_hermes_ll_buffer_destroy(&((*b)->mac));
    mid_hermes_ll_user_destroy(&((*b)->user));
    mid_hermes_ll_buffer_destroy(&((*b)->old_mac));
    mid_hermes_ll_token_destroy(&((*b)->rtoken));
    mid_hermes_ll_token_destroy(&((*b)->wtoken));
    free(*b);
    *b = NULL;
    return HM_SUCCESS;
}

#if 0

mid_hermes_ll_block_t* mid_hermes_ll_block_create_new(mid_hermes_ll_buffer_t* data,
                                                      mid_hermes_ll_buffer_t* meta,
                                                      mid_hermes_ll_user_t* user){
  HERMES_CHECK_IN_PARAM_RET_NULL(data);
  HERMES_CHECK_IN_PARAM_RET_NULL(meta);
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  mid_hermes_ll_block_t* b=calloc(1, sizeof(mid_hermes_ll_block_t));
  assert(b);
  b->rights=mid_hermes_ll_rights_list_create();
  if(!(b->rights)){
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  mid_hermes_ll_token_t* ert=mid_hermes_ll_token_generate(NULL, user);
  if(!ert){
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  mid_hermes_ll_token_t* ewt=mid_hermes_ll_token_generate(NULL, user);
  if(!ewt){
    mid_hermes_ll_token_destroy(&ert);
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  if(HM_SUCCESS!=mid_hermes_ll_rights_list_rpush(b->rights, mid_hermes_ll_rights_list_node_create(user, ert,ewt))){
    mid_hermes_ll_token_destroy(&ert);
    mid_hermes_ll_token_destroy(&ewt);    
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  mid_hermes_ll_buffer_t* wt=mid_hermes_ll_token_get_data(ewt);
  if(!wt){
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  b->mac=mid_hermes_ll_buffer_create(NULL, 0);
  hermes_status_t res=hm_mac_create(wt->data, wt->length, data->data, data->length, meta->data, meta->length, &(b->mac->data), &(b->mac->length));
  mid_hermes_ll_buffer_destroy(&wt);
  if(HM_SUCCESS!=res){
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  mid_hermes_ll_buffer_t* rt=mid_hermes_ll_token_get_data(ert);
  if(!rt){
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  b->block=mid_hermes_ll_buffer_create(NULL, 0);
  res=hm_encrypt(rt->data, rt->length, data->data, data->length, meta->data, meta->length, &(b->block->data), &(b->block->length));
  mid_hermes_ll_buffer_destroy(&rt);
  if(HM_SUCCESS!=res){
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  b->meta=meta;
  b->data=data;
  b->user=user;
  return b;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_create_new_with_id(mid_hermes_ll_buffer_t* id,
                                                              mid_hermes_ll_buffer_t* data,
                                                              mid_hermes_ll_buffer_t* meta,
                                                              mid_hermes_ll_user_t* user){
  HERMES_CHECK_IN_PARAM_RET_NULL(id);
  HERMES_CHECK_IN_PARAM_RET_NULL(data);
  HERMES_CHECK_IN_PARAM_RET_NULL(meta);
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  mid_hermes_ll_block_t* b=mid_hermes_ll_block_create_new(data, meta, user);
  if(!b){
    return NULL;
  }
  if(!(b=mid_hermes_ll_block_set_id(b, id))){
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  return b;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_restore(mid_hermes_ll_buffer_t* id,
                                                   mid_hermes_ll_buffer_t* block,
                                                   mid_hermes_ll_buffer_t* meta,
                                                   mid_hermes_ll_user_t* user,
                                                   mid_hermes_ll_rights_list_t* rights){
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  HERMES_CHECK_IN_PARAM_RET_NULL(id);
  HERMES_CHECK_IN_PARAM_RET_NULL(block);
  HERMES_CHECK_IN_PARAM_RET_NULL(meta);
  HERMES_CHECK_IN_PARAM_RET_NULL(rights);
  HERMES_CHECK_IN_PARAM_RET_NULL(rights->len>0);
  HERMES_CHECK_IN_PARAM_RET_NULL(mid_hermes_ll_rights_list_find(rights, user->id));
  mid_hermes_ll_block_t* b=calloc(1, sizeof(mid_hermes_ll_block_t));
  assert(b);
  b->id=id;
  b->block=block;
  b->meta=meta;
  b->rights=rights;
  b->user=user;
  return b;
}

mid_hermes_ll_buffer_t* mid_hermes_ll_block_get_data(mid_hermes_ll_block_t* b){
  HERMES_CHECK_IN_PARAM_RET_NULL(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->rights);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->rights->len>0);
  mid_hermes_ll_rights_list_node_t* ln=mid_hermes_ll_rights_list_find(b->rights, b->user->id);
  HERMES_CHECK_IN_PARAM_RET_NULL(ln);
  mid_hermes_ll_buffer_t* rt=mid_hermes_ll_token_get_data(ln->rtoken);
  HERMES_CHECK_IN_PARAM_RET_NULL(rt);
  mid_hermes_ll_buffer_t* bl=mid_hermes_ll_buffer_create(NULL,0);
  if(!bl){
    return NULL;
  }
  hermes_status_t res=hm_decrypt(rt->data, rt->length, b->block->data, b->block->length, b->meta->data, b->meta->length, &(bl->data), &(bl->length));
  mid_hermes_ll_buffer_destroy(&rt);
  if(HM_SUCCESS!=res){
    mid_hermes_ll_buffer_destroy(&bl);
    return NULL;
  }
  return bl;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_set_data(mid_hermes_ll_block_t* b, mid_hermes_ll_buffer_t* data, mid_hermes_ll_buffer_t* meta){
  HERMES_CHECK_IN_PARAM_RET_NULL(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->user);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->mac);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->rights);
  mid_hermes_ll_rights_list_node_t* ln=mid_hermes_ll_rights_list_find(b->rights, b->user->id);
  HERMES_CHECK_IN_PARAM_RET_NULL(ln);
  mid_hermes_ll_buffer_t* rt=mid_hermes_ll_token_get_data(ln->rtoken);
  HERMES_CHECK_IN_PARAM_RET_NULL(rt);
  mid_hermes_ll_buffer_t* wt=mid_hermes_ll_token_get_data(ln->wtoken);
  if(!wt){
    mid_hermes_ll_buffer_destroy(&rt);
    return NULL;
  }
  mid_hermes_ll_block_t* new_bl=mid_hermes_ll_block_create_new_with_id(b->id, data, meta, b->user);
  if(!new_bl){
    return NULL;
  }
  mid_hermes_ll_rights_list_iterator_t* i=mid_hermes_ll_rights_list_iterator_create(b->rights);
  mid_hermes_ll_rights_list_node_t* node=NULL;
  while((node=mid_hermes_ll_rights_list_iterator_next(i))){
    if(node->user->id->length != b->user->id->length || 0!=memcmp(node->user->id->data, b->user->id->data, b->user->id->length)){
      if(node->wtoken){
        if(HM_SUCCESS!=mid_hermes_ll_block_grant_update_access(new_bl, node->user)){
          new_bl->id=NULL;
          mid_hermes_ll_block_destroy(&new_bl);
          mid_hermes_ll_rights_list_iterator_destroy(&i);
          return NULL;
        }
      }else{
        if(HM_SUCCESS!=mid_hermes_ll_block_grant_read_access(new_bl, node->user)){
          new_bl->id=NULL;
          mid_hermes_ll_block_destroy(&new_bl);
          mid_hermes_ll_rights_list_iterator_destroy(&i);
          return NULL;
        }        
      }
    }
  }
  b->id=NULL;
  if(b->old_mac){
    new_bl->old_mac=b->old_mac;
    b->old_mac=NULL;    
  }else{
    new_bl->old_mac=b->mac;
    b->mac=NULL;
  }
  mid_hermes_ll_block_destroy(&b);
  return new_bl;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_grant_read_access(mid_hermes_ll_block_t* b, const mid_hermes_ll_user_t* user){
  HERMES_CHECK_IN_PARAM_RET_NULL(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->rights);
  HERMES_CHECK_IN_PARAM_RET_NULL(!mid_hermes_ll_rights_list_find(b->rights, user->id));
  mid_hermes_ll_rights_list_node_t* orig_tokens = mid_hermes_ll_rights_list_find(b->rights, b->user->id);
  HERMES_CHECK_IN_PARAM_RET_NULL(orig_tokens);
  HERMES_CHECK_IN_PARAM_RET_NULL(orig_tokens->rtoken);
  mid_hermes_ll_token_t* t=mid_hermes_ll_token_get_token_for_user(orig_tokens->rtoken, user);
  if(!t || HM_SUCCESS!=mid_hermes_ll_rights_list_rpush_val(b->rights, user, t, NULL)){
    mid_hermes_ll_token_destroy(&t);
    return NULL;
  }
  return b;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_grant_update_access_(mid_hermes_ll_block_t* b, const mid_hermes_ll_user_t* user){
  HERMES_CHECK_IN_PARAM_RET_NULL(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->mac);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->rights);
  mid_hermes_ll_rights_list_node_t* orig_tokens = mid_hermes_ll_rights_list_find(b->rights, b->user->id);
  HERMES_CHECK_IN_PARAM_RET_NULL(orig_tokens);
  HERMES_CHECK_IN_PARAM_RET_NULL(orig_tokens->rtoken);
  HERMES_CHECK_IN_PARAM_RET_NULL(orig_tokens->wtoken);
  mid_hermes_ll_rights_list_node_t* user_tokens = mid_hermes_ll_rights_list_find(b->rights, user->id);
  if(!user_tokens){
    if(HM_SUCCESS!=mid_hermes_ll_block_grant_read_access(b, user)){
      return NULL;
    }
    user_tokens = mid_hermes_ll_rights_list_find(b->rights, user->id);
  }
  HERMES_CHECK_IN_PARAM_RET_NULL(!(user_tokens->wtoken));
  user_tokens->wtoken=mid_hermes_ll_token_get_token_for_user(orig_tokens->wtoken, user);
  if(!(user_tokens->wtoken)){
    return NULL;
  }
  return b;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_grant_update_access(mid_hermes_ll_block_t* b, const mid_hermes_ll_user_t* user){
  HERMES_CHECK_IN_PARAM_RET_NULL(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  mid_hermes_ll_buffer_t* d=mid_hermes_ll_block_get_data(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(d);
  mid_hermes_ll_block_t* new_bl=mid_hermes_ll_block_set_data(b, d, b->meta);
  if(!new_bl || HM_SUCCESS!=mid_hermes_ll_block_grant_update_access_(new_bl, user)){
    new_bl->id=NULL;
    new_bl->meta=NULL;
    mid_hermes_ll_block_destroy(&new_bl);
    return NULL;
  }
  b->id=NULL;
  b->meta=NULL;
  mid_hermes_ll_block_destroy(&b);
  return new_bl;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_deny_read_access(mid_hermes_ll_block_t* b, const mid_hermes_ll_user_t* user){
  HERMES_CHECK_IN_PARAM_RET_NULL(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->rights);
  if(HM_SUCCESS!=mid_hermes_ll_rights_list_remove(b->rights, mid_hermes_ll_rights_list_find(b->rights, user->id))){
    return NULL;
  }
  mid_hermes_ll_buffer_t* d=mid_hermes_ll_block_get_data(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(d);
  mid_hermes_ll_block_t* new_bl=mid_hermes_ll_block_set_data(b, d, b->meta);
  HERMES_CHECK_IN_PARAM_RET_NULL(new_bl);
  b->id=NULL;
  b->meta=NULL;
  mid_hermes_ll_block_destroy(&b);
  return new_bl;  
}

mid_hermes_ll_block_t* mid_hermes_ll_block_deny_update_access(mid_hermes_ll_block_t* b, const mid_hermes_ll_user_t* user){
  HERMES_CHECK_IN_PARAM_RET_NULL(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->rights);
  mid_hermes_ll_rights_list_node_t* n=mid_hermes_ll_rights_list_find(b->rights, user->id);
  HERMES_CHECK_IN_PARAM_RET_NULL(n);
  mid_hermes_ll_token_destroy(&(n->wtoken));
  
  mid_hermes_ll_buffer_t* d=mid_hermes_ll_block_get_data(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(d);
  mid_hermes_ll_block_t* new_bl=mid_hermes_ll_block_set_data(b, d, b->meta);
  HERMES_CHECK_IN_PARAM_RET_NULL(new_bl);
  b->id=NULL;
  b->meta=NULL;
  mid_hermes_ll_block_destroy(&b);
  return new_bl;  
}

mid_hermes_ll_block_t* mid_hermes_ll_block_set_id(mid_hermes_ll_block_t* b, mid_hermes_ll_buffer_t* id){
  HERMES_CHECK_IN_PARAM_RET_NULL(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(id);
  b->id=id;
  return b;
}

hermes_status_t mid_hermes_ll_block_destroy(mid_hermes_ll_block_t** b){
  HERMES_CHECK_IN_PARAM(b);
  HERMES_CHECK_IN_PARAM(*b);
  mid_hermes_ll_buffer_destroy(&((*b)->id));
  mid_hermes_ll_buffer_destroy(&((*b)->block));
  mid_hermes_ll_buffer_destroy(&((*b)->data));
  mid_hermes_ll_buffer_destroy(&((*b)->meta));
  mid_hermes_ll_buffer_destroy(&((*b)->old_mac));
  mid_hermes_ll_buffer_destroy(&((*b)->mac));
  mid_hermes_ll_rights_list_destroy(&((*b)->rights));
  free(*b);
  *b=NULL;
  return HM_SUCCESS;
}

#endif
