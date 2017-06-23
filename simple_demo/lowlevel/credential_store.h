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


#ifndef CREDENTIAL_STORE_H
#define CREDENTIAL_STORE_H

#include <stdint.h>
#include <stdlib.h>


typedef struct credential_store_type credential_store_t;

credential_store_t* credential_store_create();

uint32_t credential_store_get_public_key(credential_store_t* cs, const uint8_t* user_id, const size_t user_id_length, uint8_t** pub_key, size_t* pub_key_length);

uint32_t credential_store_destroy(credential_store_t** cs);

#endif //CREDENTIAL_STORE_H

