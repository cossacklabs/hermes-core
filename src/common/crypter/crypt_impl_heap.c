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

#include <hermes/common/errors.h>
#include "crypt_impl.h"
#include <stdlib.h>

int hm_crypter_impl_encrypt_(hm_crypter_impl_t* crypter,
                             const uint8_t* public_key,
                             const size_t public_key_length,
                             const uint8_t* data,
                             const size_t data_length,
                             uint8_t** encrypted_data,
                             size_t* encrypted_data_length){
  size_t enc_len=0;
  int res = hm_crypter_impl_encrypt(crypter, public_key, public_key_length, data, data_length, NULL, &enc_len);
  if(HM_BUFFER_TOO_SMALL!=res){
    return res;
  }
  *encrypted_data = malloc(enc_len);
  if(!(*encrypted_data)){
    return HM_BAD_ALLOC;
  }
  res = hm_crypter_impl_encrypt(crypter, public_key, public_key_length, data, data_length, *encrypted_data, &enc_len);
  if(HM_SUCCESS!=res){
    free(encrypted_data);
    return res;
  }
  *encrypted_data_length=enc_len;
  return HM_SUCCESS;  
}

int hm_crypter_impl_decrypt_(hm_crypter_impl_t* crypter,
                             const uint8_t* public_key,
                             const size_t public_key_length,
                             const uint8_t* encrypted_data,
                             const size_t encrypted_data_length,
                             uint8_t** data,
                             size_t* data_length){
  size_t len=0;
  int res = hm_crypter_impl_decrypt(crypter, public_key, public_key_length, encrypted_data, encrypted_data_length, NULL, &len);
  if(HM_BUFFER_TOO_SMALL!=res){
    return res;
  }
  *data = malloc(len);
  if(!(*data)){
    return HM_BAD_ALLOC;
  }
  res = hm_crypter_impl_decrypt(crypter, public_key, public_key_length, encrypted_data, encrypted_data_length, *data, &len);
  if(HM_SUCCESS!=res){
    free(*data);
    return res;
  }
  *data_length=len;
  return HM_SUCCESS;
}

int hm_crypter_impl_encrypt_with_token_(hm_crypter_impl_t* crypter,
                                        const uint8_t* public_key,
                                        const size_t public_key_length,
                                        const uint8_t* token,
                                        const size_t token_length,
                                        const uint8_t* data,
                                        const size_t data_length,
                                        uint8_t** encrypted_data,
                                        size_t* encrypted_data_length){
  size_t len=0;
  int res = hm_crypter_impl_encrypt_with_token(crypter, public_key, public_key_length, token, token_length, data, data_length, NULL, &len);
  if(HM_BUFFER_TOO_SMALL!=res){
    return res;
  }
  *encrypted_data = malloc(len);
  if(!(*encrypted_data)){
    return HM_BAD_ALLOC;
  }
  res = hm_crypter_impl_encrypt_with_token(crypter, public_key, public_key_length, token, token_length, data, data_length, *encrypted_data, &len);
  if(HM_SUCCESS!=res){
    free(*encrypted_data);
    return res;
  }
  *encrypted_data_length=len;
  return HM_SUCCESS;
}

int hm_crypter_impl_encrypt_with_creating_token_(hm_crypter_impl_t* crypter,
                                                 const uint8_t* public_key,
                                                 const size_t public_key_length,
                                                 const uint8_t* data,
                                                 const size_t data_length,
                                                 uint8_t** encrypted_data,
                                                 size_t* encrypted_data_length,
                                                 uint8_t** token,
                                                 size_t* token_length){
  size_t len=0, token_len=0;
  int res = hm_crypter_impl_encrypt_with_creating_token(crypter, public_key, public_key_length, data, data_length, NULL, &len, NULL, &token_len);
  if(HM_BUFFER_TOO_SMALL!=res){
    return res;
  }
  *encrypted_data = malloc(len+token_len);
  if(!(*encrypted_data)){
    return HM_BAD_ALLOC;
  }
  *token=*encrypted_data+len;
  res = hm_crypter_impl_encrypt_with_creating_token(crypter, public_key, public_key_length, data, data_length, *encrypted_data, &len, *token, &token_len);
  if(HM_SUCCESS!=res){
    free(*encrypted_data);
    return res;
  }
  *encrypted_data_length=len;
  *token_length=token_len;
  return HM_SUCCESS;
}

int hm_crypter_impl_decrypt_with_token_(hm_crypter_impl_t* crypter,
                                        const uint8_t* public_key,
                                        const size_t public_key_length,
                                        const uint8_t* token,
                                        const size_t token_length,
                                        const uint8_t* encrypted_data,
                                        const size_t encrypted_data_length,
                                        uint8_t** data,
                                        size_t* data_length){
  size_t len=0;
  int res = hm_crypter_impl_decrypt_with_token(crypter, public_key, public_key_length, token, token_length, encrypted_data, encrypted_data_length, NULL, &len);
  if(HM_BUFFER_TOO_SMALL!=res){
    return res;
  }
  *data = malloc(len);
  if(!(*data)){
    return HM_BAD_ALLOC;
  }
  res = hm_crypter_impl_decrypt_with_token(crypter, public_key, public_key_length, token, token_length, encrypted_data, encrypted_data_length, *data, &len);
  if(HM_SUCCESS!=res){
    free(*data);
    return res;
  }
  *data_length=len;
  return HM_SUCCESS;
}

