/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#include <bson.h>
#include <mongoc.h>
#include <common/utils.h>
#include "docs_db.h"

struct docs_db_t_{
  mongoc_client_t      *client_;
  mongoc_database_t    *database_;
  mongoc_collection_t  *docs_collection_;
  mongoc_collection_t  *private_blocks_collection_;
  mongoc_collection_t  *public_blocks_collection_;
};

docs_db_t* docs_db_create(const char* endpoint, const char* db_name){
  HERMES_CHECK(endpoint && db_name, return NULL);
  docs_db_t* ctx=calloc(1, sizeof(docs_db_t));
  HERMES_CHECK(ctx, return NULL);
  mongoc_init();
  ctx->client_=mongoc_client_new(endpoint);
  HERMES_CHECK(ctx->client_, docs_db_destroy(ctx); return NULL);
  ctx->database_ = mongoc_client_get_database (ctx->client_, db_name);
  HERMES_CHECK(ctx->database_, docs_db_destroy(ctx); return NULL);
  ctx->docs_collection_ = mongoc_client_get_collection(ctx->client_, db_name, "record_set_docs");
  HERMES_CHECK(ctx->docs_collection_, docs_db_destroy(ctx); return NULL);
  ctx->private_blocks_collection_ = mongoc_client_get_collection(ctx->client_, db_name, "record_set_private_blocks");
  HERMES_CHECK(ctx->private_blocks_collection_, docs_db_destroy(ctx); return NULL);
  ctx->public_blocks_collection_ = mongoc_client_get_collection(ctx->client_, db_name, "record_set_public_blocks");
  HERMES_CHECK(ctx->public_blocks_collection_, docs_db_destroy(ctx); return NULL);
  return ctx;
}

db_status_t docs_db_destroy(docs_db_t* db){
  HERMES_CHECK(db, return DB_INVALID_PARAM);
  if(db->docs_collection_){
    mongoc_collection_destroy(db->docs_collection_);
  }
  if(db->private_blocks_collection_){
    mongoc_collection_destroy(db->private_blocks_collection_);
  }
  if(db->public_blocks_collection_){
    mongoc_collection_destroy(db->public_blocks_collection_);
  }
  if(db->database_){
    mongoc_database_destroy(db->database_);
  }
  if(db->client_){
    mongoc_client_destroy(db->client_);
  }
  mongoc_cleanup();
  free(db);
  return DB_SUCCESS;
}

db_status_t docs_db_get_blocks_ids_(docs_db_t* db, const bson_oid_t* id, bson_t** result, int private){
  bson_error_t error;
  bson_t *query = BCON_NEW("doc_id", BCON_OID(id));
  bson_t *cmd = BCON_NEW ("distinct", BCON_UTF8 (private?"record_set_private_blocks":"record_set_public_blocks"), "key", BCON_UTF8 ("_id"), "query" , BCON_DOCUMENT(query));
  bson_t res;
  HERMES_CHECK(mongoc_collection_command_simple (private?db->private_blocks_collection_:db->public_blocks_collection_, cmd, NULL, &res, &error), bson_destroy(query); bson_destroy(cmd); return DB_FAIL);
  bson_destroy(query);
  bson_destroy(cmd);
  bson_iter_t iter;
  HERMES_CHECK(bson_iter_init(&iter, &res) && bson_iter_find(&iter, "values"), bson_destroy(&res); return DB_FAIL);
  const uint8_t* val;
  uint32_t val_len;
  HERMES_CHECK(bson_iter_type (&iter) == BSON_TYPE_ARRAY, bson_destroy(&res); return DB_FAIL);
  bson_iter_array(&iter, &val_len, &val);
  *result=bson_new_from_data(val, val_len);
  bson_destroy(&res);
  return DB_SUCCESS;
}

db_status_t docs_db_is_doc_exist_(docs_db_t* db, const bson_oid_t* doc_id){
  bson_t *query = BCON_NEW("_id", BCON_OID(doc_id));  
  int64_t count = mongoc_collection_count (db->docs_collection_, MONGOC_QUERY_NONE, query, 0, 0, NULL, NULL);
  bson_destroy(query);
  HERMES_CHECK(count>0, return DB_FAIL);
  return DB_SUCCESS;
}

