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


#ifndef HERMES_RPC_SYNC_CLIENT_H
#define HERMES_RPC_SYNC_CLIENT_H

#include <hermes/rpc/param_pack.h>

typedef hm_rpc_client_type hm_rpc_client_t;

hm_rpc_client_t* hm_rpc_sync_create();
uint32_t hm_rpc_client_destroy(hm_rpc_client** c);

uint32_t hm_rpc_client_sync_call(hm_rpc_sync_client_t* c, const uint8_t* func_signature, const size_t func_signature_length, hm_param_pack_t* in_params, hm_param_pack_t** out_params);
uint32_t hm_rpc_client_async_call(hm_rpc_sync_client_t* c, const uint8_t* func_signature, const size_t func_signature_length, hm_param_pack_t* in_params, hm_param_pack_t** out_params);

#endif //HERMES_RPC_SYNC_CLIENT_H
