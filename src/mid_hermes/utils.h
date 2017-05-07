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


#ifndef MID_HERMES_UTILS_H
#define MID_HERMES_UTILS_H

#include <stdint.h>
#include <stdlib.h>

#define HM_TOKEN_LEN 64
#define HM_MAC_LEN 16

uint32_t hm_encrypt(const uint8_t* key, const size_t key_len, const uint8_t* data, const size_t data_len, const uint8_t* context, const size_t context_len, uint8_t** encrypted_data, size_t* encrypted_data_len);
uint32_t hm_decrypt(const uint8_t* key, const size_t key_len, const uint8_t* encrypted_data, const size_t encrypted_data_len, const uint8_t* context, const size_t context_len, uint8_t** data, size_t* data_len);
uint32_t hm_mac_create(const uint8_t* key, const size_t key_len, const uint8_t* data, const size_t data_len, const uint8_t* context, const size_t context_len, uint8_t** mac, size_t* mac_len);
uint32_t hm_asym_encrypt(const uint8_t* sk, const size_t sk_len, const uint8_t* pk, const size_t pk_len, const uint8_t* data, const size_t data_len, uint8_t** encrypted_data, size_t* encrypted_data_len);
uint32_t hm_asym_decrypt(const uint8_t* sk, const size_t sk_len, const uint8_t* pk, const size_t pk_len, const uint8_t* encrypted_data, const size_t encrypted_data_len, uint8_t** data, size_t* data_len);

#endif //MID_HERMES_UTILS_H

