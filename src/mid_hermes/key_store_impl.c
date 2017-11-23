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




#include "key_store_impl.h"

#include <hermes/key_store/client.h>

#include <assert.h>
#include <string.h>

struct hermes_key_store_type {
    hm_key_store_client_sync_t *cl;
};

hermes_key_store_t *hermes_key_store_create(hm_rpc_transport_t *transport) {
    if (!transport) {
        return NULL;
    }
    hermes_key_store_t *key_store = calloc(1, sizeof(hermes_key_store_t));
    if (!key_store
        || !(key_store->cl = hm_key_store_client_sync_create(transport))) {
        hermes_key_store_destroy(&key_store);
        return NULL;
    }
    return key_store;
}

hermes_status_t hermes_key_store_get_rtoken(
        hermes_key_store_t *key_store,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        uint8_t **token, size_t *token_length,
        uint8_t **owner_id, size_t *owner_id_length) {
    if (!key_store || !user_id || !user_id_length || !block_id || !block_id_length || !token || !token_length ||
        !owner_id || !owner_id_length) {
        return HM_INVALID_PARAMETER;
    }
    return hm_key_store_client_sync_call_get_rtoken(
            key_store->cl,
            block_id, block_id_length,
            user_id, user_id_length,
            token, token_length,
            owner_id, owner_id_length);
}

hermes_status_t hermes_key_store_get_wtoken(
        hermes_key_store_t *key_store,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        uint8_t **token, size_t *token_length,
        uint8_t **owner_id, size_t *owner_id_length) {
    if (!key_store || !user_id || !user_id_length || !block_id || !block_id_length || !token || !token_length ||
        !owner_id || !owner_id_length) {
        return HM_INVALID_PARAMETER;
    }
    return hm_key_store_client_sync_call_get_wtoken(
            key_store->cl,
            block_id, block_id_length,
            user_id, user_id_length,
            token, token_length,
            owner_id, owner_id_length);
}

// hermes_key_store_set_rtoken set read <token> to keystore if token not null otherwise it delete read token
hermes_status_t hermes_key_store_set_rtoken(
        hermes_key_store_t *key_store,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *token, const size_t token_length,
        const uint8_t *owner_id, const size_t owner_id_length) {
    if (!key_store || !user_id || !user_id_length || !block_id || !block_id_length) {
        return HM_INVALID_PARAMETER;
    }
    if (token && token_length) {
        return hm_key_store_client_sync_call_set_rtoken(
                key_store->cl,
                block_id, block_id_length,
                user_id, user_id_length,
                owner_id, owner_id_length,
                token, token_length);
    }
    return hm_key_store_client_sync_call_del_rtoken(
            key_store->cl,
            block_id, block_id_length,
            user_id, user_id_length,
            owner_id, owner_id_length);
}

hermes_status_t hermes_key_store_set_wtoken(
        hermes_key_store_t *key_store,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *token, const size_t token_length,
        const uint8_t *owner_id, const size_t owner_id_length) {
    if (!key_store || !user_id || !user_id_length || !block_id || !block_id_length) {
        return HM_INVALID_PARAMETER;
    }
    if (token && token_length) {
        return hm_key_store_client_sync_call_set_wtoken(
                key_store->cl,
                block_id, block_id_length,
                user_id, user_id_length,
                owner_id, owner_id_length,
                token, token_length);
    }
    return hm_key_store_client_sync_call_del_wtoken(
            key_store->cl,
            block_id, block_id_length,
            user_id, user_id_length,
            owner_id, owner_id_length);
}

hermes_status_t hermes_key_store_destroy(hermes_key_store_t **key_store) {
    if (!key_store || !(*key_store)) {
        return HM_INVALID_PARAMETER;
    }
    hm_key_store_client_sync_destroy(&((*key_store)->cl));
    free(*key_store);
    *key_store = NULL;
    return HM_SUCCESS;
}

struct hermes_key_store_iterator_type {
    hermes_key_store_t *ks;
    uint8_t *block_id;
    size_t block_id_length;
    uint8_t *current_user_id;
    size_t current_user_id_length;
    uint32_t current_rights_mask;
    size_t current_index;
};

#define KEY_STORE_ITERATOR_NODE_SIZE 5 //each node  must contain 5 buffers: user id, read token, read token owner, write token, write token owner

hermes_key_store_iterator_t *hermes_key_store_iterator_create(
        hermes_key_store_t *key_store, const uint8_t *block_id, const size_t block_id_length) {
    if (!key_store || !block_id || !block_id_length) {
        return NULL;
    }
    hermes_key_store_iterator_t *iterator = calloc(1, sizeof(hermes_key_store_iterator_t));
    assert(iterator);
    iterator->block_id = malloc(block_id_length);
    assert(iterator->block_id);
    memcpy(iterator->block_id, block_id, block_id_length);
    iterator->block_id_length = block_id_length;
    iterator->ks = key_store;
    if (HM_SUCCESS != hm_key_store_client_sync_call_get_indexed_rights(
            key_store->cl, block_id, block_id_length, iterator->current_index,
            &(iterator->current_user_id), &(iterator->current_user_id_length), &(iterator->current_rights_mask))) {
        hermes_key_store_iterator_destroy(&iterator);
        return NULL;
    }
    return iterator;
}

hermes_status_t hermes_key_store_iterator_next(hermes_key_store_iterator_t *iterator) {
    if (!iterator) {
        return HM_INVALID_PARAMETER;
    }
    free(iterator->current_user_id);
    iterator->current_user_id=NULL;
    if (HM_SUCCESS != hm_key_store_client_sync_call_get_indexed_rights(
            iterator->ks->cl, iterator->block_id, iterator->block_id_length, ++(iterator->current_index),
            &(iterator->current_user_id), &(iterator->current_user_id_length), &(iterator->current_rights_mask))) {
        return HM_FAIL;
    }
    return HM_SUCCESS;
}

uint8_t *hermes_key_store_iterator_get_user_id(hermes_key_store_iterator_t *iterator) {
    if (!iterator) {
        return NULL;
    }
    return iterator->current_user_id;
}

size_t hermes_key_store_iterator_get_user_id_length(hermes_key_store_iterator_t *iterator) {
    if (!iterator) {
        return 0;
    }
    return iterator->current_user_id_length;
}

hermes_status_t hermes_key_store_iterator_destroy(hermes_key_store_iterator_t **iterator) {
    if (!iterator || !(*iterator)) {
        return HM_INVALID_PARAMETER;
    }
    free((*iterator)->block_id);
    free((*iterator)->current_user_id);
    free(*iterator);
    *iterator = NULL;
    return HM_SUCCESS;
}
