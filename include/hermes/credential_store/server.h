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



#ifndef HERMES_CREDENTIAL_STORE_SERVER_H
#define HERMES_CREDENTIAL_STORE_SERVER_H

#include <hermes/rpc/transport.h>
#include <hermes/credential_store/db.h>

#include <stdint.h>
#include <stdlib.h>

typedef struct hm_credential_store_server_type hm_credential_store_server_t;

hm_credential_store_server_t *hm_credential_store_server_create(hm_rpc_transport_t *transport, hm_cs_db_t *db);

uint32_t hm_credential_store_server_destroy(hm_credential_store_server_t **server);

uint32_t hm_credential_store_server_call(hm_credential_store_server_t *server);

#endif //HERMES_CREDENTIAL_STORE_SERVER_H
