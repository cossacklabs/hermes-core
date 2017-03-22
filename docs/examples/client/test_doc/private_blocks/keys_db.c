/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#include <bson.h>
#include <mongoc.h>
#include <common/utils.h>
#include "keys_db.h"

struct keys_db_t_{
  mongoc_client_t      *client_;
  mongoc_database_t    *database_;
  mongoc_collection_t  *collection_;
};

keys_db_t* keys_db_create(const char* endpoint, const char* db_name){
  HERMES_CHECK(endpoint && db_name, return NULL);
  keys_db_t* ctx=calloc(1, sizeof(keys_db_t));
  HERMES_CHECK(ctx, return NULL);
  mongoc_init();
  ctx->client_=mongoc_client_new(endpoint);
  HERMES_CHECK(ctx->client_, keys_db_destroy(ctx); return NULL);
  ctx->database_ = mongoc_client_get_database (ctx->client_, db_name);
  HERMES_CHECK(ctx->database_, keys_db_destroy(ctx); return NULL);
  ctx->collection_ = mongoc_client_get_collection(ctx->client_, db_name, "key_service");
  HERMES_CHECK(ctx->collection_, keys_db_destroy(ctx); return NULL);
  return ctx;
}

db_status_t keys_db_destroy(keys_db_t* db){
  HERMES_CHECK(db, return DB_INVALID_PARAM);
  if(db->collection_){
    mongoc_collection_destroy(db->collection_);
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


db_status_t keys_db_get_key(keys_db_t* db, const char* doc_id, const char* user_id, uint8_t** key, size_t* key_length, uint8_t** owner_pub_key, size_t* owner_pub_key_length, int is_update){
  HERMES_CHECK(db && db->collection_, return DB_INVALID_PARAM);
  bson_oid_t doc_id_, user_id_;
  bson_oid_init_from_string (&doc_id_, doc_id);
  bson_oid_init_from_string (&user_id_, user_id);  
  bson_t* q=BCON_NEW("doc_id", BCON_OID(&doc_id_), "user_id", BCON_OID(&user_id_), "access", BCON_INT32(is_update));
  bson_t* p=BCON_NEW("_id", BCON_INT32(0), "key", BCON_INT32(1), "owner_pubkey", BCON_INT32(1));
  mongoc_cursor_t* cursor  = mongoc_collection_find (db->collection_, MONGOC_QUERY_NONE, 0, 0, 0, q, p, NULL);
  HERMES_CHECK(cursor, bson_destroy(q); return DB_FAIL);
  bson_destroy(q);
  const bson_t* cdoc;
  HERMES_CHECK(mongoc_cursor_next(cursor, &cdoc), mongoc_cursor_destroy(cursor); return DB_FAIL);
  bson_iter_t key_iter, pubkey_iter;
  HERMES_CHECK(bson_iter_init(&key_iter, cdoc) && bson_iter_find(&key_iter, "key") && bson_iter_init(&pubkey_iter, cdoc) && bson_iter_find(&pubkey_iter, "owner_pubkey"), mongoc_cursor_destroy(cursor); return DB_FAIL);
  *owner_pub_key_length=bson_iter_value(&pubkey_iter)->value.v_binary.data_len;
  *owner_pub_key=malloc(*owner_pub_key_length);
  memcpy(*owner_pub_key, bson_iter_value(&pubkey_iter)->value.v_binary.data, *owner_pub_key_length);
  *key_length=bson_iter_value(&key_iter)->value.v_binary.data_len;
  *key=malloc(*key_length);
  memcpy(*key, bson_iter_value(&key_iter)->value.v_binary.data, *key_length);
  mongoc_cursor_destroy(cursor);
  return DB_SUCCESS;
}

db_status_t keys_db_add_key(keys_db_t* db, const char* doc_id, const char* user_id, const uint8_t* key, const size_t key_length, const uint8_t* owner_pub_key, const size_t owner_pub_key_length, int is_update){
  HERMES_CHECK(db && db->collection_, return DB_INVALID_PARAM);
  bson_oid_t doc_id_, user_id_;
  bson_oid_init_from_string (&doc_id_, doc_id);
  bson_oid_init_from_string (&user_id_, user_id);  
  bson_t *query = BCON_NEW ("doc_id", BCON_OID(&doc_id_), "user_id", BCON_OID(&user_id_), "access", BCON_INT32(is_update));
  HERMES_CHECK(query, return DB_FAIL);
  bson_t *update = BCON_NEW("$set", "{", "key", BCON_BIN(0, key, key_length), "owner_pubkey", BCON_BIN(0, owner_pub_key, owner_pub_key_length), "}");
  HERMES_CHECK(update, bson_destroy(query); return DB_FAIL);
  mongoc_find_and_modify_opts_t *opts=mongoc_find_and_modify_opts_new();
  HERMES_CHECK(opts, bson_destroy(query); bson_destroy(update); return DB_FAIL);  
  mongoc_find_and_modify_opts_set_update (opts, update);
  mongoc_find_and_modify_opts_set_flags (opts, MONGOC_FIND_AND_MODIFY_UPSERT|MONGOC_FIND_AND_MODIFY_RETURN_NEW);
  HERMES_CHECK(query && update && mongoc_collection_find_and_modify_with_opts (db->collection_, query, opts, NULL, NULL), bson_destroy(query); bson_destroy(update); mongoc_find_and_modify_opts_destroy (opts); return DB_FAIL);
  //  HERMES_CHECK(query && mongoc_collection_insert (db->collection_, MONGOC_INSERT_NONE, query, NULL, NULL), bson_destroy(query); return DB_FAIL);
  bson_destroy(query);
  bson_destroy(update);
  mongoc_find_and_modify_opts_destroy(opts);
  return DB_SUCCESS;
}

db_status_t keys_db_del_key(keys_db_t* db, const char* doc_id, const char* user_id, int is_update){
  HERMES_CHECK(db && db->collection_, return DB_INVALID_PARAM);
  bson_oid_t doc_id_, user_id_;
  bson_oid_init_from_string (&doc_id_, doc_id);
  bson_oid_init_from_string (&user_id_, user_id);  
  bson_t *query = BCON_NEW ("doc_id", BCON_OID(&doc_id_), "user_id", BCON_OID(&user_id_), "access", BCON_INT32(is_update));
  HERMES_CHECK(query && mongoc_collection_remove (db->collection_, MONGOC_REMOVE_NONE, query, NULL, NULL), bson_destroy(query); return DB_FAIL);
  bson_destroy(query);
  return DB_SUCCESS;
}

