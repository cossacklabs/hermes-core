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

#ifndef HERMES_HERMES_USER_H_
#define HERMES_HERMES_USER_H_

#include <hermes/crypt.h>
#include <hermes/hermes_storages.h>
#include <hermes/hermes_document_block.h>

#include <stdbool.h>

#define HERMES_UPDATE_ACCESS_MASK 1
#define HERMES_READ_ACCESS_MASK 2
#define HERMES_NO_ACCESS_MASK 0

typedef struct hermes_user_t_ hermes_user_t;

hermes_user_t* hermes_user_create(const char* user_id, const uint8_t* private_key, const size_t private_key_length, hermes_storages_t* storages);

void hermes_user_destroy(hermes_user_t** user);

int hermes_user_save_block(hermes_user_t* user, hermes_document_block_t* block);
int hermes_user_load_block(hermes_user_t* user, const char* doc_id, const char* block_id, hermes_document_block_t** block);
int hermes_user_delete_block(hermes_user_t* user, hermes_document_block_t* block);

int hermes_user_grand_access_to_block(hermes_user_t* user, const char* user_id_granting_access_to, hermes_document_block_t* block, int rights_mask);
int hermes_user_deny_access_to_block(hermes_user_t* user, const char* user_id_denying_access_to, hermes_document_block_t* block);

#endif /* HERMES_HERMES_USER_H_ */
  
