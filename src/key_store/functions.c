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

#include "functions.h"

#include <hermes/common/errors.h>

#include <string.h>

uint32_t hm_key_store_set_rtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length, const uint8_t* rtoken, const size_t rtoken_length){
  if(!db || !(db->user_data) || !(db->get_rtoken) || !(db->set_rtoken) || !block_id || !block_id_length || !user_id || !user_id_length || !owner_id || !owner_id_length || !rtoken || !rtoken_length){
    return HM_INVALID_PARAMETER;
  }
  uint32_t res;
  if(user_id_length!=owner_id_length || 0!=memcmp(user_id, owner_id, user_id_length)){
    uint8_t *test_token=NULL, *test_id=NULL;
    size_t test_token_length=0, test_id_length=0;
    if(HM_SUCCESS!=(res=db->get_rtoken(db->user_data, block_id, block_id_length, owner_id, owner_id_length, &test_token, &test_token_length, &test_id, &test_id_length))){
      return HM_FAIL;
    }
    free(test_token);
    free(test_id);
  }
  return db->set_rtoken(db->user_data, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length, rtoken, rtoken_length);
}

uint32_t hm_key_store_set_wtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length, const uint8_t* wtoken, const size_t wtoken_length){
  if(!db || !(db->user_data) || !(db->get_wtoken) || !(db->set_wtoken) || !block_id || !block_id_length || !user_id || !user_id_length || !owner_id || !owner_id_length || !wtoken || !wtoken_length){
    return HM_INVALID_PARAMETER;
  }
  uint32_t res;
  if(user_id_length!=owner_id_length || 0!=memcmp(user_id, owner_id, user_id_length)){
    uint8_t *test_token=NULL, *test_id=NULL;
    size_t test_token_length=0, test_id_length=0;
    if(HM_SUCCESS!=(res=db->get_wtoken(db->user_data, block_id, block_id_length, owner_id, owner_id_length, &test_token, &test_token_length, &test_id, &test_id_length))){
      return HM_FAIL;
    }
    free(test_token);
    free(test_id);
  }
  return db->set_wtoken(db->user_data, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length, wtoken, wtoken_length);
}

uint32_t hm_key_store_get_rtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, uint8_t** rtoken, size_t* rtoken_length, uint8_t** owner_id, size_t* owner_id_length){
  if(!db || !(db->user_data) || !(db->get_rtoken) || !block_id || !block_id_length || !user_id || !user_id_length || !rtoken || !owner_id){
    return HM_INVALID_PARAMETER;
  }
  return db->get_rtoken(db->user_data, block_id, block_id_length, user_id, user_id_length, rtoken, rtoken_length, owner_id, owner_id_length);
}

uint32_t hm_key_store_get_wtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, uint8_t** wtoken, size_t* wtoken_length, uint8_t** owner_id, size_t* owner_id_length){
  if(!db || !(db->user_data) || !(db->get_wtoken) || !block_id || !block_id_length || !user_id || !user_id_length || !wtoken || !owner_id){
    return HM_INVALID_PARAMETER;
  }
  return db->get_wtoken(db->user_data, block_id, block_id_length, user_id, user_id_length, wtoken, wtoken_length, owner_id, owner_id_length);
}

uint32_t hm_key_store_del_rtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length){
  if(!db || !(db->user_data) || !(db->get_rtoken) || (!(db->del_rtoken)) || !block_id || !block_id_length || !user_id || !user_id_length || !owner_id || !owner_id_length){
    return HM_INVALID_PARAMETER;
  }
  uint8_t *test_token=NULL, *test_id=NULL;
  size_t test_token_length=0, test_id_length=0;
  if(HM_SUCCESS!=db->get_rtoken(db->user_data, block_id, block_id_length, owner_id, owner_id_length, &test_token, &test_token_length, &test_id, &test_id_length)){
    return HM_FAIL;
  }
  return db->del_rtoken(db->user_data, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length);
}

uint32_t hm_key_store_del_wtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length){
  if(!db || !(db->user_data) || !(db->get_wtoken) || !(db->del_wtoken) || !block_id || !block_id_length || !user_id || !user_id_length || !owner_id || !owner_id_length){
    return HM_INVALID_PARAMETER;
  }
  uint8_t *test_token=NULL, *test_id=NULL;
  size_t test_token_length=0, test_id_length=0;
  if(HM_SUCCESS!=db->get_wtoken(db->user_data, block_id, block_id_length, owner_id, owner_id_length, &test_token, &test_token_length, &test_id, &test_id_length)){
    return HM_FAIL;
  }
  return db->del_wtoken(db->user_data, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length);  
}


