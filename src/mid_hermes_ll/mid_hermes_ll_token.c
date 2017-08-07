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
#include <hermes/mid_hermes_ll/mid_hermes_ll_token.h>
#include "utils.h"

#include <themis/themis.h>

#include <assert.h>

mid_hermes_ll_token_t *mid_hermes_ll_token_create(
        mid_hermes_ll_user_t *user, mid_hermes_ll_user_t *owner, mid_hermes_ll_buffer_t *token) {
    HERMES_CHECK_IN_PARAM_RET_NULL(user);
    HERMES_CHECK_IN_PARAM_RET_NULL(token);
    mid_hermes_ll_token_t *t = calloc(1, sizeof(mid_hermes_ll_token_t));
    assert(t);
    t->user = user;
    t->owner = owner;
    t->token = token;
    return t;
}

mid_hermes_ll_token_t *mid_hermes_ll_token_generate(mid_hermes_ll_user_t *user_and_owner) {
    HERMES_CHECK_IN_PARAM_RET_NULL(user_and_owner);
    uint8_t token[HM_TOKEN_LEN];
    if (SOTER_SUCCESS != soter_rand(token, HM_TOKEN_LEN)) {
        return NULL;
    }
    uint32_t res;
    mid_hermes_ll_buffer_t *encryption_token = mid_hermes_ll_buffer_create(NULL, 0);
    res = hm_asym_encrypt(
            user_and_owner->sk->data, user_and_owner->sk->length, user_and_owner->pk->data, user_and_owner->pk->length,
            token, sizeof(token), &(encryption_token->data), &(encryption_token->length));
    if (HM_SUCCESS != res) {
        return NULL;
    }
    return mid_hermes_ll_token_create(user_and_owner, user_and_owner, encryption_token);
}

mid_hermes_ll_buffer_t *mid_hermes_ll_token_get_data(mid_hermes_ll_token_t *token) {
    HERMES_CHECK_IN_PARAM_RET_NULL(token);
    mid_hermes_ll_buffer_t *buffer = mid_hermes_ll_buffer_create(NULL, 0);
    if (HM_SUCCESS != hm_asym_decrypt(
            token->user->sk->data, token->user->sk->length, token->owner->pk->data, token->owner->pk->length,
            token->token->data, token->token->length, &(buffer->data), &(buffer->length))) {
        mid_hermes_ll_buffer_destroy(&buffer);
        return NULL;
    }
    return buffer;
}

mid_hermes_ll_token_t *mid_hermes_ll_token_get_token_for_user(
        mid_hermes_ll_token_t *token, mid_hermes_ll_user_t *for_user) {
    HERMES_CHECK_IN_PARAM_RET_NULL(token);
    HERMES_CHECK_IN_PARAM_RET_NULL(for_user);
    mid_hermes_ll_buffer_t *buffer = mid_hermes_ll_token_get_data(token);
    if (!buffer) {
        return NULL;
    }
    mid_hermes_ll_buffer_t *encryption_buffer = mid_hermes_ll_buffer_create(NULL, 0);
    hermes_status_t res = hm_asym_encrypt(
            token->user->sk->data, token->user->sk->length, for_user->pk->data, for_user->pk->length,
            buffer->data, buffer->length, &(encryption_buffer->data), &(encryption_buffer->length));
    if (HM_SUCCESS != res) {
        mid_hermes_ll_buffer_destroy(&buffer);
        mid_hermes_ll_buffer_destroy(&encryption_buffer);
        return NULL;
    }
    mid_hermes_ll_buffer_destroy(&buffer);
    return mid_hermes_ll_token_create(for_user, mid_hermes_ll_user_copy(token->user), encryption_buffer);
}

