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

#ifndef HERMES_HERMES_RECORD_SET_STORE_H_
#define HERMES_HERMES_RECORD_SET_STORE_H_

#include <stdint.h>
#include <stdlib.h>

typedef struct hermes_record_set_store_t_ hermes_record_set_store_t;

hermes_record_set_store_t* hermes_record_set_store_create();
void hermes_record_set_store_destroy(hermes_record_set_store_t** store);

int hermes_record_set_set_block(hermes_record_set_store_t* store, const char* doc_id, const char* block_id, const uint8_t* mac, const size_t mac_length, const uint8_t* blob, const size_t blob_length);
int hermes_record_set_set_new_block(hermes_record_set_store_t* store, const char* doc_id, const uint8_t* blob, const size_t blob_length, char** new_block_id);
int hermes_record_set_get_block(hermes_record_set_store_t* store, const char* doc_id, const char* block_id, uint8_t** blob, size_t* blob_length);
int hermes_record_set_delete_block(hermes_record_set_store_t* store, const char* doc_id, const char* block_id, const uint8_t* mac, const size_t mac_length);

#endif /* HERMES_HERMES_RECORD_SET_STORE_H_ */