//proxies

uint32_t hm_key_store_set_rtoken_sync_proxy(hm_rpc_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length, const uint8_t* rtoken, const size_t rtoken_length){
  if(!c || !block_id || !block_id_length || !user_id || !user_id_length || !owner_id || !owner_id_length || !rtoken || !rtoken_length){
    return HM_INVALID_PARAMETER;
  }
  hm_param_pack_t* in=HM_PARAM_PACK(HM_PARAM_BUFFER_C(block_id, block_id_length), HM_PARAM_BUFFER_C(user_id, user_id_length), HM_PARAM_BUFFER_C(owner_id, owner_id_length), HM_PARAM_BUFFER_C(rtoken, rtoken_length));
  if(!in){
    return HM_FAIL;
  }
  uint32_t status, res;
  if(HM_SUCCESS!=(res=hm_rpc_client_sync_call(c, (const uint8_t*)hm_key_store_set_rtoken_NAME, sizeof(hm_key_store_set_rtoken_NAME), in, &status, NULL))){
    hm_param_pack_destroy(&in);
    return res;
  }
  hm_param_pack_destroy(&in);
  return status;
}

uint32_t hm_key_store_set_wtoken_sync_proxy(hm_rpc_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length, const uint8_t* wtoken, const size_t wtoken_length){
  if(!c || !block_id || !block_id_length || !user_id || !user_id_length || !owner_id || !owner_id_length || !wtoken || !wtoken_length){
    return HM_INVALID_PARAMETER;
  }
  hm_param_pack_t* in=HM_PARAM_PACK(HM_PARAM_BUFFER_C(block_id, block_id_length), HM_PARAM_BUFFER_C(user_id, user_id_length), HM_PARAM_BUFFER_C(owner_id, owner_id_length), HM_PARAM_BUFFER_C(wtoken, wtoken_length));
  if(!in){
    return HM_FAIL;
  }
  uint32_t status, res;
  if(HM_SUCCESS!=(res=hm_rpc_client_sync_call(c, (const uint8_t*)hm_key_store_set_wtoken_NAME, sizeof(hm_key_store_set_wtoken_NAME), in, &status, NULL))){
    hm_param_pack_destroy(&in);
    return res;
  }
  hm_param_pack_destroy(&in);
  return status;
}

uint32_t hm_key_store_get_rtoken_sync_proxy(hm_rpc_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, uint8_t** rtoken, size_t* rtoken_length, uint8_t** owner_id, size_t* owner_id_length){
  if(!c || !block_id || !block_id_length || !user_id || !user_id_length){
    return HM_INVALID_PARAMETER;
  }
  hm_param_pack_t* in=HM_PARAM_PACK(HM_PARAM_BUFFER_C(block_id, block_id_length), HM_PARAM_BUFFER_C(user_id, user_id_length));
  if(!in){
    return HM_FAIL;
  }
  hm_param_pack_t* out=NULL;
  uint32_t status, res;
  if(HM_SUCCESS!=(res=hm_rpc_client_sync_call(c, (const uint8_t*)hm_key_store_get_rtoken_NAME, sizeof(hm_key_store_get_rtoken_NAME), in, &status, &(out)))){
    hm_param_pack_destroy(&in);
    return res;
  }
  hm_param_pack_destroy(&in);
  if(HM_SUCCESS!=status){
    return status;
  }
  if(HM_SUCCESS!=(res=HM_PARAM_EXTRACT(out, HM_PARAM_BUFFER(rtoken, rtoken_length), HM_PARAM_BUFFER(owner_id, owner_id_length)))){
    hm_param_pack_destroy(&out);
    return res;
  }
  return HM_SUCCESS;
}

