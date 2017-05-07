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

#include <hermes/credential_store/client.h>
#include <hermes/key_store/client.h>
#include <hermes/data_store/client.h>
#include <hermes/mid_hermes/mid_hermes.h>
#include "utils.h"
#include <hermes/common/errors.h>
#include <soter/soter.h>
#include <string.h>
#include <assert.h>

struct mid_hermes_type{
  hm_credential_store_client_sync_t* csc;
  hm_key_store_client_sync_t* ksc;
  hm_data_store_client_sync_t* dsc;
  uint8_t* id;
  size_t id_len;
  uint8_t* sk;
  size_t sk_len;
  uint8_t* pk;
  size_t pk_len;
};

mid_hermes_t* mid_hermes_create(const uint8_t* id, const size_t id_len, const uint8_t* sk, const size_t sk_len, hm_rpc_transport_t* cs_transport, hm_rpc_transport_t* ks_transport, hm_rpc_transport_t* ds_transport){
  if(!id || !id_len || !sk || !sk_len || !cs_transport || !ks_transport || !ds_transport){
    return NULL;
  }
  mid_hermes_t* mh=calloc(1, sizeof(mid_hermes_t));
  assert(mh);
  mh->id=malloc(id_len);
  assert(mh->id);
  mh->sk=malloc(sk_len);
  assert(mh->sk);
  memcpy(mh->id, id, id_len);
  mh->id_len=id_len;
  memcpy(mh->sk, sk, sk_len);
  mh->sk_len=sk_len;
  mh->csc=hm_credential_store_client_sync_create(cs_transport);
  mh->ksc=hm_key_store_client_sync_create(ks_transport);
  mh->dsc=hm_data_store_client_sync_create(ds_transport);
  if(!(mh->csc) || !(mh->ksc) || !(mh->dsc)){
    mid_hermes_destroy(&mh);
    return NULL;
  }
  if(HM_SUCCESS!=hm_credential_store_client_sync_call_get_pub_key_by_id(mh->csc, mh->id, mh->id_len, &(mh->pk), &(mh->pk_len))){
    mid_hermes_destroy(&mh);
    return NULL;
  }
  return mh;
}

uint32_t mid_hermes_destroy(mid_hermes_t** mh){
  if(!mh || !(*mh)){
    return HM_INVALID_PARAMETER;
  }
  free((*mh)->id);
  free((*mh)->sk);
  hm_credential_store_client_sync_destroy(&((*mh)->csc));
  hm_key_store_client_sync_destroy(&((*mh)->ksc));
  hm_data_store_client_sync_destroy(&((*mh)->dsc));
  free(*mh);
  *mh=NULL;
  return HM_SUCCESS;
}

