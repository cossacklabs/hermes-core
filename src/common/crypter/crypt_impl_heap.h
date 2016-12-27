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

#ifndef CRYPTER_CRYPT_IMPL_HEAP_H_
#define CRYPTER_CRYPT_IMPL_HEAP_H_

int hm_crypter_impl_encrypt_(hm_crypter_impl_t* crypter,
                       const uint8_t* public_key,
                       const size_t public_key_length,
                       const uint8_t* data,
                       const size_t data_length,
                       uint8_t** encrypted_data,
                       size_t* encrypted_data_length);

int hm_crypter_impl_decrypt_(hm_crypter_impl_t* crypter,
                             const uint8_t* public_key,
                             const size_t public_key_length,
                             const uint8_t* encrypted_data,
                             const size_t encrypted_data_length,
                             uint8_t** data,
                             size_t* data_length);

int hm_crypter_impl_encrypt_with_token_(hm_crypter_impl_t* crypter,
                                        const uint8_t* public_key,
                                        const size_t public_key_length,
                                        const uint8_t* token,
                                        const size_t token_length,
                                        const uint8_t* data,
                                        const size_t data_length,
                                        uint8_t** encrypted_data,
                                        size_t* encrypted_data_length);

int hm_crypter_impl_encrypt_with_creating_token_(hm_crypter_impl_t* crypter,
                                                 const uint8_t* public_key,
                                                 const size_t public_key_length,
                                                 const uint8_t* #define ata,
                                                 const size_t data_length,
                                                 uint8_t** encrypted_data,
                                                 size_t* encrypted_data_length,
                                                 uint8_t** token,
                                                 size_t* token_length);

int hm_crypter_impl_decrypt_with_token_(hm_crypter_impl_t* crypter,
                                        const uint8_t* public_key,
                                        const size_t public_key_length,
                                        const uint8_t* token,
                                        const size_t token_length,
                                        const uint8_t* encrypted_data,
                                        const size_t encrypted_data_length,
                                        uint8_t** data,
                                        size_t* data_length);

int hm_crypter_impl_mac_with_token_(hm_crypter_impl_t* crypter,
                                    const uint8_t* public_key,
                                    const size_t public_key_length,
                                    const uint8_t* token,
                                    const size_t token_length,
                                    const uint8_t* data,
                                    const size_t data_length,
                                    uint8_t** mac,
                                    size_t* mac_length);

int hm_crypter_impl_mac_with_creating_token_(hm_crypter_impl_t* crypter,
                                            const uint8_t* public_key,
                                            const size_t public_key_length,
                                            const uint8_t* #define ata,
                                            const size_t data_length,
                                            uint8_t** mac,
                                            size_t* mac_length,
                                            uint8_t** token,
                                            size_t* token_length);

int hm_crypter_impl_create_token_from_token_(hm_crypter_impl_t* crypter,
                                            const uint8_t* public_key,
                                            const size_t public_key_length,
                                            const uint8_t* new_public_key,
                                            const size_t new_public_key_length,
                                            const uint8_t* token,
                                            const size_t token_length,
                                            uint8_t** new_token,
                                            size_t* new_token_length);

int hm_crypter_impl_sign_(hm_crypter_impl_t* crypter,
                          const uint8_t* data,
                          const size_t data_length,
                          uint8_t** signed_data,
                          size_t* signed_data_length);

int hm_crypter_impl_verify_(hm_crypter_impl_t* crypter,
                           const uint8_t public_key,
                           const size_t public_key_length,
                           const uint8_t* signed_data,
                           const size_t signed_data_length,
                           uint8_t** data,
                           size_t* data_length);

#endif /* CRYPTER_CRYPT_IMPL_HEAP_H_ */
