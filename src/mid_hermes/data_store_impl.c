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

#include "data_store_impl.h"

#include <hermes/data_store/client.h>

struct hermes_data_store_type {
    hm_data_store_client_sync_t *cl;
};

hermes_data_store_t *hermes_data_store_create(hm_rpc_transport_t *transport) {
    if (!transport) {
        return NULL;
    }
    hermes_data_store_t *data_store = calloc(1, sizeof(hermes_data_store_t));
    if (!data_store
        || !(data_store->cl = hm_data_store_client_sync_create(transport))) {
        hermes_data_store_destroy(&data_store);
        return NULL;
    }
    return data_store;
}

hermes_status_t hermes_data_store_set_block(
        hermes_data_store_t *data_store,
        uint8_t **id, size_t *id_length,
        const uint8_t *data, const size_t data_length,
        const uint8_t *meta, const size_t meta_length,
        const uint8_t *mac, const size_t mac_length,
        const uint8_t *old_mac, const size_t old_mac_length) {
    if (!data_store || !data || !data_length || !meta || !meta_length || !mac || !mac_length || !id || !id_length) {
        return HM_INVALID_PARAMETER;
    }
    if (!old_mac) {
        if (!(*id)) {
            return hm_data_store_client_sync_call_create_block(
                    data_store->cl,
                    data, data_length,
                    meta, meta_length,
                    mac, mac_length,
                    id, id_length);
        }
        return hm_data_store_client_sync_call_create_block_with_id(
                data_store->cl,
                *id, *id_length,
                data, data_length,
                meta, meta_length,
                mac, mac_length);
    }
    return hm_data_store_client_sync_call_update_block(
            data_store->cl,
            *id, *id_length,
            data, data_length,
            meta, meta_length,
            mac, mac_length,
            old_mac, old_mac_length);
}

hermes_status_t hermes_data_store_get_block(
        hermes_data_store_t *data_store,
        const uint8_t *id, const size_t id_length,
        uint8_t **data, size_t *data_length,
        uint8_t **meta, size_t *meta_length) {
    if (!data_store || !id || !id_length || !data || !data_length || !meta || !meta_length) {
        return HM_INVALID_PARAMETER;
    }
    return hm_data_store_client_sync_call_read_block(
            data_store->cl,
            id, id_length,
            data, data_length,
            meta, meta_length);
}

hermes_status_t hermes_data_store_rem_block(
        hermes_data_store_t *data_store,
        const uint8_t *id, const size_t id_length,
        const uint8_t *old_mac, const size_t old_mac_length) {
    if (!data_store || !id || !id_length || !old_mac || !old_mac_length) {
        return HM_INVALID_PARAMETER;
    }
    return hm_data_store_client_sync_call_delete_block(data_store->cl, id, id_length, old_mac, old_mac_length);
}

hermes_status_t hermes_data_store_destroy(hermes_data_store_t **data_store) {
    if (!data_store || !(*data_store)) {
        return HM_INVALID_PARAMETER;
    }
    hm_data_store_client_sync_destroy(&((*data_store)->cl));
    free(*data_store);
    *data_store = NULL;
    return HM_SUCCESS;
}
