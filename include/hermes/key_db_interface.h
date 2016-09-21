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

#ifndef DB_KEYS_DB_H_
#define DB_KEYS_DB_H_

#include "config.h"
#include "db.h"


typedef struct keys_db_t_ keys_db_t;

keys_db_t* keys_db_create(const char* endpoint, const char* db_name);
db_status_t keys_db_destroy(keys_db_t* db);

db_status_t keys_db_get_key(keys_db_t* db, const char* doc_id, const char* user_id, uint8_t** key, size_t* key_length, uint8_t** owner_pub_key, size_t* owner_pub_key_length, int is_update);
db_status_t keys_db_add_key(keys_db_t* db, const char* doc_id, const char* user_id, const uint8_t* key, const size_t key_length, const uint8_t* owner_pub_key, const size_t owner_pub_key_length, int is_update);
db_status_t keys_db_del_key(keys_db_t* db, const char* doc_id, const char* user_id, int is_update);

#endif /* DB_KEYS_DB_H_ */
