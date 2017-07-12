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


#ifndef HERMES_RPC_SERVER_H
#define HERMES_RPC_SERVER_H

#include <hermes/rpc/param_pack.h>
#include <hermes/rpc/transport.h>

typedef struct hm_rpc_server_type hm_rpc_server_t;

typedef uint32_t(*hm_server_func_t)(hm_param_pack_t *in_params, hm_param_pack_t **out_params, void *user_data);


hm_rpc_server_t *hm_rpc_server_create(hm_rpc_transport_t *transport);

uint32_t hm_rpc_server_destroy(hm_rpc_server_t **server);

uint32_t hm_rpc_server_reg_func(
        hm_rpc_server_t *server,
        const uint8_t *func_signature, const size_t func_signature_length,
        hm_server_func_t func);

uint32_t hm_rpc_server_call_func(
        hm_rpc_server_t *server,
        const uint8_t *func_signature, const size_t func_signature_length,
        void *user_data);

uint32_t hm_rpc_server_call(hm_rpc_server_t *server, void *user_data);

#define HM_RPC_SERVER_REG_FUNC(s, func) hm_rpc_server_reg_func(s, (const uint8_t*)func##_NAME, sizeof(func##_NAME), func##_stub)

//TODO: add rouitine for registration of many functions in one call like HM_RPC_SERVER_REG_FUNCS(s, ...)

#endif //HERMES_RPC_SERVER_H