db_status_t docs_db_is_block_exist_(docs_db_t* db, const bson_oid_t* id, int is_private){
  bson_t *query = BCON_NEW("_id", BCON_OID(id));  
  int64_t count = mongoc_collection_count (is_private?db->private_blocks_collection_:db->public_blocks_collection_, MONGOC_QUERY_NONE, query, 0, 0, NULL, NULL);
  bson_destroy(query);
  HERMES_CHECK(count>0, return DB_FAIL);
  return DB_SUCCESS;
}

db_status_t docs_db_get(docs_db_t* db, const char* doc_id, char** json_doc){
  assert(db && db->docs_collection_);
  /* assert(buffer); */
  bson_oid_t id;
  bson_oid_init_from_string (&id, doc_id);  
  bson_t* q=BCON_NEW("_id", BCON_OID(&id));
  bson_t* p=BCON_NEW("mac", BCON_INT32(0));
  bson_error_t error;
  mongoc_cursor_t* cursor  = mongoc_collection_find (db->docs_collection_, MONGOC_QUERY_NONE, 0, 0, 0, q, p, NULL);
  HERMES_CHECK(cursor, bson_destroy(q); bson_destroy(p); return DB_FAIL);
  bson_destroy(q);
  bson_destroy(p);
  const bson_t* cdoc;
  HERMES_CHECK(mongoc_cursor_next(cursor, &cdoc), mongoc_cursor_destroy(cursor); return DB_FAIL);
  bson_t* doc=bson_copy(cdoc);
  mongoc_cursor_destroy(cursor);
  HERMES_CHECK(doc, return DB_FAIL);
  bson_t* private_blocks;
  bson_t* public_blocks;
  HERMES_CHECK(DB_SUCCESS==docs_db_get_blocks_ids_(db, &id, &private_blocks, 1), bson_destroy(doc); return DB_FAIL);
  HERMES_CHECK(bson_append_array(doc, "private_blocks", -1, private_blocks), bson_destroy(doc); bson_destroy(private_blocks); return DB_FAIL);
  bson_destroy(private_blocks);
  HERMES_CHECK(DB_SUCCESS==docs_db_get_blocks_ids_(db, &id, &public_blocks, 0), bson_destroy(doc); return DB_FAIL);
  HERMES_CHECK(bson_append_array(doc, "public_blocks", -1, public_blocks), bson_destroy(doc); bson_destroy(public_blocks); return DB_FAIL);
  bson_destroy(public_blocks);
  *json_doc=NULL;
  *json_doc=bson_as_json(doc, NULL);
  HERMES_CHECK(*json_doc, bson_destroy(doc); return DB_FAIL);
  bson_destroy(doc);
  return DB_SUCCESS;
}

db_status_t docs_db_get_data(docs_db_t* db, const char* doc_id, char** json_doc){
  HERMES_CHECK(db && db->docs_collection_ && db->private_blocks_collection_ && db->public_blocks_collection_, return DB_INVALID_PARAM);
  bson_oid_t id;
  bson_oid_init_from_string (&id, doc_id);  
  bson_t* q=BCON_NEW("_id", BCON_OID(&id));
  bson_t* p=BCON_NEW("data", BCON_INT32(1));
  bson_error_t error;
  mongoc_cursor_t* cursor  = mongoc_collection_find (db->docs_collection_, MONGOC_QUERY_NONE, 0, 0, 0, q, p, NULL);
  HERMES_CHECK(cursor, bson_destroy(q); return DB_FAIL);
  bson_destroy(q);
  const bson_t* cdoc;
  HERMES_CHECK(mongoc_cursor_next(cursor, &cdoc), mongoc_cursor_destroy(cursor); return DB_FAIL);
  bson_iter_t iter;
  HERMES_CHECK(bson_iter_init(&iter, cdoc) && bson_iter_find(&iter, "data"), mongoc_cursor_destroy(cursor); return DB_FAIL);
  bson_t* doc=bson_new_from_data(bson_iter_value(&iter)->value.v_doc.data, bson_iter_value(&iter)->value.v_doc.data_len);
  *json_doc=bson_as_json(doc, NULL);
  HERMES_CHECK(*json_doc, bson_destroy(doc); return DB_FAIL);
  bson_destroy(doc);
  mongoc_cursor_destroy(cursor);
  return DB_SUCCESS;
}

