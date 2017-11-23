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



#ifndef HERMES_KEY_STORE_H
#define HERMES_KEY_STORE_H

#include <hermes/common/errors.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct hermes_key_store_type hermes_key_store_t;

hermes_status_t hermes_key_store_get_rtoken(
        hermes_key_store_t *key_store,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        uint8_t **token, size_t *token_length,
        uint8_t **owner_id, size_t *owner_id_length);

hermes_status_t hermes_key_store_get_wtoken(
        hermes_key_store_t *key_store,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        uint8_t **token, size_t *token_length,
        uint8_t **owner_id, size_t *owner_id_length);

hermes_status_t hermes_key_store_set_rtoken(
        hermes_key_store_t *key_store,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *token, const size_t token_length,
        const uint8_t *owner_id, const size_t owner_id_length);

hermes_status_t hermes_key_store_set_wtoken(
        hermes_key_store_t *key_store,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *token, const size_t token_length,
        const uint8_t *owner_id, const size_t owner_id_length);

hermes_status_t hermes_key_store_destroy(hermes_key_store_t **key_store);

typedef struct hermes_key_store_iterator_type hermes_key_store_iterator_t;

hermes_key_store_iterator_t *hermes_key_store_iterator_create(
        hermes_key_store_t *key_store, const uint8_t *block_id, const size_t block_id_length);

hermes_status_t hermes_key_store_iterator_next(hermes_key_store_iterator_t *iterator);

uint8_t *hermes_key_store_iterator_get_user_id(hermes_key_store_iterator_t *iterator);

size_t hermes_key_store_iterator_get_user_id_length(hermes_key_store_iterator_t *iterator);

hermes_status_t hermes_key_store_iterator_destroy(hermes_key_store_iterator_t **iterator);


#endif //HERMES_KEY_STORE_H
