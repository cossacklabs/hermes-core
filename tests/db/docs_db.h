/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef DB_DOCS_DB_H_
#define DB_DOCS_DB_H_

#include <common/config.h>
#include <common/buffer.h>
#include "db.h"

typedef struct docs_db_t_ docs_db_t;

docs_db_t* docs_db_create(const char* endpoint, const char* db_name);
db_status_t docs_db_destroy(docs_db_t* db);

db_status_t docs_db_get_docs(docs_db_t* db, buffer_t* buffer);

db_status_t docs_db_get(docs_db_t* db, const char* id, char** doc);
db_status_t docs_db_get_data(docs_db_t* db, const char* id, char** doc);
db_status_t docs_db_update(docs_db_t* db, const char* user_id, const char* new_doc_json, const uint8_t* mac, const char* doc_id, const uint8_t* old_mac);
db_status_t docs_db_add(docs_db_t* db, const char* user_id, const char* new_doc_json, const uint8_t* mac, const uint8_t* read_token, const size_t read_token_length, const uint8_t* update_token, const size_t update_token_length, const uint8_t* public_key, const size_t public_key_length, char** new_id);
db_status_t docs_db_add_access(docs_db_t* db, const char* user_id, const char* doc_id, const uint8_t* mac, const char* new_user_name, const uint8_t* read_token, const size_t read_token_length, const uint8_t* update_token, const size_t update_token_length, const uint8_t* public_key, const size_t public_key_length);
db_status_t docs_db_rm_access(docs_db_t* db, const char* user_id, const char* doc_id, const uint8_t* mac, const char* new_user_name);
db_status_t docs_db_delete(docs_db_t* db, const char* id, const uint8_t* update_token);

db_status_t docs_db_add_block(docs_db_t* db, const char* doc_id, const char* name, const uint8_t* new_block, const size_t new_block_length, const uint8_t* mac, int is_private);
db_status_t docs_db_delete_block(docs_db_t* db, const char* doc_id, const char* block_id, const uint8_t* update_token, int is_private);
db_status_t docs_db_update_block(docs_db_t* db, const char* doc_id, const char* block_id, const uint8_t* new_block, const size_t new_block_length, const uint8_t* old_update_token, const uint8_t* new_update_token, int is_private);
db_status_t docs_db_get_block(docs_db_t* db,  const char* block_id, const char* doc_id, uint8_t** block, size_t* block_length, int is_private);

#endif /* DB_DOCS_DB_H_ */
