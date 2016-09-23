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

#include <hermes/utils.h>
#include <hermes/buffer.h>
#include <hermes/key_service_protocol.h>
#include <hermes/key_db_interface.h>

#include <hermes/access_manager_service.h>

#include <string.h>

struct key_service_service_t_{
  key_service_t* ctx_;
  keys_db_t *db_;
  //  db_ctx_t* db_;
};


int key_service_build_error_res(uint8_t** res_buf, size_t* res_buf_length, int32_t error_number, const char* error_message){
  buffer_t* res=buffer_create();
  assert(res);
  assert(BUFFER_SUCCESS==buffer_push_status(res, error_number));
  assert(BUFFER_SUCCESS==buffer_push_string(res, error_message));
  *res_buf = malloc(buffer_get_size(res));
  assert(*res_buf);
  memcpy(*res_buf, buffer_get_data(res), buffer_get_size(res));
  *res_buf_length = buffer_get_size(res);
  buffer_destroy(res);
  return 0;
}

int key_service_build_success_res(uint8_t** res_buf, size_t* res_buf_length, const char* message){
  buffer_t* res=buffer_create();
  assert(res);
  assert(BUFFER_SUCCESS==buffer_push_status(res, 0));
  assert(BUFFER_SUCCESS==buffer_push_string(res, message));
  *res_buf = malloc(buffer_get_size(res));
  assert(*res_buf);
  memcpy(*res_buf, buffer_get_data(res), buffer_get_size(res));
  *res_buf_length = buffer_get_size(res);
  buffer_destroy(res);
  return 0;
}

int key_service_build_success_res_buf(uint8_t** res_buf, size_t* res_buf_length, const uint8_t* message, const size_t message_length){
  buffer_t* res=buffer_create();
  assert(res);
  assert(BUFFER_SUCCESS==buffer_push_status(res, 0));
  assert(BUFFER_SUCCESS==buffer_push_data(res, message, message_length));
  *res_buf = malloc(buffer_get_size(res));
  assert(*res_buf);
  memcpy(*res_buf, buffer_get_data(res), buffer_get_size(res));
  *res_buf_length = buffer_get_size(res);
  buffer_destroy(res);
  return 0;
}


#define RETURN_SUCCESS(res, res_length, mes) key_service_build_success_res(res_buf, res_buf_length, mes); return 0
#define RETURN_SUCCESS_BUFF(res, res_length, mes, mes_length) key_service_build_success_res_buf(res_buf, res_buf_length, mes, mes_length); return 0
#define RETURN_ERROR(res, res_length, err_code, mes) key_service_build_error_res(res_buf, res_buf_length, err_code, mes); return -1

int key_service_get_key_(void* ctx, const char* user_id, const uint8_t* id, const size_t id_length, uint8_t** res_buf, size_t* res_buf_length, int is_update){
  HERMES_CHECK(ctx && id && id_length>0, return -2);
  key_service_service_t* kss_ctx=(key_service_service_t*)ctx;
  HERMES_CHECK(kss_ctx->ctx_ && kss_ctx->db_, return -2);
  uint8_t *key, *pubkey;
  size_t key_length, pubkey_length;
  HERMES_CHECK(DB_SUCCESS==keys_db_get_key(kss_ctx->db_, id, user_id, &key, &key_length, &pubkey, &pubkey_length, is_update), RETURN_ERROR(res_buf, res_buf_length, 1, "db error"); return -1);
  buffer_t* res=buffer_create();
  HERMES_CHECK(res, free(key); free(pubkey); RETURN_ERROR(res_buf, res_buf_length, 1, "bad alloc"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_push_data(res, key, key_length), buffer_destroy(res); free(key); free(pubkey); RETURN_ERROR(res_buf, res_buf_length, 1, "bad alloc"); return -1);
  free(key);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_push_data(res, pubkey, pubkey_length), buffer_destroy(res); free(pubkey); RETURN_ERROR(res_buf, res_buf_length, 1, "bad alloc"); return -1);
  free(pubkey);
  key_service_build_res(buffer_get_data(res), buffer_get_size(res), res_buf, res_buf_length);
  buffer_destroy(res);
  return 0;
}

int key_service_get_read_key(void* ctx, const char* user_id, const uint8_t* id, const size_t id_length, uint8_t** res_buf, size_t* res_buf_length){
  return key_service_get_key_(ctx, user_id, id, id_length, res_buf, res_buf_length, 0);
}

