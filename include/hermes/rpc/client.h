/*
 * Copyright (client) 2017 Cossack Labs Limited
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


#ifndef HERMES_RPC_SYNC_CLIENT_H
#define HERMES_RPC_SYNC_CLIENT_H

#include <hermes/rpc/param_pack.h>
#include <hermes/rpc/transport.h>

typedef struct hm_rpc_client_sync_type hm_rpc_client_sync_t;

hm_rpc_client_sync_t* hm_rpc_client_sync_create(hm_rpc_transport_t* transport);
uint32_t hm_rpc_client_sync_destroy(hm_rpc_client_sync_t** client);

uint32_t hm_rpc_client_sync_call(
        hm_rpc_client_sync_t* client,
        const uint8_t* func_name, const size_t func_name_length,
        hm_param_pack_t* in_params,
        uint32_t *error,
        hm_param_pack_t** out_params);

#endif //HERMES_RPC_SYNC_CLIENT_H
