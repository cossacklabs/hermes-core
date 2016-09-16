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

#ifndef HERMES_HERMES_DOCUMENT_BLOCK_H_
#define HERMES_HERMES_DOCUMENT_BLOCK_H_

#include <hermes/crypt.h>
#include <stdbool.h>

struct hermes_document_block_t_{
  bool is_loaded;
  bool is_changed;
  bool is_private;
  char* id;
  char* doc_id;
  char* name;
  uint8_t *mac;
  size_t mac_length;
  uint8_t *new_mac;
  size_t new_mac_length;
  uint8_t* data;
  size_t data_length;
  uint8_t* read_token;
  size_t read_token_length;
  uint8_t* update_token;
  size_t update_token_length;
  char* owner_id;
  char* user_id;
};

typedef struct hermes_document_block_t_ hermes_document_block_t;

hermes_document_block_t* hermes_document_block_create_new(const char* doc_id, const char* name, const bool is_private, const uint8_t* data, const size_t data_length);
hermes_document_block_t* hermes_document_block_create(const char* owner_id, const char* doc_id, const char* name, const bool is_private, const uint8_t* data, const size_t data_length);
hermes_document_block_t* hermes_document_block_create_from_blob(const uint8_t* blob, const size_t blob_length);
void hermes_document_block_destroy(hermes_document_block_t** block);
int hermes_document_block_set_data(hermes_document_block_t* block, const uint8_t* data, const size_t data_length);

int hermes_document_block_to_blob(hermes_document_block_t* block, uint8_t** blob, size_t* blob_length);

#endif /* HERMES_DOCUMENT_BLOCK_H_ */