uint32_t mid_hermes_create_block(mid_hermes_t* mh, const uint8_t* block, const size_t block_len, const uint8_t* meta, const size_t meta_len, uint8_t** id, size_t* id_len){
  if(!mh || !(mh->dsc) || !(mh->ksc) ||!block || !block_len || !meta || !meta_len || !id || !id_len){
    return HM_INVALID_PARAMETER;
  }
  uint8_t rtoken[HM_TOKEN_LEN];
  uint8_t wtoken[HM_TOKEN_LEN];
  if(SOTER_SUCCESS!=soter_rand(rtoken, HM_TOKEN_LEN) || SOTER_SUCCESS!=soter_rand(wtoken, HM_TOKEN_LEN)){
    return HM_FAIL;
  }
  uint8_t *enc_rtoken=NULL, *enc_wtoken=NULL;
  size_t enc_rtoken_len=0, enc_wtoken_len=0;
  uint32_t res;
  if(HM_SUCCESS!=(res=hm_asym_encrypt(mh->sk, mh->sk_len, mh->pk, mh->pk_len, rtoken, HM_TOKEN_LEN, &enc_rtoken, &enc_rtoken_len))){
    return res;
  }
  if(HM_SUCCESS!=(res=hm_asym_encrypt(mh->sk, mh->sk_len, mh->pk, mh->pk_len, wtoken, HM_TOKEN_LEN, &enc_wtoken, &enc_wtoken_len))){
    free(enc_rtoken);
    return res;
  }  
  uint8_t* mac=NULL, *enc_block=NULL;
  size_t mac_len=0, enc_block_len=0, id_len_=0;
  if(HM_SUCCESS!=(res=hm_mac_create(wtoken, HM_TOKEN_LEN, block, block_len, meta, meta_len, &mac, &mac_len))){
    free(enc_rtoken);
    free(enc_wtoken);
    return res;
  }
  if(HM_SUCCESS!=(res=hm_encrypt(rtoken, HM_TOKEN_LEN, block, block_len, meta, meta_len, &enc_block, &enc_block_len))){
    free(enc_rtoken);
    free(enc_wtoken);
    free(mac);
    return res;
  }
  res=hm_data_store_client_sync_call_create_block(mh->dsc, enc_block, enc_block_len, meta, meta_len, mac, mac_len, id, &id_len_);
  free(enc_block);
  if(HM_SUCCESS!=res){
    free(mac);
    free(enc_rtoken);
    free(enc_wtoken);
    return res;
  }
  if(HM_SUCCESS!=(res=hm_key_store_client_sync_call_set_rtoken(mh->ksc, *id, id_len_, mh->id, mh->id_len, mh->id, mh->id_len, enc_rtoken, enc_rtoken_len))){
    free(enc_rtoken);
    free(enc_wtoken);
    hm_data_store_client_sync_call_delete_block(mh->dsc, *id, id_len_, mac, mac_len);
    free(mac);
    free(*id);
    return res;    
  }
  free(enc_rtoken);
  if(HM_SUCCESS!=(res=hm_key_store_client_sync_call_set_rtoken(mh->ksc, *id, id_len_, mh->id, mh->id_len, mh->id, mh->id_len, enc_wtoken, enc_wtoken_len))){
    free(enc_wtoken);
    hm_data_store_client_sync_call_delete_block(mh->dsc, *id, id_len_, mac, mac_len);
    free(mac);
    free(*id);
    return res;    
  }
  free(enc_wtoken);
  *id_len=id_len_;
  return HM_SUCCESS;
}

uint32_t mid_hermes_get_token(mid_hermes_t* mh, const uint8_t* block_id, const size_t block_id_len, bool is_update, uint8_t** token, size_t* token_len){
  if(!mh || !block_id || !block_id_len || !token || !token_len){
    return HM_INVALID_PARAMETER;
  }
  uint8_t* enc_token=NULL, *owner_id=NULL, *owner_pk=NULL;
  size_t enc_token_len=0, owner_id_len=0, owner_pk_len=0;
  uint32_t res;
  if(HM_SUCCESS!=(res=(is_update?hm_key_store_client_sync_call_get_wtoken(mh->ksc, block_id, block_id_len, mh->id, mh->id_len, &enc_token, &enc_token_len, &owner_id, &owner_id_len):hm_key_store_client_sync_call_get_wtoken(mh->ksc, block_id, block_id_len, mh->id, mh->id_len, &enc_token, &enc_token_len, &owner_id, &owner_id_len)))){
    return res;
  }
  if(HM_SUCCESS!=(res=hm_credential_store_client_sync_call_get_pub_key_by_id(mh->csc, owner_id, owner_id_len, &owner_pk, &owner_pk_len))){
    free(enc_token);
    free(owner_id);
    return res;
  }
  return res;
}

