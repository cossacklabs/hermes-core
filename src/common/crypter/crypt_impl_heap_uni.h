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

#ifndef CRYPTER_CRYPT_IMPL_HEAP_UNI_H_
#define CRYPTER_CRYPT_IMPL_HEAP_UNI_H_

#include "crypt_impl_heap.h"

typedef int (*crypter_impl_func)(
        hm_crypter_impl_t *crypter,
        const uint8_t *param1, const size_t param1_length,
        const uint8_t *param2, const size_t param2_length,
        const uint8_t *param3, const size_t param3_length,
        uint8_t **out_param1, size_t *out_param1_length,
        uint8_t **out_param2, size_t *out_param2_length);

int hm_crypter_impl_uni_encrypt(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *data, const size_t data_length,
        const uint8_t *unused, const size_t unused_l,
        uint8_t **encrypted_data, size_t *encrypted_data_length,
        uint8_t **unused1, size_t *unused1_l);

int hm_crypter_impl_uni_decrypt(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *encrypted_data, const size_t encrypted_data_length,
        const uint8_t *unused, const size_t unused_l,
        uint8_t **data, size_t *data_length,
        uint8_t **unused1, size_t *unused1_l);

int hm_crypter_impl_uni_encrypt_with_token(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *token, const size_t token_length,
        const uint8_t *data, const size_t data_length,
        uint8_t **encrypted_data, size_t *encrypted_data_length,
        uint8_t **unused1, size_t *unused1_l);


int hm_crypter_impl_uni_encrypt_with_creating_token(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *data, const size_t data_length,
        const uint8_t *unused, const size_t unused_l,
        uint8_t **encrypted_data, size_t *encrypted_data_length,
        uint8_t **token, size_t *token_length);

int hm_crypter_impl_uni_decrypt_with_token(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *token, const size_t token_length,
        const uint8_t *encrypted_data, const size_t encrypted_data_length,
        uint8_t **data, size_t *data_length,
        uint8_t **unused, size_t *unused_l);

int hm_crypter_impl_uni_mac_with_token(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *token, const size_t token_length,
        const uint8_t *data, const size_t data_length,
        uint8_t **mac, size_t *mac_length,
        uint8_t **unused, size_t *unused_l);

int hm_crypter_impl_uni_mac_with_creating_token(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *data, const size_t data_length,
        const uint8_t *unused, const size_t unused_l,
        uint8_t **mac, size_t *mac_length,
        uint8_t **token, size_t *token_length);

int hm_crypter_impl_uni_create_token_from_token(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *new_public_key, const size_t new_public_key_length,
        const uint8_t *token, const size_t token_length,
        uint8_t **new_token, size_t *new_token_length,
        uint8_t **unused, size_t *unused_l);

int hm_crypter_impl_uni_sign(
        hm_crypter_impl_t *crypter,
        const uint8_t *data, const size_t data_length,
        const uint8_t *unused, const size_t unused_l,
        const uint8_t *unused1, const size_t unused1_l,
        uint8_t **signed_data, size_t *signed_data_length,
        uint8_t **unused2, size_t *unused2_l);

int hm_crypter_impl_uni_verify(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *signed_data, const size_t signed_data_length,
        const uint8_t *unused, const size_t unused_l,
        uint8_t **data, size_t *data_length,
        uint8_t **unused2, size_t *unused2_l);

#endif /* CRYPTER_CRYPT_IMPL_HEAP_UNI_H_ */
