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


#ifndef HERMES_CREDENTIAL_STORE_CLIENT_H
#define HERMES_CREDENTIAL_STORE_CLIENT_H

#include <hermes/rpc/transport.h>

#include <stdint.h>
#include <stdlib.h>

typedef struct hm_credential_store_client_sync_type hm_credential_store_client_sync_t;

hm_credential_store_client_sync_t* hm_credential_store_client_sync_create(hm_rpc_transport_t* transport);
uint32_t hm_credential_store_client_sync_destroy(hm_credential_store_client_sync_t** c);
uint32_t hm_crerential_store_client_sync_call_get_pub_key_by_id(hm_credential_store_client_sync_t* c, const uint8_t* id, const size_t id_length, uint8_t** key, size_t* key_length);
#endif //HERMES_CREDENTIAL_STORE_CLIENT_H
