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

#include "crypt_impl_heap_uni.h"

int hm_crypter_impl_uni_encrypt(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *data, const size_t data_length,
        const uint8_t *unused, const size_t unused_l,
        uint8_t **encrypted_data, size_t *encrypted_data_length,
        uint8_t **unused1, size_t *unused1_l) {
    *unused1 = NULL;
    *unused1_l = 0;
    return hm_crypter_impl_encrypt_(
            crypter, public_key, public_key_length, data, data_length, encrypted_data, encrypted_data_length);
}

int hm_crypter_impl_uni_decrypt(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *encrypted_data, const size_t encrypted_data_length,
        const uint8_t *unused, const size_t unused_l,
        uint8_t **data, size_t *data_length,
        uint8_t **unused1, size_t *unused1_l) {
    *unused1 = NULL;
    *unused1_l = 0;
    return hm_crypter_impl_decrypt_(
            crypter, public_key, public_key_length, encrypted_data, encrypted_data_length, data, data_length);
}

int hm_crypter_impl_uni_encrypt_with_token(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *token, const size_t token_length,
        const uint8_t *data, const size_t data_length,
        uint8_t **encrypted_data, size_t *encrypted_data_length,
        uint8_t **unused1, size_t *unused1_l) {
    *unused1 = NULL;
    *unused1_l = 0;
    return hm_crypter_impl_encrypt_with_token_(
            crypter, public_key, public_key_length, token, token_length, data, data_length, encrypted_data,
            encrypted_data_length);
}


int hm_crypter_impl_uni_encrypt_with_creating_token(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *data, const size_t data_length,
        const uint8_t *unused, const size_t unused_l,
        uint8_t **encrypted_data, size_t *encrypted_data_length,
        uint8_t **token, size_t *token_length) {
    return hm_crypter_impl_encrypt_with_creating_token_(
            crypter, public_key, public_key_length, data, data_length, encrypted_data, encrypted_data_length,
            token, token_length);
}

int hm_crypter_impl_uni_decrypt_with_token(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *token, const size_t token_length,
        const uint8_t *encrypted_data, const size_t encrypted_data_length,
        uint8_t **data, size_t *data_length,
        uint8_t **unused, size_t *unused_l) {
    *unused = NULL;
    *unused_l = 0;
    return hm_crypter_impl_decrypt_with_token_(
            crypter, public_key, public_key_length, token, token_length, encrypted_data, encrypted_data_length,
            data, data_length);
}

int hm_crypter_impl_uni_mac_with_token(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *token, const size_t token_length,
        const uint8_t *data, const size_t data_length,
        uint8_t **mac, size_t *mac_length,
        uint8_t **unused, size_t *unused_l) {
    *unused = NULL;
    *unused_l = 0;
    return hm_crypter_impl_mac_with_token_(
            crypter, public_key, public_key_length, token, token_length, data, data_length, mac, mac_length);
}

int hm_crypter_impl_uni_mac_with_creating_token(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *data, const size_t data_length,
        const uint8_t *unused, const size_t unused_l,
        uint8_t **mac, size_t *mac_length,
        uint8_t **token, size_t *token_length) {
    return hm_crypter_impl_mac_with_creating_token_(
            crypter, public_key, public_key_length, data, data_length, mac, mac_length, token, token_length);
}

int hm_crypter_impl_uni_create_token_from_token(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *new_public_key, const size_t new_public_key_length,
        const uint8_t *token, const size_t token_length,
        uint8_t **new_token, size_t *new_token_length,
        uint8_t **unused, size_t *unused_l) {
    *unused = NULL;
    *unused_l = 0;
    return hm_crypter_impl_create_token_from_token_(
            crypter, public_key, public_key_length, new_public_key, new_public_key_length, token, token_length,
            new_token, new_token_length);
}

int hm_crypter_impl_uni_sign(
        hm_crypter_impl_t *crypter,
        const uint8_t *data, const size_t data_length,
        const uint8_t *unused, const size_t unused_l,
        const uint8_t *unused1, const size_t unused1_l,
        uint8_t **signed_data, size_t *signed_data_length,
        uint8_t **unused2, size_t *unused2_l) {
    *unused2 = NULL;
    *unused2_l = 0;
    return hm_crypter_impl_sign_(crypter, data, data_length, signed_data, signed_data_length);
}

int hm_crypter_impl_uni_verify(
        hm_crypter_impl_t *crypter,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *signed_data, const size_t signed_data_length,
        const uint8_t *unused, const size_t unused_l,
        uint8_t **data, size_t *data_length,
        uint8_t **unused2, size_t *unused2_l) {
    *unused2 = NULL;
    *unused2_l = 0;
    return hm_crypter_impl_verify_(
            crypter, public_key, public_key_length, signed_data, signed_data_length, data, data_length);
}


