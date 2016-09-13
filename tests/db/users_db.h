/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef DB_USERS_DB_H_
#define DB_USERS_DB_H_

#include <common/config.h>
#include <common/buffer.h>
#include "db.h"

typedef struct users_db_t_ users_db_t;

users_db_t* users_db_create(const char* endpoint, const char* db_name);
db_status_t users_db_destroy(users_db_t* db);

db_status_t users_db_get_pub_key(users_db_t* db, const char* user_id, buffer_t* res);
db_status_t users_db_get_users_list(users_db_t* db, buffer_t* res);

#endif /* DB_USERS_DB_H_ */
