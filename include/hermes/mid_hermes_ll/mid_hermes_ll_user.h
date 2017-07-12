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


#ifndef MID_HERMES_LL_USER_H
#define MID_HERMES_LL_USER_H

#include <hermes/common/errors.h>
#include <hermes/mid_hermes_ll/mid_hermes_ll_buffer.h>
#include <hermes/mid_hermes_ll/interfaces/credential_store.h>

#include <stdbool.h>

typedef struct mid_hermes_ll_user_type {
    mid_hermes_ll_buffer_t *id;
    mid_hermes_ll_buffer_t *sk;
    mid_hermes_ll_buffer_t *pk;
    uint32_t count;
} mid_hermes_ll_user_t;

mid_hermes_ll_user_t *mid_hermes_ll_local_user_create(
        mid_hermes_ll_buffer_t *id, mid_hermes_ll_buffer_t *private_key, mid_hermes_ll_buffer_t *public_key);

mid_hermes_ll_user_t *mid_hermes_ll_user_create(mid_hermes_ll_buffer_t *id, mid_hermes_ll_buffer_t *public_key);

mid_hermes_ll_user_t *mid_hermes_ll_user_create_c(
        const uint8_t *id, const size_t id_length,
        const uint8_t *public_key, const size_t public_key_length);

mid_hermes_ll_user_t *mid_hermes_ll_local_user_create_c(
        const uint8_t *id, const size_t id_length,
        const uint8_t *private_key, const size_t private_key_length,
        const uint8_t *public_key, const size_t public_key_length);

mid_hermes_ll_user_t *mid_hermes_ll_user_copy(mid_hermes_ll_user_t *user);

bool mid_hermes_ll_user_is_equal(const mid_hermes_ll_user_t *user1, const mid_hermes_ll_user_t *user2);

hermes_status_t mid_hermes_ll_user_forse_destroy(mid_hermes_ll_user_t **user);

hermes_status_t mid_hermes_ll_user_destroy(mid_hermes_ll_user_t **user);

//store dependent
mid_hermes_ll_user_t *mid_hermes_ll_user_load(mid_hermes_ll_buffer_t *id, hermes_credential_store_t *credential_store);

mid_hermes_ll_user_t *mid_hermes_ll_user_load_c(
        const uint8_t *id, const size_t id_length,
        hermes_credential_store_t *credential_store);

mid_hermes_ll_user_t *mid_hermes_ll_local_user_load_c(
        const uint8_t *id, const size_t id_length,
        const uint8_t *private_key, const size_t private_key_length,
        hermes_credential_store_t *credential_store);

#endif //MID_HERMES_LL_USER_H
