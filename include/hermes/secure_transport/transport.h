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

#ifndef HERMES_CORE_TRANSPORT_H
#define HERMES_CORE_TRANSPORT_H

#include <hermes/rpc/transport.h>
#include <themis/secure_session.h>

typedef struct secure_transport_type secure_transport_t;

uint32_t destroy_secure_transport(secure_transport_t** transport_);
uint32_t destroy_rpc_secure_transport(hm_rpc_transport_t** transport_);
hm_rpc_transport_t* create_secure_transport(
        const uint8_t *user_id, size_t user_id_length,
        const uint8_t *private_key, size_t private_key_length,
        const uint8_t *public_key, size_t public_key_length,
        const uint8_t *public_key_id, size_t public_key_id_length,
        hm_rpc_transport_t* user_transport,
        bool is_server);

hm_rpc_transport_t* create_secure_transport_with_callback(
        const uint8_t *user_id, size_t user_id_length,
        const uint8_t *private_key, size_t private_key_length,
        secure_session_user_callbacks_t* callback,
        hm_rpc_transport_t* user_transport,
        bool is_server);
#endif //HERMES_CORE_TRANSPORT_H