hermes_status_t mid_hermes_ll_token_destroy(mid_hermes_ll_token_t **token) {
    HERMES_CHECK_IN_PARAM(token);
    HERMES_CHECK_IN_PARAM(*token);
    mid_hermes_ll_buffer_destroy(&((*token)->token));
    if (((*token)->user) != ((*token)->owner)) {
        mid_hermes_ll_user_destroy(&((*token)->owner));
    }
    mid_hermes_ll_user_destroy(&((*token)->user));
    free(*token);
    *token = NULL;
    return HM_SUCCESS;
}

//store dependent
mid_hermes_ll_token_t *mid_hermes_ll_token_load_c(
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        hermes_key_store_t *key_store,
        hermes_credential_store_t *credential_store,
        bool is_update) {
    if (!user_id || !user_id_length || !block_id || !block_id_length || !key_store || !credential_store) {
        return NULL;
    }
    mid_hermes_ll_buffer_t *ownerid = mid_hermes_ll_buffer_create(NULL, 0);
    mid_hermes_ll_buffer_t *token = mid_hermes_ll_buffer_create(NULL, 0);
    mid_hermes_ll_user_t *user = NULL, *owner = NULL;
    if (!ownerid
        || !token
        || (0 != (is_update ? hermes_key_store_get_wtoken : hermes_key_store_get_rtoken)(
            key_store,
            user_id, user_id_length,
            block_id, block_id_length,
            &(token->data), &(token->length),
            &(ownerid->data), &(ownerid->length)))
        || !(user = mid_hermes_ll_user_load_c(user_id, user_id_length, credential_store))
        || !(owner = mid_hermes_ll_user_load(ownerid, credential_store))) {
        mid_hermes_ll_user_destroy(&user);
        mid_hermes_ll_buffer_destroy(&ownerid);
        mid_hermes_ll_buffer_destroy(&token);
        return NULL;
    }
    mid_hermes_ll_token_t *token_ = mid_hermes_ll_token_create(user, owner, token);
    if (!token) {
        mid_hermes_ll_user_destroy(&user);
        mid_hermes_ll_user_destroy(&owner);
        mid_hermes_ll_buffer_destroy(&token);
        return NULL;
    }
    return token_;
}

mid_hermes_ll_token_t *mid_hermes_ll_rtoken_load_c(
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        hermes_key_store_t *key_store,
        hermes_credential_store_t *credential_store) {
    return mid_hermes_ll_token_load_c(
            user_id, user_id_length, block_id, block_id_length, key_store, credential_store, false);
}

mid_hermes_ll_token_t *mid_hermes_ll_wtoken_load_c(
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        hermes_key_store_t *key_store,
        hermes_credential_store_t *credential_store) {
    return mid_hermes_ll_token_load_c(
            user_id, user_id_length, block_id, block_id_length, key_store, credential_store, true);
}

hermes_status_t mid_hermes_ll_token_save(
        const mid_hermes_ll_user_t *user, const mid_hermes_ll_buffer_t *block_id,
        const mid_hermes_ll_token_t *token, hermes_key_store_t *key_store, bool is_update) {
    if (!user || !block_id || !token || !key_store) {
        return HM_FAIL;
    }
    return (is_update ? hermes_key_store_set_wtoken : hermes_key_store_set_rtoken)(
            key_store, token->user->id->data, token->user->id->length, block_id->data, block_id->length,
            token->token->data, token->token->length, token->owner->id->data, token->owner->id->length);
}

hermes_status_t mid_hermes_ll_token_del(
        const mid_hermes_ll_user_t *user, const mid_hermes_ll_buffer_t *bl_id,
        hermes_key_store_t *key_store, bool is_update) {
    if (!user || !bl_id || !key_store) {
        return HM_FAIL;
    }
    hermes_status_t res = hermes_key_store_set_wtoken(
            key_store, user->id->data, user->id->length, bl_id->data, bl_id->length, NULL, 0,
            user->id->data, user->id->length);
    if (!is_update) {
        return hermes_key_store_set_rtoken(
                key_store, user->id->data, user->id->length, bl_id->data, bl_id->length, NULL, 0,
                user->id->data, user->id->length);
    }
    return res;
}
