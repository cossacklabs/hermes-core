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



#include <hermes/common/errors.h>
#include <hermes/mid_hermes_ll/mid_hermes_ll_user.h>

#include "utils.h"
#include <assert.h>

mid_hermes_ll_user_t *mid_hermes_ll_local_user_create(
        mid_hermes_ll_buffer_t *id, mid_hermes_ll_buffer_t *private_key,
        mid_hermes_ll_buffer_t *public_key) {
    HERMES_CHECK_IN_PARAM_RET_NULL(id);
    HERMES_CHECK_IN_PARAM_RET_NULL(!mid_hermes_ll_buffer_is_empty(id));
    HERMES_CHECK_IN_PARAM_RET_NULL(private_key);
    HERMES_CHECK_IN_PARAM_RET_NULL(!mid_hermes_ll_buffer_is_empty(private_key));
    HERMES_CHECK_IN_PARAM_RET_NULL(public_key);
    HERMES_CHECK_IN_PARAM_RET_NULL(!mid_hermes_ll_buffer_is_empty(public_key));
    mid_hermes_ll_user_t *u = mid_hermes_ll_user_create(id, public_key);
    u->sk = private_key;
    return u;
}

mid_hermes_ll_user_t *mid_hermes_ll_user_create(mid_hermes_ll_buffer_t *id, mid_hermes_ll_buffer_t *public_key) {
    HERMES_CHECK_IN_PARAM_RET_NULL(id);
    HERMES_CHECK_IN_PARAM_RET_NULL(!mid_hermes_ll_buffer_is_empty(id));
    HERMES_CHECK_IN_PARAM_RET_NULL(public_key);
    HERMES_CHECK_IN_PARAM_RET_NULL(!mid_hermes_ll_buffer_is_empty(public_key));
    mid_hermes_ll_user_t *u = calloc(1, sizeof(mid_hermes_ll_user_t));
    assert(u);
    u->id = id;
    u->pk = public_key;
    return u;
}

hermes_status_t mid_hermes_ll_user_destroy(mid_hermes_ll_user_t **user) {
    HERMES_CHECK_IN_PARAM(user);
    HERMES_CHECK_IN_PARAM(*user);
    mid_hermes_ll_buffer_destroy(&((*user)->id));
    mid_hermes_ll_buffer_destroy(&((*user)->sk));
    mid_hermes_ll_buffer_destroy(&((*user)->pk));
    free(*user);
    *user = NULL;
    return HM_SUCCESS;
}

mid_hermes_ll_user_t *mid_hermes_ll_user_load(mid_hermes_ll_buffer_t *id, hermes_credential_store_t *credential_store) {
    if (!id || !credential_store) {
        return NULL;
    }
    mid_hermes_ll_user_t *user = NULL;
    uint8_t *public_key = NULL;
    size_t public_key_length = 0;
    mid_hermes_ll_buffer_t *user_public_key = NULL;//mid_hermes_ll_buffer_create(NULL, 0);
    if ((HM_SUCCESS != hermes_credential_store_get_public_key(
            credential_store, id->data, id->length, &public_key, &public_key_length))
        || !(user_public_key = mid_hermes_ll_buffer_create(public_key, public_key_length))
        || !(user = mid_hermes_ll_user_create(id, user_public_key))) {
        mid_hermes_ll_buffer_destroy(&user_public_key);
        return NULL;
    }
    return user;
}

mid_hermes_ll_user_t *mid_hermes_ll_user_load_c(
        const uint8_t *id, const size_t id_length, hermes_credential_store_t *credential_store) {
    if (!id || !id_length || !credential_store) {
        return NULL;
    }
    mid_hermes_ll_buffer_t *id_buffer = mid_hermes_ll_buffer_create(id, id_length);
    mid_hermes_ll_user_t *block_id = NULL;
    if (!id_buffer
        || !(block_id = mid_hermes_ll_user_load(id_buffer, credential_store))) {
        mid_hermes_ll_buffer_destroy(&id_buffer);
    }
    return block_id;
}