int hm_crypter_impl_mac_with_token_(hm_crypter_impl_t* crypter,
                                    const uint8_t* public_key,
                                    const size_t public_key_length,
                                    const uint8_t* token,
                                    const size_t token_length,
                                    const uint8_t* data,
                                    const size_t data_length,
                                    uint8_t** mac,
                                    size_t* mac_length){
  size_t mac_len=0;
  int res = hm_crypter_impl_mac_with_token(crypter, public_key, public_key_length, token, token_length, data, data_length, NULL, &mac_len);
  if(HM_BUFFER_TOO_SMALL!=res){
    return res;
  }
  *mac=malloc(mac_len);
  if(!(*mac)){
    return HM_BAD_ALLOC;
  }
  res = hm_crypter_impl_mac_with_token(crypter, public_key, public_key_length, token, token_length, data, data_length, *mac, &mac_len);
  if(HM_SUCCESS!=res){
    return res;
  }
  *mac_length=mac_len;
  return HM_SUCCESS;
}

int hm_crypter_impl_mac_with_creating_token_(hm_crypter_impl_t* crypter,
                                             const uint8_t* public_key,
                                             const size_t public_key_length,
                                             const uint8_t* data,
                                             const size_t data_length,
                                             uint8_t** mac,
                                             size_t* mac_length,
                                             uint8_t** token,
                                             size_t* token_length){
  size_t len=0, token_len=0;
  int res = hm_crypter_impl_mac_with_creating_token(crypter, public_key, public_key_length, data, data_length, NULL, &len, NULL, &token_len);
  if(HM_BUFFER_TOO_SMALL!=res){
    return res;
  }
  *mac = malloc(len+token_len);
  if(!(*mac)){
    return HM_BAD_ALLOC;
  }
  *token=*mac+len;
  res = hm_crypter_impl_mac_with_creating_token(crypter, public_key, public_key_length, data, data_length, *mac, &len, *token, &token_len);
  if(HM_SUCCESS!=res){
    free(*mac);
    return res;
  }
  *mac_length=len;
  *token_length=token_len;
  return HM_SUCCESS;
}

int hm_crypter_impl_create_token_from_token_(hm_crypter_impl_t* crypter,
                                             const uint8_t* public_key,
                                             const size_t public_key_length,
                                             const uint8_t* new_public_key,
                                             const size_t new_public_key_length,
                                             const uint8_t* token,
                                             const size_t token_length,
                                             uint8_t** new_token,
                                             size_t* new_token_length){
  size_t new_token_len=0;
  int res=hm_crypter_impl_create_token_from_token(crypter, public_key, public_key_length, new_public_key, new_public_key_length, token, token_length, NULL, &new_token_len);
  if(HM_BUFFER_TOO_SMALL!=res){
    return res;
  }
  *new_token = malloc(new_token_len);
  if(!(*new_token)){
    return HM_BAD_ALLOC;
  }
  res=hm_crypter_impl_create_token_from_token(crypter, public_key, public_key_length, new_public_key, new_public_key_length, token, token_length, *new_token, &new_token_len);
  if(HM_SUCCESS!=res){
    free(*new_token);
    return res;
  }
  *new_token_length=new_token_len;
  return HM_SUCCESS;
}
int hm_crypter_impl_sign_(hm_crypter_impl_t* crypter,
                          const uint8_t* data,
                          const size_t data_length,
                          uint8_t** signed_data,
                          size_t* signed_data_length){
  size_t len=0;
  int res=hm_crypter_impl_sign(crypter, data, data_length, NULL, &len);
  if(HM_BUFFER_TOO_SMALL!=res){
    return res;
  }
  *signed_data=malloc(len);
  if(!(*signed_data)){
    return HM_BAD_ALLOC;
  }
  res=hm_crypter_impl_sign(crypter, data, data_length, *signed_data, &len);
  if(HM_SUCCESS!=res){
    free(*signed_data);
    return res;
  }
  *signed_data_length=len;
  return HM_SUCCESS;
}

int hm_crypter_impl_verify_(hm_crypter_impl_t* crypter,
                            const uint8_t* public_key,
                            const size_t public_key_length,
                            const uint8_t* signed_data,
                            const size_t signed_data_length,
                            uint8_t** data,
                            size_t* data_length){
  size_t len=0;
  int res=hm_crypter_impl_verify(crypter, public_key, public_key_length, signed_data, signed_data_length, NULL, &len);
  if(HM_BUFFER_TOO_SMALL!=res){
    return res;
  }
  *data=malloc(len);
  if(!(*data)){
    return HM_BAD_ALLOC;
  }
  res=hm_crypter_impl_verify(crypter, public_key, public_key_length, signed_data, signed_data_length, *data, &len);
  if(HM_SUCCESS!=res){
    free(*data);
    return res;
  }
  *data_length=len;
  return HM_SUCCESS;
}
