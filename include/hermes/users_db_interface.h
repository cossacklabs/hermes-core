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

#ifndef DB_USERS_DB_H_
#define DB_USERS_DB_H_

#include "config.h"
#include "buffer.h"
#include "db.h"

typedef struct users_db_t_ users_db_t;

users_db_t* users_db_create(const char* endpoint, const char* db_name);
db_status_t users_db_destroy(users_db_t* db);

db_status_t users_db_get_pub_key(users_db_t* db, const char* user_id, buffer_t* res);
db_status_t users_db_get_users_list(users_db_t* db, buffer_t* res);

#endif /* DB_USERS_DB_H_ */