mid_hermes_ll_user_t *mid_hermes_ll_local_user_load_c(
        const uint8_t *id, const size_t id_length, const uint8_t *private_key, const size_t private_key_length,
        hermes_credential_store_t *credential_store) {
    if (!id || !id_length || !private_key || !private_key_length || !credential_store) {
        return NULL;
    }
    mid_hermes_ll_buffer_t *id_buffer = mid_hermes_ll_buffer_create(id, id_length);
    if(!id_buffer){
        return NULL;
    }
    mid_hermes_ll_buffer_t *private_key_buffer = mid_hermes_ll_buffer_create(private_key, private_key_length);
    if(!private_key_buffer){
        mid_hermes_ll_buffer_destroy(&id_buffer);
        return NULL;
    }
    mid_hermes_ll_user_t *user = mid_hermes_ll_user_load(id_buffer, credential_store);;
    if (!user){
        mid_hermes_ll_buffer_destroy(&id_buffer);
        mid_hermes_ll_buffer_destroy(&private_key_buffer);
        return NULL;
    }
    user->sk = private_key_buffer;
    return user;
}


bool mid_hermes_ll_user_is_equal(const mid_hermes_ll_user_t *user1, const mid_hermes_ll_user_t *user2) {
    assert(user1);
    assert(user2);
    return mid_hermes_ll_buffer_is_equal(user1->id, user2->id);
}

mid_hermes_ll_user_t *mid_hermes_ll_user_create_c(
        const uint8_t *id, const size_t id_length, const uint8_t *public_key, const size_t public_key_length) {
    if (!id || !id_length || !public_key || !public_key_length) {
        return NULL;
    }
    mid_hermes_ll_buffer_t *idbuf = mid_hermes_ll_buffer_create(id, id_length);
    mid_hermes_ll_buffer_t *pkbuf = mid_hermes_ll_buffer_create(public_key, public_key_length);
    mid_hermes_ll_user_t *user = NULL;
    if (!idbuf
        || !pkbuf
        || !(user = mid_hermes_ll_user_create(idbuf, pkbuf))) {
        mid_hermes_ll_buffer_destroy(&idbuf);
        mid_hermes_ll_buffer_destroy(&pkbuf);
        return NULL;
    }
    return user;
}

mid_hermes_ll_user_t *mid_hermes_ll_local_user_create_c(
        const uint8_t *id, const size_t id_length,
        const uint8_t *private_key, const size_t private_key_length,
        const uint8_t *public_key, const size_t public_key_length) {
    if (!id || !id_length || !private_key || !private_key_length || !public_key || !public_key_length) {
        return NULL;
    }
    mid_hermes_ll_buffer_t *id_buffer = mid_hermes_ll_buffer_create(id, id_length);
    mid_hermes_ll_buffer_t *public_key_buffer = mid_hermes_ll_buffer_create(public_key, public_key_length);
    mid_hermes_ll_buffer_t *private_key_buffer = mid_hermes_ll_buffer_create(private_key, private_key_length);
    mid_hermes_ll_user_t *user = NULL;
    if (!id_buffer
        || !private_key_buffer
        || !public_key_buffer
        || !(user = mid_hermes_ll_local_user_create(id_buffer, private_key_buffer, public_key_buffer))) {
        mid_hermes_ll_buffer_destroy(&id_buffer);
        mid_hermes_ll_buffer_destroy(&private_key_buffer);
        mid_hermes_ll_buffer_destroy(&public_key_buffer);
        return NULL;
    }
    return user;
}

mid_hermes_ll_user_t *mid_hermes_ll_user_copy(mid_hermes_ll_user_t *user) {
    if (!user) {
        return NULL;
    }
    mid_hermes_ll_user_t *user_copy = mid_hermes_ll_user_create_c(user->id->data, user->id->length, user->pk->data,
                                                                  user->pk->length);
    if (user_copy && user->sk) {
        user_copy->sk = mid_hermes_ll_buffer_create(user->sk->data, user->sk->length);
        if (!(user_copy->sk)) {
            mid_hermes_ll_user_destroy(&user_copy);
            return NULL;
        }
    }
    return user_copy;
}
