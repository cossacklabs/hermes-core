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



#ifndef MID_HERMES_LL_TOKEN_H
#define MID_HERMES_LL_TOKEN_H

#include <hermes/common/errors.h>

#include <hermes/mid_hermes_ll/mid_hermes_ll_buffer.h>
#include <hermes/mid_hermes_ll/mid_hermes_ll_user.h>
#include <hermes/mid_hermes_ll/interfaces/key_store.h>

typedef struct mid_hermes_ll_token_type mid_hermes_ll_token_t;

struct mid_hermes_ll_token_type {
    mid_hermes_ll_user_t *user;
    mid_hermes_ll_user_t *owner;
    mid_hermes_ll_buffer_t *token;

    mid_hermes_ll_token_t *(*init)(
            mid_hermes_ll_user_t *user, mid_hermes_ll_user_t *owner, mid_hermes_ll_buffer_t *token);

    mid_hermes_ll_token_t *(*generate)(mid_hermes_ll_user_t *user_and_owner);

    mid_hermes_ll_buffer_t *(*get_data)(mid_hermes_ll_token_t *token);

    mid_hermes_ll_token_t *(*get_token_for_user)(mid_hermes_ll_token_t *token, mid_hermes_ll_user_t *for_user);

    hermes_status_t (*cleanup)(mid_hermes_ll_token_t *token);

    mid_hermes_ll_token_t *(*load_c)(
            const uint8_t *user_id, const size_t user_id_length,
            const uint8_t *block_id, const size_t block_id_length,
            hermes_key_store_t *key_store, hermes_credential_store_t *credential_store,
            bool is_update);

    hermes_status_t (*save)(const mid_hermes_ll_token_t *token, hermes_key_store_t *key_store, bool is_update);
};

mid_hermes_ll_token_t *mid_hermes_ll_token_create(
        mid_hermes_ll_user_t *user, mid_hermes_ll_user_t *owner, mid_hermes_ll_buffer_t *token);

mid_hermes_ll_token_t *mid_hermes_ll_token_generate(mid_hermes_ll_user_t *user_and_owner);

mid_hermes_ll_buffer_t *mid_hermes_ll_token_get_data(mid_hermes_ll_token_t *token);

mid_hermes_ll_token_t *mid_hermes_ll_token_get_token_for_user(
        mid_hermes_ll_token_t *token, mid_hermes_ll_user_t *for_user);

hermes_status_t mid_hermes_ll_token_destroy(mid_hermes_ll_token_t **token);

//store dependent
mid_hermes_ll_token_t *mid_hermes_ll_rtoken_load_c(
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        hermes_key_store_t *key_store, hermes_credential_store_t *credential_store);

mid_hermes_ll_token_t *mid_hermes_ll_wtoken_load_c(
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        hermes_key_store_t *key_store, hermes_credential_store_t *credential_store);

hermes_status_t mid_hermes_ll_token_save(
        const mid_hermes_ll_user_t *user,
        const mid_hermes_ll_buffer_t *block_id,
        const mid_hermes_ll_token_t *token,
        hermes_key_store_t *key_store,
        bool is_update);

hermes_status_t mid_hermes_ll_token_del(
        const mid_hermes_ll_user_t *user,
        const mid_hermes_ll_user_t *owner,
        const mid_hermes_ll_buffer_t *block_id,
        hermes_key_store_t *key_store,
        bool is_update);

#endif //MID_HERMES_LL_TOKEN_H
