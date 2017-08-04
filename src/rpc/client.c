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




#include <hermes/rpc/client.h>
#include <hermes/common/errors.h>

#include <assert.h>

struct hm_rpc_client_sync_type {
    hm_rpc_transport_t *transport;
};

hm_rpc_client_sync_t *hm_rpc_client_sync_create(hm_rpc_transport_t *transport) {
    if (!transport) {
        return NULL;
    }
    hm_rpc_client_sync_t *client = calloc(sizeof(hm_rpc_client_sync_t), 1);
    assert(client);
    client->transport = transport;
    return client;
}

uint32_t hm_rpc_client_sync_destroy(hm_rpc_client_sync_t **client) {
    if (!client || !(*client)) {
        return HM_INVALID_PARAMETER;
    }
    free(*client);
    *client = NULL;
    return HM_SUCCESS;
}

uint32_t hm_rpc_client_sync_call(
        hm_rpc_client_sync_t *client,
        const uint8_t *func_name, const size_t func_name_length,
        hm_param_pack_t *in_params,
        uint32_t *error,
        hm_param_pack_t **out_params) {
    if (!client || !func_name || !func_name_length || !in_params) {
        return HM_INVALID_PARAMETER;
    }
    uint32_t res = HM_SUCCESS;
    if (HM_SUCCESS != (res = client->transport->send(
            client->transport->user_data, (const uint8_t *) (&func_name_length), sizeof(uint32_t)))) {
        return res;
    }
    if (HM_SUCCESS != (res = client->transport->send(client->transport->user_data, func_name, func_name_length))) {
        return res;
    }
    if (HM_SUCCESS != (res = hm_param_pack_send(in_params, client->transport))) {
        return res;
    }
    if (HM_SUCCESS !=
        (res = client->transport->recv(client->transport->user_data, (uint8_t *) error, sizeof(uint32_t)))) {
        return res;
    }
    if (HM_SUCCESS == (*error) && out_params) {
        (*out_params) = hm_param_pack_receive(client->transport);
        if (!*out_params) {
            return HM_FAIL;
        }
    }
    return HM_SUCCESS;
}
