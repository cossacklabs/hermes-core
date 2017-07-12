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


#include "credential_store_impl.h"

#include <hermes/credential_store/client.h>

struct hermes_credential_store_type{
  hm_credential_store_client_sync_t* cl;
};

hermes_credential_store_t* hermes_credential_store_create(hm_rpc_transport_t* transport){
  if(!transport){
    return NULL;
  }
  hermes_credential_store_t* cs=calloc(1, sizeof(hermes_credential_store_t));
  if(!cs
     || !(cs->cl=hm_credential_store_client_sync_create(transport))){
    hermes_credential_store_destroy(&cs);
    return NULL;
  }
  return cs;
}

hermes_status_t hermes_credential_store_get_public_key(hermes_credential_store_t* cs, const uint8_t* user_id, const size_t user_id_length, uint8_t** pub_key, size_t* pub_key_length){
  if(!cs || !user_id || !user_id_length || !pub_key || !pub_key_length){
    return HM_INVALID_PARAMETER;
  }
  return hm_credential_store_client_sync_call_get_pub_key_by_id(cs->cl, user_id, user_id_length, pub_key, pub_key_length);
}

hermes_status_t hermes_credential_store_destroy(hermes_credential_store_t** cs){
  if(!cs || !(*cs)){
    return HM_INVALID_PARAMETER;
  }
  hm_credential_store_client_sync_destroy(&((*cs)->cl));
  free(*cs);
  *cs=NULL;
  return HM_SUCCESS;
}
