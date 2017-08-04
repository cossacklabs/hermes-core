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



#include <hermes/credential_store/service.h>
#include <hermes/credential_store/server.h>
#include <hermes/common/errors.h>
#include <assert.h>
#include <stdbool.h>

struct hm_credential_server_service_type {
    hm_credential_store_server_t *s;
    bool finish;
};

hm_credential_store_service_t *hm_credential_store_service_create(hm_rpc_transport_t *transport, hm_cs_db_t *db) {
    if (!transport || !db) {
        return NULL;
    }
    hm_credential_store_service_t *service = calloc(sizeof(hm_credential_store_service_t), 1);
    assert(service);
    if (!(service->s = hm_credential_store_server_create(transport, db))) {
        free(service);
        return NULL;
    }
    return service;
}

uint32_t hm_credential_store_service_destroy(hm_credential_store_service_t **service) {
    if (!service || !(*service)) {
        return HM_INVALID_PARAMETER;
    }
    hm_credential_store_server_destroy(&((*service)->s));
    free(*service);
    *service = NULL;
    return HM_SUCCESS;
}

uint32_t hm_credential_store_service_start(hm_credential_store_service_t *service) {
    if (!service) {
        return HM_INVALID_PARAMETER;
    }
    while (true) {
        if (service->finish) {
            return HM_SUCCESS;
        }
        if (HM_SUCCESS != hm_credential_store_server_call(service->s)) {
            return HM_FAIL;
        }
    }
    return HM_FAIL;
}

uint32_t hm_credential_store_service_stop(hm_credential_store_service_t *service) {
    if (!service) {
        return HM_INVALID_PARAMETER;
    }
    service->finish = true;
    return HM_SUCCESS;
}
