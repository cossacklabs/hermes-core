/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#include <bson.h>
#include <mongoc.h>
#include <common/utils.h>
#include "users_db.h"

struct users_db_t_{
  mongoc_client_t      *client_;
  mongoc_database_t    *database_;
  mongoc_collection_t  *collection_;
};

users_db_t* users_db_create(const char* endpoint, const char* db_name){
  HERMES_CHECK(endpoint && db_name, return NULL);
  users_db_t* ctx=calloc(1, sizeof(users_db_t));
  HERMES_CHECK(ctx, return NULL);
  mongoc_init();
  ctx->client_=mongoc_client_new(endpoint);
  HERMES_CHECK(ctx->client_, users_db_destroy(ctx); return NULL);
  ctx->database_ = mongoc_client_get_database (ctx->client_, db_name);
  HERMES_CHECK(ctx->database_, users_db_destroy(ctx); return NULL);
  ctx->collection_ = mongoc_client_get_collection(ctx->client_, db_name, "users");
  HERMES_CHECK(ctx->collection_, users_db_destroy(ctx); return NULL);
  return ctx;
}

db_status_t users_db_destroy(users_db_t* db){
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


db_status_t users_db_get_pub_key(users_db_t* db, const char* user_id, buffer_t* res){
  assert(res);
  HERMES_CHECK(db && db->collection_, return DB_INVALID_PARAM);
  bson_oid_t doc_id_, user_id_;
  bson_t* q=BCON_NEW("name", BCON_UTF8(user_id));
  bson_t* p=BCON_NEW("_id", BCON_INT32(0), "pub_key", BCON_INT32(1));
  mongoc_cursor_t* cursor  = mongoc_collection_find (db->collection_, MONGOC_QUERY_NONE, 0, 0, 0, q, p, NULL);
  HERMES_CHECK(cursor, bson_destroy(q); return DB_FAIL);
  bson_destroy(q);
  const bson_t* cdoc;
  HERMES_CHECK(mongoc_cursor_next(cursor, &cdoc), mongoc_cursor_destroy(cursor); return DB_FAIL);
  bson_iter_t pubkey_iter;
  HERMES_CHECK(bson_iter_init(&pubkey_iter, cdoc) && bson_iter_find(&pubkey_iter, "pub_key"), mongoc_cursor_destroy(cursor); return DB_FAIL);
  assert(BUFFER_SUCCESS==buffer_push_data(res, bson_iter_value(&pubkey_iter)->value.v_binary.data, bson_iter_value(&pubkey_iter)->value.v_binary.data_len));
  mongoc_cursor_destroy(cursor);
  return DB_SUCCESS;
}

db_status_t users_db_get_users_list(users_db_t* db, buffer_t* buffer){
  assert(buffer);
  assert(db && db->collection_ && buffer);
  bson_t* q=bson_new();
  bson_t* p=BCON_NEW("_id", BCON_INT32(0));
  bson_error_t error;
  mongoc_cursor_t* cursor  = mongoc_collection_find (db->collection_, MONGOC_QUERY_NONE, 0, 0, 0, q, p, NULL);
  HERMES_CHECK(cursor, bson_destroy(q); return DB_FAIL);
  bson_destroy(q);
  const bson_t* cdoc;
  while(mongoc_cursor_next(cursor, &cdoc)){
    char* dd = bson_as_json(cdoc, NULL);
    //    bson_iter_t name, pub_key;
    //    HERMES_CHECK(bson_iter_init(&name, cdoc) && bson_iter_find(&name, "name") && bson_iter_init(&pub_key, cdoc) && bson_iter_find(&pub_key, "pub_key"), mongoc_cursor_destroy(cursor); return DB_FAIL);
    assert(BUFFER_SUCCESS==buffer_push_string(buffer, dd));
    free(dd);
    //    assert(BUFFER_SUCCESS==buffer_push_data(buffer, bson_iter_value(&pub_key)->value.v_binary.data, bson_iter_value(&pub_key)->value.v_binary.data_len));
  }
  mongoc_cursor_destroy(cursor);
  return DB_SUCCESS;  
}