int key_service_get_update_key(void* ctx, const char* user_id, const uint8_t* id, const size_t id_length, uint8_t** res_buf, size_t* res_buf_length){
  return key_service_get_key_(ctx, user_id, id, id_length, res_buf, res_buf_length, 1);
}

int key_service_grand_access_(void* ctx, const char* id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length, int is_update){
  HERMES_CHECK(ctx && param && param_length>0, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  key_service_service_t* kss_ctx=(key_service_service_t*)ctx;
  HERMES_CHECK(kss_ctx->ctx_ && kss_ctx->db_, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  buffer_t *param_buf=buffer_create_with_data(param, param_length);
  HERMES_CHECK(param_buf, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  const char *doc_id, *user_id;
  const uint8_t *key, *pubkey;
  size_t key_length, pubkey_length; 
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &doc_id), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &user_id), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &key, &key_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &pubkey, &pubkey_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(strlen(doc_id)==24 && strlen(user_id)==24, buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(DB_SUCCESS==keys_db_add_key(kss_ctx->db_, doc_id, user_id, key, key_length, pubkey, pubkey_length, is_update), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "db error"); return -1);
  buffer_destroy(param_buf);
  RETURN_SUCCESS(res_buf, res_buf_length, "read access granded successfully");
  return 0;
}


int key_service_grant_read_access(void* ctx, const char* user_id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length){
  return key_service_grand_access_(ctx, user_id, param, param_length, res_buf, res_buf_length, 0);
}

int key_service_grant_update_access(void* ctx, const char* user_id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length){
  return key_service_grand_access_(ctx, user_id, param, param_length, res_buf, res_buf_length, 1);
}

int key_service_revoke_access(void* ctx, const char* id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length, int is_update){
  HERMES_CHECK(ctx && param && param_length>0, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  key_service_service_t* kss_ctx=(key_service_service_t*)ctx;
  HERMES_CHECK(kss_ctx->ctx_ && kss_ctx->db_, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  buffer_t *param_buf=buffer_create_with_data(param, param_length);
  HERMES_CHECK(param_buf, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  const char *doc_id, *user_id;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &doc_id), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &user_id), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(strlen(doc_id)==24 && strlen(user_id)==24, buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(DB_SUCCESS==keys_db_del_key(kss_ctx->db_, doc_id, user_id, is_update), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "db error"); return -1);
  buffer_destroy(param_buf);
  RETURN_SUCCESS(res_buf, res_buf_length, "access revoked successfully");
  return 0;
}

int key_service_revoke_read_access(void* ctx, const char* user_id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length){
  return key_service_revoke_access(ctx, user_id, param, param_length, res_buf, res_buf_length, 0);
}

int key_service_revoke_update_access(void* ctx, const char* user_id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length){
  return key_service_revoke_access(ctx, user_id, param, param_length, res_buf, res_buf_length, 1);
}

service_status_t key_service_service_destroy(key_service_service_t* service){
  HERMES_CHECK(service, return SERVICE_INVALID_PARAM);
  if(service->ctx_)
    credential_store_destroy(service->ctx_);
  if(service->db_)
    keys_db_destroy(service->db_);
  return SERVICE_SUCCESS;
}

key_service_service_t* key_service_service_create(){
  key_service_service_t* service=malloc(sizeof(key_service_service_t));
  HERMES_CHECK(service, return NULL);
  service->db_=NULL;
  service->ctx_=key_service_create();
  HERMES_CHECK(service->ctx_, key_service_service_destroy(service); return NULL);
  return service;
}

service_status_t key_service_service_run(key_service_service_t* service){
  HERMES_CHECK(service && service->ctx_ && service->db_, return SERVICE_INVALID_PARAM);
  HERMES_CHECK(PROTOCOL_SUCCESS==key_service_bind(service->ctx_, (void*)service), return SERVICE_FAIL);
  return SERVICE_SUCCESS;  
}

service_status_t key_service_service_stop(key_service_service_t* service){
  HERMES_CHECK(service && service->ctx_ && service->db_, return SERVICE_INVALID_PARAM);
  return SERVICE_FAIL;
}

#undef RETURN_SUCCESS
#undef RETURN_ERROR