db_status_t docs_db_get_docs(docs_db_t* db, buffer_t* buffer){
  assert(buffer);
  assert(db && db->docs_collection_ && buffer);
  bson_t* q=bson_new();
  bson_t* p=BCON_NEW("_id", BCON_INT32(1), "data.name", BCON_INT32(1));
  bson_error_t error;
  mongoc_cursor_t* cursor  = mongoc_collection_find (db->docs_collection_, MONGOC_QUERY_NONE, 0, 0, 0, q, p, NULL);
  HERMES_CHECK(cursor, bson_destroy(q); return DB_FAIL);
  bson_destroy(q);
  const bson_t* cdoc;
  while(mongoc_cursor_next(cursor, &cdoc)){
    bson_iter_t id, data, data_name;
    HERMES_CHECK(bson_iter_init(&id, cdoc) && bson_iter_find(&id, "_id") && bson_iter_init(&data, cdoc) && bson_iter_find(&data, "data"), mongoc_cursor_destroy(cursor); return DB_FAIL);
    char id_str[25];
    bson_oid_to_string (&(bson_iter_value(&id)->value.v_oid), id_str);
    assert(BUFFER_SUCCESS==buffer_push_string(buffer, id_str));
    assert(bson_iter_recurse(&data, &data_name) && bson_iter_find(&data_name, "name"));
    assert(BUFFER_SUCCESS==buffer_push_string(buffer, bson_iter_utf8(&data_name, NULL)));
  }
  mongoc_cursor_destroy(cursor);
  return DB_SUCCESS;  
}

db_status_t docs_db_update(docs_db_t* db, const char* user_id, const char* new_doc_json, const uint8_t* mac, const char* doc_id, const uint8_t* old_mac){
  HERMES_CHECK(db && db->docs_collection_, return DB_INVALID_PARAM);
  bson_oid_t id;
  bson_oid_init_from_string (&id, doc_id);
  bson_t updated_doc;
  HERMES_CHECK(bson_init_from_json(&updated_doc, new_doc_json, -1, NULL), return DB_FAIL);
  bson_t *query = BCON_NEW ("_id", BCON_OID(&id), "mac", BCON_BIN(0, old_mac, HERMES_MAC_LENGTH));
  bson_t *update = BCON_NEW("$set", "{", "data", BCON_DOCUMENT(&updated_doc), "mac", BCON_BIN(0, mac, HERMES_MAC_LENGTH),"}");
  HERMES_CHECK(mongoc_collection_update (db->docs_collection_, MONGOC_UPDATE_NONE, query, update, NULL, NULL), bson_destroy(query); bson_destroy(update); bson_destroy(&updated_doc); return DB_FAIL);
  bson_destroy(query);
  bson_destroy(update);
  bson_destroy(&updated_doc);
  query = BCON_NEW("doc_id", BCON_OID(&id));
  update = BCON_NEW("$set", "{", "mac", BCON_BIN(0, mac, HERMES_MAC_LENGTH),"}");
  HERMES_CHECK(mongoc_collection_update (db->public_blocks_collection_, MONGOC_UPDATE_NONE, query, update, NULL, NULL), bson_destroy(query); bson_destroy(update); return DB_FAIL);
  HERMES_CHECK(mongoc_collection_update (db->private_blocks_collection_, MONGOC_UPDATE_NONE, query, update, NULL, NULL), bson_destroy(query); bson_destroy(update); return DB_FAIL);
  return DB_SUCCESS;
}