uint32_t hm_key_store_get_wtoken_sync_proxy(hm_rpc_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, uint8_t** wtoken, size_t* wtoken_length, uint8_t** owner_id, size_t* owner_id_length){
  if(!c || !block_id || !block_id_length || !user_id || !user_id_length){
    return HM_INVALID_PARAMETER;
  }
  hm_param_pack_t* in=HM_PARAM_PACK(HM_PARAM_BUFFER_C(block_id, block_id_length), HM_PARAM_BUFFER_C(user_id, user_id_length));
  if(!in){
    return HM_FAIL;
  }
  hm_param_pack_t* out=NULL;
  uint32_t status, res;
  if(HM_SUCCESS!=(res=hm_rpc_client_sync_call(c, (const uint8_t*)hm_key_store_get_wtoken_NAME, sizeof(hm_key_store_get_wtoken_NAME), in, &status, &(out)))){
    hm_param_pack_destroy(&in);
    return res;
  }
  hm_param_pack_destroy(&in);
  if(HM_SUCCESS!=status){
    return status;
  }
  if(HM_SUCCESS!=(res=HM_PARAM_EXTRACT(out, HM_PARAM_BUFFER(wtoken, wtoken_length), HM_PARAM_BUFFER(owner_id, owner_id_length)))){
    hm_param_pack_destroy(&out);
    return res;
  }
  return HM_SUCCESS;
}

uint32_t hm_key_store_del_rtoken_sync_proxy(hm_rpc_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length){
  if(!c || !block_id || !block_id_length || !user_id || !user_id_length || !owner_id || !owner_id_length){
    return HM_INVALID_PARAMETER;
  }
  hm_param_pack_t* in=HM_PARAM_PACK(HM_PARAM_BUFFER_C(block_id, block_id_length), HM_PARAM_BUFFER_C(user_id, user_id_length), HM_PARAM_BUFFER_C(owner_id, owner_id_length));
  if(!in){
    return HM_FAIL;
  }
  uint32_t status, res;
  if(HM_SUCCESS!=(res=hm_rpc_client_sync_call(c, (const uint8_t*)hm_key_store_del_rtoken_NAME, sizeof(hm_key_store_del_rtoken_NAME), in, &status, NULL))){
    hm_param_pack_destroy(&in);
    return res;
  }
  hm_param_pack_destroy(&in);
  return status;
}

uint32_t hm_key_store_del_wtoken_sync_proxy(hm_rpc_client_sync_t* c, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length){
  if(!c || !block_id || !block_id_length || !user_id || !user_id_length || !owner_id || !owner_id_length){
    return HM_INVALID_PARAMETER;
  }
  hm_param_pack_t* in=HM_PARAM_PACK(HM_PARAM_BUFFER_C(block_id, block_id_length), HM_PARAM_BUFFER_C(user_id, user_id_length), HM_PARAM_BUFFER_C(owner_id, owner_id_length));
  if(!in){
    return HM_FAIL;
  }
  uint32_t status, res;
  if(HM_SUCCESS!=(res=hm_rpc_client_sync_call(c, (const uint8_t*)hm_key_store_del_wtoken_NAME, sizeof(hm_key_store_del_wtoken_NAME), in, &status, NULL))){
    hm_param_pack_destroy(&in);
    return res;
  }
  hm_param_pack_destroy(&in);
  return status;
}

//stubs
uint32_t hm_key_store_set_rtoken_stub(hm_param_pack_t* in, hm_param_pack_t** out, void* user_data){
  if(!user_data || !in || !out){
    return HM_INVALID_PARAMETER;
  }
  uint8_t *block_id=NULL, *user_id=NULL, *owner_id=NULL, *rtoken=NULL;
  size_t block_id_length=0, user_id_length=0, owner_id_length=0, rtoken_length=0;
  uint32_t res;
  if(HM_SUCCESS!=(res=HM_PARAM_EXTRACT(in, HM_PARAM_BUFFER(&block_id, &block_id_length), HM_PARAM_BUFFER(&user_id, &user_id_length), HM_PARAM_BUFFER(&owner_id, &owner_id_length), HM_PARAM_BUFFER(&rtoken, &rtoken_length)))){
    return res;
  }
  return hm_key_store_set_rtoken((hm_ks_db_t*)user_data, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length, rtoken, rtoken_length);
}

uint32_t hm_key_store_set_wtoken_stub(hm_param_pack_t* in, hm_param_pack_t** out, void* user_data){
  if(!user_data || !in || !out){
    return HM_INVALID_PARAMETER;
  }
  uint8_t *block_id=NULL, *user_id=NULL, *owner_id=NULL, *wtoken=NULL;
  size_t block_id_length=0, user_id_length=0, owner_id_length=0, wtoken_length=0;
  uint32_t res;
  if(HM_SUCCESS!=(res=HM_PARAM_EXTRACT(in, HM_PARAM_BUFFER(&block_id, &block_id_length), HM_PARAM_BUFFER(&user_id, &user_id_length), HM_PARAM_BUFFER(&owner_id, &owner_id_length), HM_PARAM_BUFFER(&wtoken, &wtoken_length)))){
    return res;
  }
  return hm_key_store_set_wtoken((hm_ks_db_t*)user_data, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length, wtoken, wtoken_length);
}

