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



#include "utils.h"
#include <hermes/common/errors.h>
#include <themis/themis.h>
#include <assert.h>
#include <string.h>

uint32_t hm_encrypt(
        const uint8_t *key, const size_t key_length,
        const uint8_t *data, const size_t data_length,
        const uint8_t *context, const size_t context_length,
        uint8_t **encrypted_data, size_t *encrypted_data_length) {
    if (!key || !key_length || !data || !data_length || !encrypted_data || !encrypted_data_length) {
        return HM_INVALID_PARAMETER;
    }
    size_t result_buffer_length = 0;
    if (THEMIS_BUFFER_TOO_SMALL != themis_secure_cell_encrypt_seal(
            key, key_length, context, context_length, data, data_length, NULL, &result_buffer_length)) {
        return HM_FAIL;
    }
    *encrypted_data = malloc(result_buffer_length);
    assert(*encrypted_data);
    if (THEMIS_SUCCESS != themis_secure_cell_encrypt_seal(
            key, key_length, context, context_length, data, data_length, *encrypted_data, &result_buffer_length)) {
        free(*encrypted_data);
        *encrypted_data = NULL;
        return HM_FAIL;
    }
    *encrypted_data_length = result_buffer_length;
    return HM_SUCCESS;
}

uint32_t hm_decrypt(
        const uint8_t *key, const size_t key_length,
        const uint8_t *encrypted_data, const size_t encrypted_data_length,
        const uint8_t *context, const size_t context_length,
        uint8_t **data, size_t *data_length) {
    if (!key || !key_length || !data || !data_length || !encrypted_data || !encrypted_data_length) {
        return HM_INVALID_PARAMETER;
    }
    size_t result_buffer_length = 0;
    if (THEMIS_BUFFER_TOO_SMALL != themis_secure_cell_decrypt_seal(
            key, key_length, context, context_length, encrypted_data, encrypted_data_length, NULL,
            &result_buffer_length)) {
        return HM_FAIL;
    }
    *data = malloc(result_buffer_length);
    assert(*data);
    if (THEMIS_SUCCESS != themis_secure_cell_decrypt_seal(
            key, key_length, context, context_length, encrypted_data, encrypted_data_length,
            *data, &result_buffer_length)) {
        free(*data);
        *data = NULL;
        return HM_FAIL;
    }
    *data_length = result_buffer_length;
    return HM_SUCCESS;
}

uint32_t hm_mac_create(
        const uint8_t *key, const size_t key_length,
        const uint8_t *data, const size_t data_length,
        const uint8_t *context, const size_t context_length,
        uint8_t **mac, size_t *mac_length) {
    if (!key || !key_length || !data || !data_length || !mac || !mac_length) {
        return HM_INVALID_PARAMETER;
    }
    soter_hmac_ctx_t* hmac_ctx=soter_hmac_create(SOTER_HASH_SHA512, key, key_length);
    if(!hmac_ctx){
      return HM_FAIL;
    }
    if(SOTER_SUCCESS!=soter_hmac_update(hmac_ctx, data, data_length)){
      soter_hmac_destroy(hmac_ctx);
      return HM_FAIL;
    }
    if(SOTER_SUCCESS!=soter_hmac_update(hmac_ctx, context, context_length)){
      soter_hmac_destroy(hmac_ctx);
      return HM_FAIL;
    }
    size_t mac_len = 0;
    if (SOTER_BUFFER_TOO_SMALL != soter_hmac_final(hmac_ctx, NULL, &mac_len)){
      soter_hmac_destroy(hmac_ctx);
      return HM_FAIL;
    }
    *mac = malloc(mac_len);
    assert(*mac);
    if (SOTER_SUCCESS != soter_hmac_final(hmac_ctx, *mac, &mac_len)){
      soter_hmac_destroy(hmac_ctx);
      return HM_FAIL;
    }
    soter_hmac_destroy(hmac_ctx);
    *mac_length = mac_len;
    return HM_SUCCESS;
}

uint32_t hm_asym_encrypt(
        const uint8_t *private_key, const size_t private_key_length,
        const uint8_t *public_key, const size_t public_key_len,
        const uint8_t *data, const size_t data_length,
        uint8_t **encrypted_data, size_t *encrypted_data_length) {
    if (!private_key || !private_key_length || !public_key || !public_key_len || !data || !data_length ||
        !encrypted_data || !encrypted_data_length) {
        return HM_INVALID_PARAMETER;
    }
    size_t result_buffer_length = 0;
    if (THEMIS_BUFFER_TOO_SMALL != themis_secure_message_wrap(
            private_key, private_key_length, public_key, public_key_len, data, data_length, NULL,
            &result_buffer_length)) {
        return HM_FAIL;
    }
    *encrypted_data = malloc(result_buffer_length);
    assert(*encrypted_data);
    if (THEMIS_SUCCESS != themis_secure_message_wrap(
            private_key, private_key_length, public_key, public_key_len, data, data_length,
            *encrypted_data, &result_buffer_length)) {
        free(*encrypted_data);
        *encrypted_data = NULL;
        return HM_FAIL;
    }
    *encrypted_data_length = result_buffer_length;
    return HM_SUCCESS;
}

uint32_t hm_asym_decrypt(
        const uint8_t *private_key, const size_t private_key_len,
        const uint8_t *public_key, const size_t public_key_len,
        const uint8_t *encrypted_data, const size_t encrypted_data_length,
        uint8_t **data, size_t *data_length) {
    if (!private_key || !private_key_len || !public_key || !public_key_len || !encrypted_data ||
        !encrypted_data_length || !data || !data_length) {
        return HM_INVALID_PARAMETER;
    }
    size_t result_buffer_length = 0;
    if (THEMIS_BUFFER_TOO_SMALL != themis_secure_message_unwrap(
            private_key, private_key_len, public_key, public_key_len, encrypted_data, encrypted_data_length,
            NULL, &result_buffer_length)) {
        return HM_FAIL;
    }
    *data = malloc(result_buffer_length);
    assert(*data);
    if (THEMIS_SUCCESS != themis_secure_message_unwrap(
            private_key, private_key_len, public_key, public_key_len, encrypted_data, encrypted_data_length,
            *data, &result_buffer_length)) {
        memset(data, 0, result_buffer_length);
        free(*data);
        *data = NULL;
        return HM_FAIL;
    }
    *data_length = result_buffer_length;
    return HM_SUCCESS;
}