db_status_t docs_db_add(docs_db_t* db, const char* user_id, const char* new_doc_json, const uint8_t* mac, const uint8_t* read_token, const size_t read_token_length, const uint8_t* update_token, const size_t update_token_length, const uint8_t* public_key, const size_t public_key_length, char** new_id){
  HERMES_CHECK(db && db->docs_collection_, return DB_INVALID_PARAM);
  bson_oid_t new_oid;
  bson_oid_init(&new_oid, NULL);
  bson_t insert_doc;
  HERMES_CHECK(bson_init_from_json(&insert_doc, new_doc_json, -1, NULL), return DB_FAIL);
  bson_t *query = BCON_NEW ("_id", BCON_OID(&new_oid), "data", BCON_DOCUMENT(&insert_doc), "mac", BCON_BIN(0, mac, HERMES_MAC_LENGTH), "access", "{", user_id, "{", "rt",  BCON_BIN(0, read_token, read_token_length), "ut", BCON_BIN(0, update_token, update_token_length), "pub_key", BCON_BIN(0, public_key, public_key_length), "}", "}");
  bson_error_t error;
  HERMES_CHECK(query && mongoc_collection_insert (db->docs_collection_, MONGOC_INSERT_NONE, query, NULL, &error), bson_destroy(query); bson_destroy(&insert_doc); return DB_FAIL);
  bson_destroy(query);
  bson_destroy(&insert_doc);
  *new_id=malloc(25);
  bson_oid_to_string(&new_oid, *new_id);
  return DB_SUCCESS;
}
db_status_t docs_db_add_access(docs_db_t* db, const char* user_id, const char* doc_id, const uint8_t* mac, const char* new_user_name, const uint8_t* read_token, const size_t read_token_length, const uint8_t* update_token, const size_t update_token_length, const uint8_t* public_key, const size_t public_key_length){
  HERMES_CHECK(db && db->docs_collection_, return DB_INVALID_PARAM);
  bson_oid_t id;
  bson_oid_init_from_string (&id, doc_id);
  bson_t *query = BCON_NEW ("_id", BCON_OID(&id), "mac", BCON_BIN(0, mac, HERMES_MAC_LENGTH));
  char new_filed_name[1024];
  sprintf(new_filed_name, "access.%s", new_user_name);
  bson_t *update = BCON_NEW("$set", "{", new_filed_name, "{", "ut", BCON_BIN(0, read_token, read_token_length), "rt", BCON_BIN(0, update_token, update_token_length), "pub_key", BCON_BIN(0, public_key, public_key_length), "}", "}");
  HERMES_CHECK(mongoc_collection_update (db->docs_collection_, MONGOC_UPDATE_NONE, query, update, NULL, NULL), bson_destroy(query); bson_destroy(update); return DB_FAIL);
  bson_destroy(query);
  bson_destroy(update);
  return DB_SUCCESS;
}

db_status_t docs_db_rm_access(docs_db_t* db, const char* user_id, const char* doc_id, const uint8_t* mac, const char* new_user_name){
  HERMES_CHECK(db && db->docs_collection_, return DB_INVALID_PARAM);
  bson_oid_t id;
  bson_oid_init_from_string (&id, doc_id);
  bson_t *query = BCON_NEW ("_id", BCON_OID(&id), "mac", BCON_BIN(0, mac, HERMES_MAC_LENGTH));
  char new_filed_name[1024];
  sprintf(new_filed_name, "access.%s", new_user_name);
  bson_t *update = BCON_NEW("$unset", "{", new_filed_name, BCON_UTF8(""), "}");
  HERMES_CHECK(mongoc_collection_update (db->docs_collection_, MONGOC_UPDATE_NONE, query, update, NULL, NULL), bson_destroy(query); bson_destroy(update); return DB_FAIL);
  bson_destroy(query);
  bson_destroy(update);
  return DB_SUCCESS;
}


db_status_t docs_db_delete(docs_db_t* db, const char* id, const uint8_t* update_token){
  HERMES_CHECK(db && db->docs_collection_, return DB_INVALID_PARAM);
  bson_oid_t doc_id;
  HERMES_CHECK(bson_oid_is_valid(id, strlen(id)+1), return DB_INVALID_PARAM);
  bson_oid_init_from_string(&doc_id, id);
  bson_t *query = BCON_NEW ("_id", BCON_OID(&doc_id), "ut", BCON_BIN(0, update_token, HERMES_MAC_LENGTH));
  HERMES_CHECK(query && mongoc_collection_remove (db->docs_collection_, MONGOC_REMOVE_NONE, query, NULL, NULL), bson_destroy(query); return DB_FAIL);
  bson_destroy(query);
  return DB_SUCCESS;
}

db_status_t docs_db_add_block(docs_db_t* db, const char* doc_id, const char* name, const uint8_t* new_block, const size_t new_block_length, const uint8_t* mac, int is_private){
  HERMES_CHECK(db && is_private?db->private_blocks_collection_:db->public_blocks_collection_, return DB_INVALID_PARAM);
  bson_oid_t new_oid, doc_id_;
  bson_oid_init(&new_oid, NULL);
  HERMES_CHECK(bson_oid_is_valid(doc_id, strlen(doc_id)+1), return DB_INVALID_PARAM);
  bson_oid_init_from_string(&doc_id_, doc_id);
  bson_t *query = BCON_NEW ("_id", BCON_OID(&new_oid), "doc_id", BCON_OID(&doc_id_), "data", BCON_BIN(0, new_block, new_block_length), "name", BCON_UTF8(name), "mac", BCON_BIN(0, mac, HERMES_MAC_LENGTH));
  HERMES_CHECK(query && mongoc_collection_insert (is_private?db->private_blocks_collection_:db->public_blocks_collection_, MONGOC_INSERT_NONE, query, NULL, NULL), bson_destroy(query); return DB_FAIL);
  bson_destroy(query);
  return DB_SUCCESS;
}