uint32_t mid_hermes_read_block_(mid_hermes_t* mh, const uint8_t* block_id, const size_t block_id_len, uint8_t** block, size_t* block_len, uint8_t** meta, size_t* meta_len, uint8_t** rtoken, size_t* rtoken_len){
  if(!mh || !block_id || !block_id_len || !block || !block_len || !meta || !meta_len || !rtoken || !rtoken_len){
    return HM_INVALID_PARAMETER;
  }
  uint8_t *enc_block=NULL;
  size_t enc_block_len=0;
  uint32_t res;
  if(HM_SUCCESS!=(res=mid_hermes_get_token(mh, block_id, block_id_len, false, rtoken, rtoken_len))){
    return res;
  }
  if(HM_SUCCESS!=(res=hm_data_store_client_sync_call_read_block(mh->dsc, block_id, block_id_len, &enc_block, &enc_block_len, meta, meta_len))){
    free(*rtoken);
    return res;
  }
  res=hm_decrypt(*rtoken, *rtoken_len, enc_block, enc_block_len, *meta, *meta_len, block, block_len);
  free(enc_block);
  if(HM_SUCCESS!=res){
    free(*rtoken);
    free(*meta);
    return res;
  }
  return HM_SUCCESS;
}

uint32_t mid_hermes_read_block(mid_hermes_t* mh, const uint8_t* block_id, const size_t block_id_len, uint8_t** block, size_t* block_len, uint8_t** meta, size_t* meta_len){
  if(!mh || !block_id || !block_id_len || !block || !block_len || !meta || !meta_len){
    return HM_INVALID_PARAMETER;
  }
  uint8_t* rtoken=NULL;
  size_t rtoken_len=0;
  uint32_t res=mid_hermes_read_block_(mh, block_id, block_id_len, block, block_len, meta, meta_len, &rtoken, &rtoken_len);
  if(HM_SUCCESS==res){
    free(rtoken);
  }
  return res;
}

uint32_t mid_hermes_read_block_mac(mid_hermes_t* mh, const uint8_t* block_id, const size_t block_id_len, uint8_t** mac, size_t* mac_len, uint8_t** rtoken, size_t* rtoken_len, uint8_t** wtoken, size_t* wtoken_len){
  if(!mh || !block_id || !block_id_len || ! mac || !mac_len){
    return HM_INVALID_PARAMETER;
  }
  uint8_t *block=NULL, *meta=NULL;
  size_t block_len=0, meta_len=0;
  uint32_t res;
  if(HM_SUCCESS!=(res=mid_hermes_read_block_(mh, block_id, block_id_len, &block, &block_len, &meta, &meta_len, rtoken, rtoken_len))){
    return res;
  }
  if(HM_SUCCESS!=(res=mid_hermes_get_token(mh, block_id, block_id_len, true, wtoken, wtoken_len))){
    free(block);
    free(meta);
    free(*rtoken);
    return res;
  }
  res=hm_mac_create(*wtoken, *wtoken_len, block, block_len, meta, meta_len, mac, mac_len);
  free(block);
  free(meta);
  if(HM_SUCCESS!=res){
    free(*wtoken);
    free(*rtoken);
  }
  return res;
}

uint32_t mid_hermes_update_block(mid_hermes_t* mh, const uint8_t* block_id, const size_t block_id_len, const uint8_t* block, const size_t block_len, const uint8_t* meta, const size_t meta_len){
  if(!mh || !block_id || !block_id_len || !block || !block_len || !meta || !meta_len){
    return HM_INVALID_PARAMETER;
  }
  uint8_t* old_mac=NULL, *rtoken=NULL, *wtoken=NULL, *mac=NULL, *enc_block=NULL;
  size_t old_mac_len=0, rtoken_len=0, wtoken_len=0, mac_len=0, enc_block_len=0;
  uint32_t res;
  if(HM_SUCCESS!=(res=mid_hermes_read_block_mac(mh, block_id, block_id_len, &old_mac, &old_mac_len, &rtoken, &rtoken_len, &wtoken, &wtoken_len))){
    return res;
  }
  if(HM_SUCCESS!=(res=hm_mac_create(wtoken, wtoken_len, block, block_len, meta, meta_len, &mac, &mac_len))){
    free(old_mac);
    free(wtoken);
    free(rtoken);
    return res;
  }
  free(wtoken);
  if(HM_SUCCESS!=(res=hm_encrypt(rtoken, rtoken_len, block, block_len, meta, meta_len, &enc_block, &enc_block_len))){
    free(rtoken);
    free(old_mac);
    free(mac);
    return res;
  }
  free(rtoken);
  res=hm_data_store_client_sync_call_update_block(mh->dsc, block_id, block_id_len, enc_block, enc_block_len, meta, meta_len, mac, mac_len, old_mac, old_mac_len);
  free(mac);
  free(enc_block);
  free(old_mac);
  return res;
}

