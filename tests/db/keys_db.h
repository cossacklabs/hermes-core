/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef DB_KEYS_DB_H_
#define DB_KEYS_DB_H_

#include <common/config.h>
#include "db.h"

typedef struct keys_db_t_ keys_db_t;

keys_db_t* keys_db_create(const char* endpoint, const char* db_name);
db_status_t keys_db_destroy(keys_db_t* db);

db_status_t keys_db_get_key(keys_db_t* db, const char* doc_id, const char* user_id, uint8_t** key, size_t* key_length, uint8_t** owner_pub_key, size_t* owner_pub_key_length, int is_update);
db_status_t keys_db_add_key(keys_db_t* db, const char* doc_id, const char* user_id, const uint8_t* key, const size_t key_length, const uint8_t* owner_pub_key, const size_t owner_pub_key_length, int is_update);
db_status_t keys_db_del_key(keys_db_t* db, const char* doc_id, const char* user_id, int is_update);

#endif /* DB_KEYS_DB_H_ */
