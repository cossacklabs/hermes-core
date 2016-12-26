/*
 * Copyright (c) 2016 Cossack Labs Limited
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
#include "crypt_impl.h"
#include <hermes/common/errors.h>
#include <themis/themis.h>

#include <string.h>

#define HM_TOKEN_LENGTH 64
#define HM_MAC_LENGTH 16
#define HM_DEFAULT_CTX "hermes default context"
#define HM_DEFAULT_CTX_LENGTH strlen(HM_DEFAULT_CTX)

struct hm_crypter_impl_t_{
  uint8_t *id_;
  size_t id_length_;
  uint8_t *priv_;
  size_t priv_length_;
};

hm_crypter_impl_t* hm_crypter_impl_create(const uint8_t* id, const size_t id_length, const uint8_t* private_key, const size_t private_key_length){
  if(!id || !id_length || !private_key || !private_key_length){
    return NULL; //invalid param
  }
  hm_crypter_impl_t* crypter = calloc(sizeof(hm_crypter_impl_t), 1);
  if(!crypter){
    return NULL; //bad alloc
  }
  crypter->id_ = malloc(id_length);
  if(!crypter->id_){
    hm_crypter_impl_destroy(&crypter);
    return NULL;
  }
  memcpy(crypter->id_, id, id_length);
  crypter->id_length_=id_length;
  crypter->priv_ = malloc(private_key_length);
  if(!crypter->priv_){
    hm_crypter_impl_destroy(&crypter);
    return NULL;
  }
  memcpy(crypter->priv_, private_key, private_key_length);
  crypter->priv_length_=private_key_length;
  return crypter;
}

void hm_crypter_impl_destroy(hm_crypter_impl_t** crypter){
  if(!crypter || !(*crypter)){
    return; //invalid param
  }
  free((*crypter)->id_);
  free((*crypter)->priv_);
  free(*crypter);
  *crypter = NULL;
}

int hm_crypter_impl_encrypt(hm_crypter_impl_t* crypter,
                            const uint8_t* public_key,
                            const size_t public_key_length,
                            const uint8_t* data,
                            const size_t data_length,
                            uint8_t* encrypted_data,
                            size_t* encrypted_data_length){
  if(!crypter || !(crypter->priv_) || !public_key || !public_key_length || !data || !data_length){
    return HM_INVALID_PARAMETER;
  }
  themis_status_t res = themis_secure_message_wrap(crypter->priv_, crypter->priv_length_, public_key, public_key_length, data, data_length, encrypted_data, encrypted_data_length);
  switch(res){
  case THEMIS_BUFFER_TOO_SMALL:
    return HM_BUFFER_TOO_SMALL;
  case THEMIS_SUCCESS:
    return HM_SUCCESS;
  }
  return HM_FAIL;
}

int hm_crypter_impl_decrypt(hm_crypter_impl_t* crypter,
                            const uint8_t* public_key,
                            const size_t public_key_length,
                            const uint8_t* encrypted_data,
                            const size_t encrypted_data_length,
                            uint8_t* data, size_t* data_length){
  if(!crypter || !(crypter->priv_) || !public_key || !public_key_length || !encrypted_data || !encrypted_data_length){
    return HM_INVALID_PARAMETER;
  }
  themis_status_t res = themis_secure_message_unwrap(crypter->priv_, crypter->priv_length_, public_key, public_key_length, encrypted_data, encrypted_data_length, data, data_length);
  switch(res){
  case THEMIS_BUFFER_TOO_SMALL:
    return HM_BUFFER_TOO_SMALL;
  case THEMIS_SUCCESS:
    return HM_SUCCESS;
  }
  return HM_FAIL;  
}

int hm_crypter_impl_encrypt_with_token(hm_crypter_impl_t* crypter,
                                       const uint8_t* public_key,
                                       const size_t public_key_length,
                                       const uint8_t* token,
                                       const size_t token_length,
                                       const uint8_t* data,
                                       const size_t data_length,
                                       uint8_t* encrypted_data,
                                       size_t* encrypted_data_length){
  if(!crypter || !(crypter->priv_) || !public_key || !public_key_length || !token || !token_length || !data || !data_length){
    return HM_INVALID_PARAMETER;
  }
  uint8_t token_key[HM_TOKEN_LENGTH];
  size_t token_key_length = HM_TOKEN_LENGTH;
  if(THEMIS_SUCCESS != themis_secure_message_unwrap(crypter->priv_, crypter->priv_length_, public_key, public_key_length, token, token_length, token_key, &token_key_length) || token_key_length != HM_TOKEN_LENGTH){
    return HM_INVALID_PARAMETER;
  }
  themis_status_t res = themis_secure_cell_encrypt_seal(token_key, token_key_length, NULL, 0, data, data_length, encrypted_data, encrypted_data_length);
  switch(res){
  case THEMIS_BUFFER_TOO_SMALL:
    return HM_BUFFER_TOO_SMALL;
  case THEMIS_SUCCESS:
    return HM_SUCCESS;
  }
  return HM_FAIL;  
}

int hm_crypter_impl_encrypt_with_creating_token(hm_crypter_impl_t* crypter,
                                                const uint8_t* public_key,
                                                const size_t public_key_length,
                                                const uint8_t* data,
                                                const size_t data_length,
                                                uint8_t* encrypted_data,
                                                size_t* encrypted_data_length,
                                                uint8_t* token,
                                                size_t* token_length){
  if(!crypter || !crypter->priv_ || !public_key || !public_key_length || !data || !data_length){
    return HM_INVALID_PARAMETER;
  }
  uint8_t token_key[HM_TOKEN_LENGTH];
  size_t token_key_length = HM_TOKEN_LENGTH;  
  if(THEMIS_SUCCESS != soter_rand(token_key, token_key_length)){
    return HM_FAIL;
  }
  themis_status_t res=themis_secure_message_wrap(crypter->priv_, crypter->priv_length_, public_key, public_key_length, token_key, token_key_length, token, token_length);
  switch(res){
  case THEMIS_BUFFER_TOO_SMALL:
    themis_secure_cell_encrypt_seal(token_key, token_key_length, NULL, 0, data, data_length, NULL, encrypted_data_length);
    return HM_BUFFER_TOO_SMALL;
  case THEMIS_SUCCESS:
    res = themis_secure_cell_encrypt_seal(token_key, token_key_length, NULL, 0, data, data_length, encrypted_data, encrypted_data_length);
    switch(res){
    case THEMIS_BUFFER_TOO_SMALL:
      return HM_BUFFER_TOO_SMALL;
    case THEMIS_SUCCESS:
      return HM_SUCCESS;
    }
  }
  return HM_FAIL;
}

int hm_crypter_impl_decrypt_with_token(hm_crypter_impl_t* crypter,
                                       const uint8_t* public_key,
                                       const size_t public_key_length,
                                       const uint8_t* token,
                                       const size_t token_length,
                                       const uint8_t* encrypted_data,
                                       const size_t encrypted_data_length,
                                       uint8_t* data, size_t* data_length){
  if(!crypter || !(crypter->priv_) || !public_key || !public_key_length || !token || !token_length || !encrypted_data || !encrypted_data_length){
    return HM_INVALID_PARAMETER;
  }
  uint8_t token_key[HM_TOKEN_LENGTH];
  size_t token_key_length = HM_TOKEN_LENGTH;
  if(THEMIS_SUCCESS != themis_secure_message_unwrap(crypter->priv_, crypter->priv_length_, public_key, public_key_length, token, token_length, token_key, &token_key_length) || token_key_length != HM_TOKEN_LENGTH){
    return HM_INVALID_PARAMETER;
  }
  themis_status_t res = themis_secure_cell_decrypt_seal(token_key, token_key_length, NULL, 0, encrypted_data, encrypted_data_length, data, data_length);
  switch(res){
  case THEMIS_BUFFER_TOO_SMALL:
    return HM_BUFFER_TOO_SMALL;
  case THEMIS_SUCCESS:
    return HM_SUCCESS;
  }
  return HM_FAIL;    
}

int hm_crypter_impl_mac_with_token(hm_crypter_impl_t* crypter,
                                   const uint8_t* public_key,
                                   const size_t public_key_length,
                                   const uint8_t* token,
                                   const size_t token_length,
                                   const uint8_t* data,
                                   const size_t data_length,
                                   uint8_t* mac,
                                   size_t* mac_length){
  if(!crypter || !(crypter->priv_) || !public_key || !public_key_length || !token || !token_length || !data || !data_length){
    return HM_INVALID_PARAMETER;
  }
  uint8_t token_key[HM_TOKEN_LENGTH];
  size_t token_key_length = HM_TOKEN_LENGTH;
  if(THEMIS_SUCCESS != themis_secure_message_unwrap(crypter->priv_, crypter->priv_length_, public_key, public_key_length, token, token_length, token_key, &token_key_length) && token_key_length == HM_TOKEN_LENGTH){
    return HM_INVALID_PARAMETER;
  }
  if(*mac_length<HM_MAC_LENGTH){
    *mac_length=HM_MAC_LENGTH;
    return HM_BUFFER_TOO_SMALL;
  }
  uint8_t* pre_mac=NULL;
  size_t pre_mac_length=0;
  if(THEMIS_BUFFER_TOO_SMALL!=themis_secure_cell_encrypt_context_imprint(token_key, token_key_length, data, data_length, HM_DEFAULT_CTX, HM_DEFAULT_CTX_LENGTH, pre_mac, &pre_mac_length)){
    return HM_FAIL;
  }
  pre_mac=malloc(pre_mac_length);
  if(!pre_mac){
    return HM_BAD_ALLOC;
  }
  if(THEMIS_SUCCESS!=themis_secure_cell_encrypt_context_imprint(token_key, token_key_length, data, data_length, HM_DEFAULT_CTX, HM_DEFAULT_CTX_LENGTH, pre_mac, &pre_mac_length)){
    free(pre_mac);
    return HM_FAIL;
  }
  memcpy(mac, pre_mac+pre_mac_length-HM_MAC_LENGTH, HM_MAC_LENGTH);
  *mac_length = HM_MAC_LENGTH;
  free(pre_mac);
  return HM_SUCCESS;
}

int hm_crypter_impl_mac_with_creating_token(hm_crypter_impl_t* crypter,
                                            const uint8_t* public_key,
                                            const size_t public_key_length,
                                            const uint8_t* data,
                                            const size_t data_length,
                                            uint8_t* mac,
                                            size_t* mac_length,
                                            uint8_t* token,
                                            size_t* token_length){
  if(!crypter || !(crypter->priv_) || !public_key || !public_key_length || !data || !data_length){
    return HM_INVALID_PARAMETER;
  }
  uint8_t token_key[HM_TOKEN_LENGTH];
  size_t token_key_length = HM_TOKEN_LENGTH;
  if(THEMIS_SUCCESS != soter_rand(token_key, token_key_length)){
    return HM_FAIL;
  }
  themis_status_t res=themis_secure_message_wrap(crypter->priv_, crypter->priv_length_, public_key, public_key_length, token_key, token_key_length, token, token_length);
  switch(res){
  case THEMIS_BUFFER_TOO_SMALL:
    *mac_length = HM_MAC_LENGTH;
    return HM_BUFFER_TOO_SMALL;
  case THEMIS_SUCCESS:
    return hm_crypter_impl_mac_with_token(crypter, public_key, public_key_length, token, *token_length, data, data_length, mac, mac_length);
  }
  return HM_FAIL;
}

int hm_crypter_impl_create_token_from_token(hm_crypter_impl_t* crypter,
                                            const uint8_t* public_key,
                                            const size_t public_key_length,
                                            const uint8_t* new_public_key,
                                            const size_t new_public_key_length,
                                            const uint8_t* token,
                                            const size_t token_length,
                                            uint8_t* new_token,
                                            size_t* new_token_length){
  if(!crypter || !crypter->priv_ || !public_key || !public_key_length || !new_public_key || !new_public_key_length || !token || !token_length){
    return HM_INVALID_PARAMETER;
  }
  uint8_t token_key[HM_TOKEN_LENGTH];
  size_t token_key_length = HM_TOKEN_LENGTH;
  if(THEMIS_SUCCESS != themis_secure_message_unwrap(crypter->priv_, crypter->priv_length_, public_key, public_key_length, token, token_length, token_key, &token_key_length) && token_key_length == HM_TOKEN_LENGTH){
    return HM_INVALID_PARAMETER;
  }
  themis_status_t res = themis_secure_message_wrap(crypter->priv_, crypter->priv_length_, new_public_key, new_public_key_length, token_key, token_key_length, new_token, new_token_length);
  switch(res){
  case THEMIS_BUFFER_TOO_SMALL:
    return HM_BUFFER_TOO_SMALL;
  case THEMIS_SUCCESS:
    return HM_SUCCESS;
  }
  return HM_FAIL;
}

int hm_crypter_impl_sign(hm_crypter_impl_t* crypter,
                         const uint8_t* data,
                         const size_t data_length,
                         uint8_t* signed_data,
                         size_t* signed_data_length){
  return HM_NOT_IMPLEMENTED;
}

int hm_crypter_impl_verify(hm_crypter_impl_t* crypter,
                           const uint8_t* public_key,
                           const size_t public_key_length,
                           const uint8_t* signed_data,
                           const size_t signed_data_length,
                           uint8_t* data,
                           size_t* data_length){
  return HM_NOT_IMPLEMENTED;
}