uint32_t mid_hermes_delete_block(mid_hermes_t* mh, const uint8_t* block_id, const size_t block_id_length){
  if(!mh || !block_id || !block_id_length){
    return HM_INVALID_PARAMETER;
  }
  uint8_t* old_mac=NULL, *rtoken=NULL, *wtoken=NULL, *mac=NULL;
  size_t old_mac_len=0, rtoken_len=0, wtoken_len=0, mac_len=0;
  uint32_t res;
  if(HM_SUCCESS!=(res=mid_hermes_read_block_mac(mh, block_id, block_id_length, &old_mac, &old_mac_len, &rtoken, &rtoken_len, &wtoken, &wtoken_len))){
    return res;
  }
  free(rtoken);
  free(wtoken);
  res=hm_data_store_client_sync_call_delete_block(mh->dsc, block_id, block_id_length, old_mac, old_mac_len);
  free(old_mac);
  return res;
}

uint32_t grant_access(mid_hermes_t* mh, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, bool is_update){
  if(!mh || !block_id || !block_id_length || !user_id || !user_id_length){
    return HM_INVALID_PARAMETER;
  }
  uint8_t* token=NULL, *enc_token=NULL, *peer_pk=NULL;
  size_t token_len=0, enc_token_len=0, peer_pk_len=0;
  uint32_t res;
  if(HM_SUCCESS!=(res=hm_credential_store_client_sync_call_get_pub_key_by_id(mh->csc, user_id, user_id_length, &peer_pk, &peer_pk_len))){
    return res;
  }
  if(HM_SUCCESS!=(res=mid_hermes_get_token(mh, block_id, block_id_length, is_update, &token, &token_len))){
    free(peer_pk);
    return res;
  }
  if(HM_SUCCESS!=(res=hm_asym_encrypt(mh->sk, mh->sk_len, peer_pk, peer_pk_len, token, token_len, &enc_token, &enc_token_len))){
    free(peer_pk);
    free(token);
    return res;
  }
  res=is_update?hm_key_store_client_sync_call_set_wtoken(mh->ksc, block_id, block_id_length, user_id, user_id_length, mh->id, mh->id_len, enc_token, enc_token_len):hm_key_store_client_sync_call_set_rtoken(mh->ksc, block_id, block_id_length, user_id, user_id_length, mh->id, mh->id_len, enc_token, enc_token_len);
  free(peer_pk);
  free(token);
  free(enc_token);
  return res;
}

uint32_t grant_read_access(mid_hermes_t* mh, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length){
  return grant_access(mh, block_id, block_id_length, user_id, user_id_length, false);
}

uint32_t grant_update_access(mid_hermes_t* mh, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length){
  return grant_access(mh, block_id, block_id_length, user_id, user_id_length, true);
}

uint32_t deny_read_access(mid_hermes_t* mh, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length){
  if(!mh){
    return HM_INVALID_PARAMETER;
  }
  return hm_key_store_client_sync_call_del_rtoken(mh->ksc, block_id, block_id_length, user_id, user_id_length, mh->id, mh->id_len);
}

uint32_t deny_update_access(mid_hermes_t* mh, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length){
  if(!mh){
    return HM_INVALID_PARAMETER;
  }
  return hm_key_store_client_sync_call_del_wtoken(mh->ksc, block_id, block_id_length, user_id, user_id_length, mh->id, mh->id_len);
}

