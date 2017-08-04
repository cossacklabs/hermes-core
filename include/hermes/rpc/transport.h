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



#ifndef HERMES_RPC_TRANSPORT_H
#define HERMES_RPC_TRANSPORT_H

#include <stdint.h>
#include <stdlib.h>

typedef uint32_t(*hm_rpc_transport_send_t)(void *transport, const uint8_t *buffer, const size_t buffer_length);

typedef uint32_t(*hm_rpc_transport_recv_t)(void *transport, uint8_t *buffer, size_t buffer_length);

typedef uint32_t(*hm_rpc_transport_get_remote_id_t)(void *transport, uint8_t **id, size_t *id_length);

typedef struct hm_rpc_transport_type {
    hm_rpc_transport_send_t send;
    hm_rpc_transport_recv_t recv;
    hm_rpc_transport_get_remote_id_t get_remote_id;
    void *user_data;
} hm_rpc_transport_t;


#endif //HERMES_RPC_TRANSPORT_H
