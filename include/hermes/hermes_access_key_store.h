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

#ifndef HERMES_HERMES_ACCESS_KEY_STORE_H_
#define HERMES_HERMES_ACCESS_KEY_STORE_H_

#include <stdint.h>
#include <stdlib.h>

typedef struct hermes_access_key_store_t_ hermes_access_key_store_t;

hermes_access_key_store_t* hermes_access_key_store_create();
void hermes_access_key_store_destroy(hermes_access_key_store_t** store);

int hermes_access_key_store_set_read_token(hermes_access_key_store_t* store, const char* doc_id, const char* block_id, const char* user_id, const uint8_t* token, const size_t token_length);
int hermes_access_key_store_set_read_token(hermes_access_key_store_t* store, const char* doc_id, const char* block_id, const char* user_id, const uint8_t* token, const size_t token_length);

int hermes_access_key_store_get_read_token(hermes_access_key_store_t* store, const char* doc_id, const char* block_id, const char* user_id, uint8_t** token, size_t* token_length);
int hermes_access_key_store_get_read_token(hermes_access_key_store_t* store, const char* doc_id, const char* block_id, const char* user_id, uint8_t** token, size_t* token_length);



#endif /* HERMES_HERMES_ACCESS_KEY_STORE_H_ */
