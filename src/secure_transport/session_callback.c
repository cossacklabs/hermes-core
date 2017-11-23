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

#include <string.h>
#include "../../include/hermes/secure_transport/session_callback.h"
#include <hermes/credential_store/db.h>

int get_public_key_for_id_from_remote_credential_store_callback(
        const void *id, size_t id_length, void *key_buffer, size_t key_buffer_length, void *user_data){
    hm_credential_store_client_sync_t* db = (hm_credential_store_client_sync_t*)user_data;
    size_t temp_buffer_size;
    uint8_t *temp_buffer;
    if(hm_credential_store_client_sync_call_get_pub_key_by_id(db, id, id_length, &temp_buffer, &temp_buffer_size) != HM_SUCCESS){
        return THEMIS_FAIL;
    };
    if(temp_buffer_size > key_buffer_length){
        return THEMIS_FAIL;
    }
    memcpy(key_buffer, temp_buffer, temp_buffer_size);
    free(temp_buffer);
    temp_buffer = NULL;
    return THEMIS_SUCCESS;
}

int get_public_key_for_id_from_local_credential_store_callback(
        const void *id, size_t id_length, void *key_buffer, size_t key_buffer_length, void *user_data) {
    hm_cs_db_t *db = (hm_cs_db_t *) user_data;
    uint8_t *temp_buffer;
    size_t temp_buffer_size;
    if (db->get_pub(db->user_data, id, id_length, &temp_buffer, &temp_buffer_size) != HM_SUCCESS ) {
        return THEMIS_FAIL;
    };
    if (temp_buffer_size > key_buffer_length) {
        return THEMIS_FAIL;
    }
    memcpy(key_buffer, temp_buffer, temp_buffer_size);
    free(temp_buffer);
    temp_buffer = NULL;
    return THEMIS_SUCCESS;
}


secure_session_user_callbacks_t* get_session_callback_with_remote_credential_store(hm_rpc_transport_t *transport){
    secure_session_user_callbacks_t* session_callback = calloc(1, sizeof(secure_session_user_callbacks_t));
    if(!session_callback){
        return NULL;
    }
    hm_credential_store_client_sync_t* db = hm_credential_store_client_sync_create(transport);
    if (!db){
        free(session_callback);
        return NULL;
    }

    session_callback->user_data = db;
    session_callback->get_public_key_for_id = get_public_key_for_id_from_remote_credential_store_callback;
    return session_callback;
}

secure_session_user_callbacks_t* get_session_callback_with_local_credential_store(hm_cs_db_t* credential_store){
    secure_session_user_callbacks_t* session_callback = calloc(1, sizeof(secure_session_user_callbacks_t));
    session_callback->user_data = credential_store;
    session_callback->get_public_key_for_id = get_public_key_for_id_from_local_credential_store_callback;
    return session_callback;
}
