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

#ifndef HERMES_CRYPT_H_
#define HERMES_CRYPT_H_

#include <stdint.h>
#include <string.h>
typedef struct hm_crypter_t_ hm_crypter_t;

hm_crypter_t* hm_crypter_create(const uint8_t* id, const size_t id_length);
void hm_crypter_destroy(hm_crypter_t** crypter);

int hm_crypter_encrypt(hm_crypter_t* crypter,
                       const uint8_t* public_key,
                       const size_t public_key_length,
                       const uint8_t* data,
                       const size_t data_length,
                       uint8_t** encrypted_data,
                       size_t* encrypted_data_length);

int hm_crypter_decrypt(hm_crypter_t* crypter,
                       const uint8_t* public_key,
                       const size_t public_key_length,
                       const uint8_t* encrypted_data,
                       const size_t encrypted_data_length,
                       uint8_t** data, size_t* data_length);

int hm_crypter_encrypt_with_token(hm_crypter_t* crypter,
                                  const uint8_t* public_key,
                                  const size_t public_key_length,
                                  const uint8_t* token,
                                  const size_t token_length,
                                  const uint8_t* data,
                                  const size_t data_length,
                                  uint8_t** encrypted_data,
                                  size_t* encrypted_data_length);

int hm_crypter_encrypt_with_creating_token(hm_crypter_t* crypter,
                                           const uint8_t* public_key,
                                           const size_t public_key_length,
                                           const uint8_t* data,
                                           const size_t data_length,
                                           uint8_t** encrypted_data,
                                           size_t* encrypted_data_length,
                                           uint8_t** token,
                                           size_t* token_length);

int hm_crypter_decrypt_with_token(hm_crypter_t* crypter,
                                  const uint8_t* public_key,
                                  const size_t public_key_length,
                                  const uint8_t* token,
                                  const size_t token_length,
                                  const uint8_t* encrypted_data,
                                  const size_t encrypted_data_length,
                                  uint8_t** data, size_t* data_length);

int hm_crypter_mac_with_token(hm_crypter_t* crypter,
                              const uint8_t* public_key,
                              const size_t public_key_length,
                              const uint8_t* token,
                              const size_t token_length,
                              const uint8_t* data,
                              const size_t data_length,
                              uint8_t** mac,
                              size_t* mac_length);

int hm_crypter_mac_with_creating_token(hm_crypter_t* crypter,
                                       const uint8_t* public_key,
                                       const size_t public_key_length,
                                       const uint8_t* data,
                                       const size_t data_length,
                                       uint8_t** mac,
                                       size_t* mac_length,
                                       uint8_t** token,
                                       size_t* token_length);

int hm_crypter_create_token_from_token(hm_crypter_t* crypter,
                                       const uint8_t* public_key,
                                       const size_t public_key_length,
                                       const uint8_t* new_public_key,
                                       const size_t new_public_key_length,
                                       const uint8_t* token,
                                       const size_t token_length,
                                       uint8_t** new_token,
                                       size_t* new_token_length);

int hm_crypter_sign(hm_crypter_t* crypter,
                    const uint8_t* data,
                    const size_t data_length,
                    uint8_t** signed_data,
                    size_t* signed_data_length);

int hm_crypter_verify(hm_crypter_t* crypter,
                      const uint8_t* public_key,
                      const size_t public_key_length,
                      const uint8_t* signed_data,
                      const size_t signed_data_length,
                      uint8_t** data,
                      size_t* data_length);

#endif /* HERMES_CRYPT_H_ */
