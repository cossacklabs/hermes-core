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

#ifndef HERMES_CORE_SESSION_CALLBACK_H
#define HERMES_CORE_SESSION_CALLBACK_H

#include <themis/secure_session.h>
#include <hermes/credential_store/client.h>
#include <hermes/credential_store/db.h>
#include <hermes/common/errors.h>

//get_session_callback_with_remote_credential_store return secure_session callback that fetch public keys from remote
//credential store
secure_session_user_callbacks_t* get_session_callback_with_remote_credential_store(hm_rpc_transport_t *transport);
secure_session_user_callbacks_t* get_session_callback_with_local_credential_store(hm_cs_db_t* credential_store);
int get_public_key_for_id_from_local_credential_store_callback(
        const void *id, size_t id_length, void *key_buffer, size_t key_buffer_length, void *user_data);

int get_public_key_for_id_from_remote_credential_store_callback(
        const void *id, size_t id_length, void *key_buffer, size_t key_buffer_length, void *user_data);

#endif //HERMES_CORE_SESSION_CALLBACK_H