uint32_t hm_key_store_get_rtoken_stub(hm_param_pack_t* in, hm_param_pack_t** out, void* user_data){
  if(!user_data || !in || !out){
    return HM_INVALID_PARAMETER;
  }
  uint8_t *block_id=NULL, *user_id=NULL, *owner_id=NULL, *rtoken=NULL;
  size_t block_id_length=0, user_id_length=0, owner_id_length=0, rtoken_length=0;
  uint32_t res;
  if(HM_SUCCESS!=(res=HM_PARAM_EXTRACT(in, HM_PARAM_BUFFER(&block_id, &block_id_length), HM_PARAM_BUFFER(&user_id, &user_id_length)))){
    return res;
  }
  if(HM_SUCCESS!=(res=hm_key_store_get_rtoken((hm_ks_db_t*)user_data, block_id, block_id_length, user_id, user_id_length, &rtoken, &rtoken_length, &owner_id, &owner_id_length))){
    return res;
  }
  *out=HM_PARAM_PACK(HM_PARAM_BUFFER(rtoken, rtoken_length), HM_PARAM_BUFFER(owner_id, owner_id_length));
  if(!(*out)){
    return HM_FAIL;
  }
  return HM_SUCCESS;
}

uint32_t hm_key_store_get_wtoken_stub(hm_param_pack_t* in, hm_param_pack_t** out, void* user_data){
  if(!user_data || !in || !out){
    return HM_INVALID_PARAMETER;
  }
  uint8_t *block_id=NULL, *user_id=NULL, *owner_id=NULL, *wtoken=NULL;
  size_t block_id_length=0, user_id_length=0, owner_id_length=0, wtoken_length=0;
  uint32_t res;
  if(HM_SUCCESS!=(res=HM_PARAM_EXTRACT(in, HM_PARAM_BUFFER(&block_id, &block_id_length), HM_PARAM_BUFFER(&user_id, &user_id_length)))){
    return res;
  }
  if(HM_SUCCESS!=(res=hm_key_store_get_wtoken((hm_ks_db_t*)user_data, block_id, block_id_length, user_id, user_id_length, &wtoken, &wtoken_length, &owner_id, &owner_id_length))){
    return res;
  }
  *out=HM_PARAM_PACK(HM_PARAM_BUFFER(wtoken, wtoken_length), HM_PARAM_BUFFER(owner_id, owner_id_length));
  if(!(*out)){
    return HM_FAIL;
  }
  return HM_SUCCESS;
}

uint32_t hm_key_store_del_rtoken_stub(hm_param_pack_t* in, hm_param_pack_t** out, void* user_data){
  if(!user_data || !in || !out){
    return HM_INVALID_PARAMETER;
  }
  uint8_t *block_id=NULL, *user_id=NULL, *owner_id=NULL;
  size_t block_id_length=0, user_id_length=0, owner_id_length=0;
  uint32_t res;
  if(HM_SUCCESS!=(res=HM_PARAM_EXTRACT(in, HM_PARAM_BUFFER(&block_id, &block_id_length), HM_PARAM_BUFFER(&user_id, &user_id_length), HM_PARAM_BUFFER(&owner_id, &owner_id_length)))){
    return res;
  }
  return hm_key_store_del_rtoken((hm_ks_db_t*)user_data, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length);
}

uint32_t hm_key_store_del_wtoken_stub(hm_param_pack_t* in, hm_param_pack_t** out, void* user_data){
  if(!user_data || !in || !out){
    return HM_INVALID_PARAMETER;
  }
  uint8_t *block_id=NULL, *user_id=NULL, *owner_id=NULL;
  size_t block_id_length=0, user_id_length=0, owner_id_length=0;
  uint32_t res;
  if(HM_SUCCESS!=(res=HM_PARAM_EXTRACT(in, HM_PARAM_BUFFER(&block_id, &block_id_length), HM_PARAM_BUFFER(&user_id, &user_id_length), HM_PARAM_BUFFER(&owner_id, &owner_id_length)))){
    return res;
  }
  return hm_key_store_del_wtoken((hm_ks_db_t*)user_data, block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length);
}
