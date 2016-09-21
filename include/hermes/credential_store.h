/*
 * Copyright (c) 2016 Cossack Labs Limited
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

#ifndef MIDHERMES_CREDENTIAL_STORE_H_
#define MIDHERMES_CREDENTIAL_STORE_H_

#include "config.h"
#include "credential_store_protocol.h"

typedef credential_store_t credential_store_client_t;

credential_store_client_t* credential_store_client_create(const config_t* config);
int credential_store_client_get_pub_key(credential_store_client_t* ctx, const char* id, uint8_t** key, size_t* key_length);
int credential_store_client_get_users_list(credential_store_client_t* ctx, const char* id, char** res);
int credential_store_client_destroy(credential_store_client_t* ctx);

#endif /* MIDHERMES_CREDENTIAL_STORE_H_ */