db_status_t docs_db_delete_block(docs_db_t* db, const char* doc_id, const char* block_id, const uint8_t* mac, int is_private){
  HERMES_CHECK(db && is_private?db->private_blocks_collection_:db->public_blocks_collection_, return DB_INVALID_PARAM);
  bson_oid_t doc_id_, block_id_;
  HERMES_CHECK(bson_oid_is_valid(doc_id, strlen(doc_id)+1) && bson_oid_is_valid(block_id, strlen(block_id)+1), return DB_INVALID_PARAM);
  bson_oid_init_from_string(&doc_id_, doc_id);
  bson_oid_init_from_string(&block_id_, block_id);
  bson_t *query = BCON_NEW ("_id", BCON_OID(&block_id_), "doc_id", BCON_OID(&doc_id_), "mac", BCON_BIN(0, mac, HERMES_MAC_LENGTH));
  HERMES_CHECK(query && mongoc_collection_remove (is_private?db->private_blocks_collection_:db->public_blocks_collection_, MONGOC_REMOVE_NONE, query, NULL, NULL), bson_destroy(query); return DB_FAIL);
  bson_destroy(query);
  return DB_SUCCESS; 
}

db_status_t docs_db_update_block(docs_db_t* db, const char* doc_id, const char* block_id, const uint8_t* new_block, const size_t new_block_length, const uint8_t* old_update_token, const uint8_t* new_update_token, int is_private){
  HERMES_CHECK(db && is_private?db->private_blocks_collection_:db->public_blocks_collection_, return DB_INVALID_PARAM);
  bson_oid_t id;
  bson_oid_init_from_string (&id, doc_id);
  HERMES_CHECK(DB_SUCCESS==docs_db_is_block_exist_(db, &id, is_private), return DB_FAIL);
  bson_t *query = BCON_NEW ("_id", BCON_OID(&id), "ut", BCON_BIN(0, old_update_token, HERMES_MAC_LENGTH));
  bson_t *update = BCON_NEW("$set", "{", "data", BCON_BIN(0, new_block, new_block_length), "ut", BCON_BIN(0, new_update_token, HERMES_MAC_LENGTH),"}");
  HERMES_CHECK(mongoc_collection_update (is_private?db->private_blocks_collection_:db->public_blocks_collection_, MONGOC_UPDATE_NONE, query, update, NULL, NULL), bson_destroy(query); bson_destroy(update); return DB_FAIL);
  bson_destroy(query);
  bson_destroy(update);
  return DB_SUCCESS;
}

db_status_t docs_db_get_block(docs_db_t* db, const char* block_id, const char* doc_id, uint8_t** block, size_t* block_length, int is_private){
  HERMES_CHECK(db && is_private?db->private_blocks_collection_:db->public_blocks_collection_, return DB_INVALID_PARAM);
  bson_oid_t doc_id_, block_id_;
  bson_oid_init_from_string (&doc_id_, doc_id);  
  bson_oid_init_from_string (&block_id_, block_id);  
  bson_t* q=BCON_NEW("_id", BCON_OID(&block_id_), "doc_id", BCON_OID(&doc_id_));
  bson_t* p=BCON_NEW("_id", BCON_INT32(0), "data", BCON_INT32(1));
  bson_error_t error;
  mongoc_cursor_t* cursor  = mongoc_collection_find (is_private?db->private_blocks_collection_:db->public_blocks_collection_, MONGOC_QUERY_NONE, 0, 0, 0, q, p, NULL);
  HERMES_CHECK(cursor, bson_destroy(q); bson_destroy(p);return DB_FAIL);
  bson_destroy(q);
  bson_destroy(p);
  const bson_t* cdoc;
  HERMES_CHECK(mongoc_cursor_next(cursor, &cdoc), mongoc_cursor_destroy(cursor); return DB_FAIL);
  bson_iter_t dd;
  HERMES_CHECK(bson_iter_init(&dd, cdoc) && bson_iter_find(&dd, "data"), mongoc_cursor_destroy(cursor); return DB_FAIL);
  *block_length=bson_iter_value(&dd)->value.v_binary.data_len;
  *block=realloc(*block, *block_length);
  memcpy(*block, bson_iter_value(&dd)->value.v_binary.data, *block_length);
  mongoc_cursor_destroy(cursor);
  return DB_SUCCESS;
}


