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




#include "credential_store_impl.h"

#include <hermes/credential_store/client.h>

struct hermes_credential_store_type {
    hm_credential_store_client_sync_t *cl;
};

hermes_credential_store_t *hermes_credential_store_create(hm_rpc_transport_t *transport) {
    if (!transport) {
        return NULL;
    }
    hermes_credential_store_t *credential_store = calloc(1, sizeof(hermes_credential_store_t));
    if (!credential_store
        || !(credential_store->cl = hm_credential_store_client_sync_create(transport))) {
        hermes_credential_store_destroy(&credential_store);
        return NULL;
    }
    return credential_store;
}

hermes_status_t hermes_credential_store_get_public_key(
        hermes_credential_store_t *credential_store,
        const uint8_t *user_id, const size_t user_id_length,
        uint8_t **public_key, size_t *public_key_length) {
    if (!credential_store || !user_id || !user_id_length || !public_key || !public_key_length) {
        return HM_INVALID_PARAMETER;
    }
    return hm_credential_store_client_sync_call_get_pub_key_by_id(
            credential_store->cl, user_id, user_id_length, public_key, public_key_length);
}

hermes_status_t hermes_credential_store_destroy(hermes_credential_store_t **credential_store) {
    if (!credential_store || !(*credential_store)) {
        return HM_INVALID_PARAMETER;
    }
    hm_credential_store_client_sync_destroy(&((*credential_store)->cl));
    free(*credential_store);
    *credential_store = NULL;
    return HM_SUCCESS;
}
