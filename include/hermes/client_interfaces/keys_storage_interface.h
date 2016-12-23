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

#ifndef INCLUDE_KEYS_STORAGE_INTERFACE_H_
#define INCLUDE_KEYS_STORAGE_INTERFACE_H_

typedef struct hm_keys_storage_t_ hm_keys_storage_t;

hm_keys_storage_t* hm_keys_storage_create();
int hm_keys_storage_destroy(hm_keys_storage_t* ks);
int hm_keys_storage_get_private_key_by_id(hm_keys_storage_t* ks, const uint8_t* id, const size_t id_kength, uint8_t* key, size_t* key_length);
int hm_keys_storage_get_public_key_by_id(hm_keys_storage_t* ks, const uint8_t* id, const size_t id_kength, uint8_t* key, size_t* key_length);

#endif /* INCLUDE_KEYS_STORAGE_INTERFACE_H_ */
