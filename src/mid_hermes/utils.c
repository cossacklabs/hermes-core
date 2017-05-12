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

#include "utils.h"
#include <hermes/common/errors.h>
#include <themis/themis.h>
#include <assert.h>
#include <string.h> 

uint32_t hm_encrypt(const uint8_t* key, const size_t key_len, const uint8_t* data, const size_t data_len, const uint8_t* context, const size_t context_len, uint8_t** encrypted_data, size_t* encrypted_data_len){
  if(!key || !key_len || !data || !data_len || !encrypted_data || !encrypted_data_len){
    return HM_INVALID_PARAMETER;
  }
  size_t res_buf_len=0;
  if(THEMIS_BUFFER_TOO_SMALL!=themis_secure_cell_encrypt_seal(key, key_len, context, context_len, data, data_len, NULL, &res_buf_len)){
    return HM_FAIL;
  }
  *encrypted_data=malloc(res_buf_len);
  assert(*encrypted_data);
  if(THEMIS_SUCCESS!=themis_secure_cell_encrypt_seal(key, key_len, context, context_len, data, data_len, *encrypted_data, &res_buf_len)){
    free(*encrypted_data);
    return HM_FAIL;
  }
  *encrypted_data_len=res_buf_len;
  return HM_SUCCESS;
}

uint32_t hm_decrypt(const uint8_t* key, const size_t key_len, const uint8_t* encrypted_data, const size_t encrypted_data_len, const uint8_t* context, const size_t context_len, uint8_t** data, size_t* data_len){
  if(!key || !key_len || !data || !data_len || !encrypted_data || !encrypted_data_len){
    return HM_INVALID_PARAMETER;
  }
  size_t res_buf_len=0;
  if(THEMIS_BUFFER_TOO_SMALL!=themis_secure_cell_decrypt_seal(key, key_len, context, context_len, encrypted_data, encrypted_data_len, NULL, &res_buf_len)){
    return HM_FAIL;
  }
  *data=malloc(res_buf_len);
  assert(*data);
  if(THEMIS_SUCCESS!=themis_secure_cell_decrypt_seal(key, key_len, context, context_len, encrypted_data, encrypted_data_len, *data, &res_buf_len)){
    free(*data);
    return HM_FAIL;
  }
  *data_len=res_buf_len;
  return HM_SUCCESS;
}

uint32_t hm_mac_create(const uint8_t* key, const size_t key_len, const uint8_t* data, const size_t data_len, const uint8_t* context, const size_t context_len, uint8_t** mac, size_t* mac_len){
  if(!key || !key_len || !data || !data_len || !mac || !mac_len){
    return HM_INVALID_PARAMETER;
  }
  uint8_t* pre_mac=NULL;
  size_t pre_mac_len=0;
  if(THEMIS_BUFFER_TOO_SMALL!=themis_secure_cell_encrypt_context_imprint(key, key_len, data, data_len, context, context_len, NULL, &pre_mac_len)){
    return HM_FAIL;
  }
  pre_mac=malloc(pre_mac_len);
  assert(pre_mac);
  if(THEMIS_SUCCESS!=themis_secure_cell_encrypt_context_imprint(key, key_len, data, data_len, context, context_len, pre_mac, &pre_mac_len)){
    free(pre_mac);
    return HM_FAIL;
  }
  *mac=malloc(HM_MAC_LEN);
  assert(*mac);
  memcpy(*mac, pre_mac+pre_mac_len-HM_MAC_LEN, HM_MAC_LEN);
  free(pre_mac);
  *mac_len=HM_MAC_LEN;
  return HM_SUCCESS;
}

uint32_t hm_asym_encrypt(const uint8_t* sk, const size_t sk_len, const uint8_t* pk, const size_t pk_len, const uint8_t* data, const size_t data_len, uint8_t** encrypted_data, size_t* encrypted_data_len){
  if(!sk || !sk_len || !pk || !pk_len || !data || !data_len || !encrypted_data || !encrypted_data_len){
    return HM_INVALID_PARAMETER;
  }
  size_t res_buf_len=0;
  if(THEMIS_BUFFER_TOO_SMALL != themis_secure_message_wrap(sk, sk_len, pk, pk_len, data, data_len, NULL, &res_buf_len)){
    return HM_FAIL;
  }
  *encrypted_data=malloc(res_buf_len);
  assert(*encrypted_data);
  if(THEMIS_SUCCESS != themis_secure_message_wrap(sk, sk_len, pk, pk_len, data, data_len, *encrypted_data, &res_buf_len)){
    free(*encrypted_data);
    return HM_FAIL;
  }
  *encrypted_data_len=res_buf_len;
  return HM_SUCCESS;
}

uint32_t hm_asym_decrypt(const uint8_t* sk, const size_t sk_len, const uint8_t* pk, const size_t pk_len, const uint8_t* encrypted_data, const size_t encrypted_data_len, uint8_t** data, size_t* data_len){
  if(!sk || !sk_len || !pk || !pk_len || !encrypted_data || !encrypted_data_len || !data || !data_len){
    return HM_INVALID_PARAMETER;
  }
  size_t res_buf_len=0;
  if(THEMIS_BUFFER_TOO_SMALL != themis_secure_message_unwrap(sk, sk_len, pk, pk_len, encrypted_data, encrypted_data_len, NULL, &res_buf_len)){
    return HM_FAIL;
  }
  *data=malloc(res_buf_len);
  assert(*data);
  if(THEMIS_SUCCESS != themis_secure_message_unwrap(sk, sk_len, pk, pk_len, encrypted_data, encrypted_data_len, *data, &res_buf_len)){
    free(*data);
    return HM_FAIL;
  }
  *data_len=res_buf_len;
  return HM_SUCCESS;  
}
