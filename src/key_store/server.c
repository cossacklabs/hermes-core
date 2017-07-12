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

#include <hermes/key_store/server.h>

#include <hermes/common/errors.h>
#include <hermes/rpc/server.h>

#include <assert.h>

#include "functions.h"

struct hm_key_store_server_type {
    hm_rpc_server_t *s;
    hm_ks_db_t *db;
};

hm_key_store_server_t *hm_key_store_server_create(hm_rpc_transport_t *transport, hm_ks_db_t *db) {
    if (!transport || !db) {
        return NULL;
    }
    hm_key_store_server_t *server = calloc(sizeof(hm_key_store_server_t), 1);
    assert(server);
    server->s = hm_rpc_server_create(transport);
    if (!(server->s)) {
        hm_key_store_server_destroy(&server);
        return NULL;
    }
    uint32_t res = HM_RPC_SERVER_REG_FUNC(server->s, hm_key_store_set_rtoken);
    if (HM_SUCCESS != res) {
        hm_key_store_server_destroy(&server);
        return NULL;
    }
    if (HM_SUCCESS != (res = HM_RPC_SERVER_REG_FUNC(server->s, hm_key_store_set_wtoken))) {
        hm_key_store_server_destroy(&server);
        return NULL;
    }
    if (HM_SUCCESS != (res = HM_RPC_SERVER_REG_FUNC(server->s, hm_key_store_get_rtoken))) {
        hm_key_store_server_destroy(&server);
        return NULL;
    }
    if (HM_SUCCESS != (res = HM_RPC_SERVER_REG_FUNC(server->s, hm_key_store_get_wtoken))) {
        hm_key_store_server_destroy(&server);
        return NULL;
    }
    if (HM_SUCCESS != (res = HM_RPC_SERVER_REG_FUNC(server->s, hm_key_store_del_rtoken))) {
        hm_key_store_server_destroy(&server);
        return NULL;
    }
    if (HM_SUCCESS != (res = HM_RPC_SERVER_REG_FUNC(server->s, hm_key_store_del_wtoken))) {
        hm_key_store_server_destroy(&server);
        return NULL;
    }
    if (HM_SUCCESS != (res = HM_RPC_SERVER_REG_FUNC(server->s, hm_key_store_get_indexed_rights))) {
        hm_key_store_server_destroy(&server);
        return NULL;
    }
    server->db = db;
    return server;
}

uint32_t hm_key_store_server_destroy(hm_key_store_server_t **server) {
    if (!server || !(*server)) {
        return HM_INVALID_PARAMETER;
    }
    hm_rpc_server_destroy(&((*server)->s));
    free(*server);
    *server = NULL;
    return HM_SUCCESS;
}

uint32_t hm_key_store_server_call(hm_key_store_server_t *server) {
    if (!server) {
        return HM_INVALID_PARAMETER;
    }
    return hm_rpc_server_call(server->s, (void *) (server->db));
}

