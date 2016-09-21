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

#include <hermes/record_set_protocol.h>
#include <hermes/docs_db_interface.h>
#include <hermes/mid_hermes.h>

#include <hermes/record_set_service.h>

#include <string.h>

#define DOCS_DB 0
#define PRIVATE_BLOCKS_DB 1
#define PUBLIC_BLOCKS_DB 2

struct record_set_service_t_{
  record_set_t* ctx_;
  const config_t* config_;
  docs_db_t* docs_db_;
};

int record_set_build_error_res(uint8_t** res_buf, size_t* res_buf_length, int32_t error_number, const char* error_message){
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

int record_set_build_success_res(uint8_t** res_buf, size_t* res_buf_length, const char* message){
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

int record_set_build_success_res_buf(uint8_t** res_buf, size_t* res_buf_length, const uint8_t* message, const size_t message_length){
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


#define RETURN_SUCCESS(res, res_length, mes) record_set_build_success_res(res_buf, res_buf_length, mes); return 0
#define RETURN_SUCCESS_BUFF(res, res_length, mes, mes_length) record_set_build_success_res_buf(res_buf, res_buf_length, mes, mes_length); return 0
#define RETURN_ERROR(res, res_length, err_code, mes) record_set_build_error_res(res_buf, res_buf_length, err_code, mes); return -1


int record_set_get_docs(void* ctx, const char* user_id, const uint8_t* param, const size_t param_len, uint8_t** res_buf, size_t* res_buf_length){
  record_set_service_t* rss_ctx=(record_set_service_t*)ctx;
  assert(rss_ctx && rss_ctx->ctx_ && rss_ctx->docs_db_);
  buffer_t* res=buffer_create();
  assert(res);
  assert(BUFFER_SUCCESS==buffer_push_status(res, 0));
  HERMES_CHECK(DB_SUCCESS==docs_db_get_docs(rss_ctx->docs_db_, res), buffer_destroy(res); RETURN_ERROR(res_buf, res_buf_length, 2, "db error"); return -1);
  *res_buf=realloc(*res_buf, buffer_get_size(res));
  assert(*res_buf);
  memcpy(*res_buf, buffer_get_data(res), buffer_get_size(res));
  *res_buf_length = buffer_get_size(res);
  buffer_destroy(res);
  return 0;
}

int record_set_get_doc_breaf(void* ctx, const char* user_id, const uint8_t* params, const size_t params_length, uint8_t** res_buf, size_t* res_buf_length){
  record_set_service_t* rss_ctx=(record_set_service_t*)ctx;
  assert(rss_ctx->ctx_ && rss_ctx->docs_db_);
  const char *user_id_, *doc_id;
  buffer_t *buff = buffer_create_with_data(params, params_length);
  assert(buff);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(buff, &user_id_) && BUFFER_SUCCESS==buffer_pop_string(buff, &doc_id), buffer_destroy(buff); return -1);
  char* res=NULL;
  HERMES_CHECK(DB_SUCCESS==docs_db_get(rss_ctx->docs_db_, doc_id, &res), RETURN_ERROR(res_buf, res_buf_length, 2, "db error"); buffer_destroy(buff);return -1);
  buffer_destroy(buff);
  RETURN_SUCCESS(res_buf, res_buf_length, res);
  free(res);
  return 0;
}

int record_set_get_doc_data(void* ctx, const char* user_id, const uint8_t* doc_id, const size_t doc_id_length, uint8_t** res_buf, size_t* res_buf_length){
  record_set_service_t* rss_ctx=(record_set_service_t*)ctx;
  HERMES_CHECK(rss_ctx->ctx_ && rss_ctx->docs_db_, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  char* res=NULL;
  HERMES_CHECK(DB_SUCCESS==docs_db_get_data(rss_ctx->docs_db_, doc_id, &res), RETURN_ERROR(res_buf, res_buf_length, 2, "db error"); return -1);
  RETURN_SUCCESS(res_buf, res_buf_length, res);
  free(res);
  return 0;
}

int record_set_get_doc(void* ctx, const char* user_id, const uint8_t* id, const size_t id_length, uint8_t** res_buf, size_t* res_buf_length){
  RETURN_ERROR(res_buf, res_buf_length, 1, "not supported");  
}

int record_set_get_block_(void* ctx, const char* user_id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length, int is_private){
  record_set_service_t* rss_ctx=(record_set_service_t*)ctx;
  assert(rss_ctx->ctx_ && rss_ctx->docs_db_);
  const char *user_id_, *block_id, *doc_id;
  buffer_t *buff = buffer_create_with_data(param, param_length);
  assert(buff);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(buff, &user_id_), buffer_destroy(buff); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(buff, &block_id), buffer_destroy(buff); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(buff, &doc_id), buffer_destroy(buff); return -1);
  uint8_t* res=NULL;
  size_t res_length=0;
  HERMES_CHECK(DB_SUCCESS==docs_db_get_block(rss_ctx->docs_db_, block_id, doc_id, &res, &res_length, is_private), RETURN_ERROR(res_buf, res_buf_length, 2, "db error"); buffer_destroy(buff);return -1);
  buffer_destroy(buff);
  RETURN_SUCCESS_BUFF(res_buf, res_buf_length, res, res_length);
  free(res);
  return 0;
}

int record_set_get_public_block(void* ctx, const char* user_id, const uint8_t* id, const size_t id_length, uint8_t** res_buf, size_t* res_buf_length){
  return record_set_get_block_(ctx, user_id, id, id_length, res_buf, res_buf_length, 0);
}

int record_set_get_private_block(void* ctx, const char* user_id, const uint8_t* id, const size_t id_length, uint8_t** res_buf, size_t* res_buf_length){
  return record_set_get_block_(ctx, user_id, id, id_length, res_buf, res_buf_length, 1);
}

int record_set_update_block_(void* ctx, const char* user_id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length, int is_private){
  HERMES_CHECK(ctx && param && param_length>0, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  record_set_service_t* rss_ctx=(record_set_service_t*)ctx;
  HERMES_CHECK(rss_ctx->ctx_ && rss_ctx->docs_db_, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  buffer_t* param_buf=buffer_create_with_data(param, param_length);
  HERMES_CHECK(param_buf, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  const char* doc_id, *block_id;
  const uint8_t *old_ut, *new_ut, *new_block;
  size_t old_ut_length, new_ut_length, new_block_length;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &doc_id), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &block_id), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &new_block, &new_block_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &old_ut, &old_ut_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &new_ut, &new_ut_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(strlen(doc_id)==HERMES_ID_LENGTH && strlen(block_id)==HERMES_ID_LENGTH && old_ut_length==HERMES_MAC_LENGTH && new_ut_length==HERMES_MAC_LENGTH, buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(DB_SUCCESS==docs_db_update_block(rss_ctx->docs_db_, doc_id, block_id, new_block, new_block_length, old_ut, new_ut, is_private), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 2, "db error"); return -1);
  buffer_destroy(param_buf);
  RETURN_SUCCESS(res_buf, res_buf_length, "updating success");
  return 0;
}

int record_set_update_document(void* ctx, const char* user_id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length){
  HERMES_CHECK(ctx && param && param_length>0, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  record_set_service_t* rss_ctx=(record_set_service_t*)ctx;
  HERMES_CHECK(rss_ctx->ctx_ && rss_ctx->docs_db_, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  buffer_t* param_buf=buffer_create_with_data(param, param_length);
  HERMES_CHECK(param_buf, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  const char *user, *doc, *doc_id;
  const uint8_t *mac, *old_mac;
  size_t mac_length, old_mac_length;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &user), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &doc), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &mac, &mac_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(mac_length==HERMES_MAC_LENGTH, buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &doc_id), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &old_mac, &old_mac_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(old_mac_length==HERMES_MAC_LENGTH, buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(DB_SUCCESS==docs_db_update(rss_ctx->docs_db_, user, doc, mac, doc_id, old_mac), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 2, "db error"); return -1);
  buffer_destroy(param_buf);
  RETURN_SUCCESS(res_buf, res_buf_length, "success");
  return 0;
}

int record_set_update_public_block(void* ctx, const char* user_id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length){
  return record_set_update_block_(ctx, user_id, param, param_length, res_buf, res_buf_length, 0);
}

int record_set_update_private_block(void* ctx, const char* user_id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length){
  return record_set_update_block_(ctx, user_id, param, param_length, res_buf, res_buf_length, 1);
}

int record_set_add_block_(void* ctx, const char* user_id_, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length, int private){
  HERMES_CHECK(ctx && param && param_length>0, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  record_set_service_t* rss_ctx=(record_set_service_t*)ctx;
  HERMES_CHECK(rss_ctx->ctx_ && rss_ctx->docs_db_, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  buffer_t* param_buf=buffer_create_with_data(param, param_length);
  HERMES_CHECK(param_buf, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  const char * user_id, *doc_id, *name;
  const uint8_t *new_block, *mac;
  size_t new_block_length, mac_length;
  fprintf(stderr, "%i\n", buffer_get_size(param_buf));
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &user_id), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &doc_id), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &mac, &mac_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &name), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &new_block, &new_block_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);  
  HERMES_CHECK(strlen(doc_id)==HERMES_ID_LENGTH && mac_length==HERMES_MAC_LENGTH, buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(DB_SUCCESS==docs_db_add_block(rss_ctx->docs_db_, doc_id, name, new_block, new_block_length, mac, private), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 2, "db error"); return -1);
  buffer_destroy(param_buf);
  RETURN_SUCCESS(res_buf, res_buf_length, "success");
  return 0;
}

int record_set_add_access(void* ctx, const char* user_id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length){
  HERMES_CHECK(ctx && param && param_length>0, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  record_set_service_t* rss_ctx=(record_set_service_t*)ctx;
  HERMES_CHECK(rss_ctx->ctx_ && rss_ctx->docs_db_, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  buffer_t* param_buf=buffer_create_with_data(param, param_length);
  HERMES_CHECK(param_buf, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  const char *user, *doc_id, *new_user_name;
  const uint8_t *mac, *rt, *ut, *pub_key;
  size_t mac_length, rt_length, ut_length, pub_key_length;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &user), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &doc_id), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &mac, &mac_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(mac_length==HERMES_MAC_LENGTH, buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &new_user_name), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &rt, &rt_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &ut, &ut_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &pub_key, &pub_key_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(DB_SUCCESS==docs_db_add_access(rss_ctx->docs_db_, user, doc_id, mac, new_user_name, rt, rt_length, ut, ut_length, pub_key, pub_key_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 2, "db error"); return -1);
  buffer_destroy(param_buf);
  RETURN_SUCCESS(res_buf, res_buf_length, "success");
  return 0;
}

int record_set_rm_access(void* ctx, const char* user_id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length){
  HERMES_CHECK(ctx && param && param_length>0, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  record_set_service_t* rss_ctx=(record_set_service_t*)ctx;
  HERMES_CHECK(rss_ctx->ctx_ && rss_ctx->docs_db_, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  buffer_t* param_buf=buffer_create_with_data(param, param_length);
  HERMES_CHECK(param_buf, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  const char *user, *doc_id, *new_user_name;
  const uint8_t *mac;
  size_t mac_length;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &user), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &doc_id), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &mac, &mac_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(mac_length==HERMES_MAC_LENGTH, buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &new_user_name), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(DB_SUCCESS==docs_db_rm_access(rss_ctx->docs_db_, user, doc_id, mac, new_user_name), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 2, "db error"); return -1);
  buffer_destroy(param_buf);
  RETURN_SUCCESS(res_buf, res_buf_length, "success");
  return 0;
}

int record_set_add_document(void* ctx, const char* user_id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length){
  HERMES_CHECK(ctx && param && param_length>0, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  record_set_service_t* rss_ctx=(record_set_service_t*)ctx;
  HERMES_CHECK(rss_ctx->ctx_ && rss_ctx->docs_db_, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  buffer_t* param_buf=buffer_create_with_data(param, param_length);
  HERMES_CHECK(param_buf, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  const char *user, *doc;
  const uint8_t *mac, *rt, *ut, *pub_key;
  size_t mac_length, rt_length, ut_length, pub_key_length;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &user), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &doc), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &mac, &mac_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(mac_length==HERMES_MAC_LENGTH, buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &rt, &rt_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &ut, &ut_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &pub_key, &pub_key_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  char *new_id_str;
  HERMES_CHECK(DB_SUCCESS==docs_db_add(rss_ctx->docs_db_, user, doc, mac, rt, rt_length, ut, ut_length, pub_key, pub_key_length, &new_id_str), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 2, "db error"); return -1);
  buffer_destroy(param_buf);
  RETURN_SUCCESS(res_buf, res_buf_length, new_id_str);
  return 0;
}

int record_set_add_public_block(void* ctx, const char* user_id, const uint8_t* param_buf, const size_t param_buf_length, uint8_t** res_buf, size_t* res_buf_length){
  return record_set_add_block_(ctx, user_id, param_buf, param_buf_length, res_buf, res_buf_length, 0);
}

int record_set_add_private_block(void* ctx, const char* user_id, const uint8_t* param_buf, const size_t param_buf_length, uint8_t** res_buf, size_t* res_buf_length){
  return record_set_add_block_(ctx, user_id, param_buf, param_buf_length, res_buf, res_buf_length, 1);  
}

int record_set_delete_block_(void* ctx, const char* user_id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length, int is_private){
  HERMES_CHECK(ctx && param && param_length>0, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  record_set_service_t* rss_ctx=(record_set_service_t*)ctx;
  HERMES_CHECK(rss_ctx->ctx_ && rss_ctx->docs_db_, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  buffer_t* param_buf=buffer_create_with_data(param, param_length);
  HERMES_CHECK(param_buf, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  const char *user_id_, *doc_id, *block_id;
  const uint8_t *mac;
  size_t mac_length;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &user_id_), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &block_id), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &doc_id), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &mac, &mac_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(strlen(doc_id)==HERMES_ID_LENGTH && strlen(block_id)==HERMES_ID_LENGTH && mac_length==HERMES_MAC_LENGTH, buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(DB_SUCCESS==docs_db_delete_block(rss_ctx->docs_db_, doc_id, block_id, mac, is_private), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 2, "db error"); return -1);
  buffer_destroy(param_buf);
  RETURN_SUCCESS(res_buf, res_buf_length, "updating success");
  return 0;
}

int record_set_delete_public_block(void* ctx, const char* user_id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length){
  return record_set_delete_block_(ctx, user_id, param, param_length, res_buf, res_buf_length, 0);
}

int record_set_delete_private_block(void* ctx, const char* user_id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length){
  return record_set_delete_block_(ctx, user_id, param, param_length, res_buf, res_buf_length, 1);
}

int record_set_delete_document(void* ctx, const char* user_id, const uint8_t* param, const size_t param_length, uint8_t** res_buf, size_t* res_buf_length){
  HERMES_CHECK(ctx && param && param_length>0, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  record_set_service_t* rss_ctx=(record_set_service_t*)ctx;
  HERMES_CHECK(rss_ctx->ctx_ && rss_ctx->docs_db_, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  buffer_t* param_buf=buffer_create_with_data(param, param_length);
  HERMES_CHECK(param_buf, RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -2);
  const char* doc_id;
  const uint8_t *ut;
  size_t ut_length;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_string(param_buf, &doc_id), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(param_buf, &ut, &ut_length), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(strlen(doc_id)==HERMES_ID_LENGTH && ut_length==HERMES_MAC_LENGTH, buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 1, "get corrupted data"); return -1);
  HERMES_CHECK(DB_SUCCESS==docs_db_delete(rss_ctx->docs_db_, doc_id, ut), buffer_destroy(param_buf); RETURN_ERROR(res_buf, res_buf_length, 2, "db error"); return -1);
  buffer_destroy(param_buf);
  RETURN_SUCCESS(res_buf, res_buf_length, "updating success");
  return 0;
}

service_status_t record_set_service_destroy(record_set_service_t* service){
  HERMES_CHECK(service, return SERVICE_INVALID_PARAM);
  if(service->ctx_)
    record_set_destroy(service->ctx_);
  if(service->docs_db_)
    docs_db_destroy(service->docs_db_);
  return SERVICE_SUCCESS;
}

record_set_service_t* record_set_service_create(const config_t* config){
  HERMES_CHECK(config, return NULL);
  record_set_service_t* service=calloc(1, sizeof(record_set_service_t));
  HERMES_CHECK(service, return NULL);
  service->ctx_=record_set_create();
  HERMES_CHECK(service->ctx_, record_set_service_destroy(service); return NULL);
  service->config_=config;
  service->docs_db_=docs_db_create(config->record_set.db_endpoint, config->record_set.db_name);
  HERMES_CHECK(service->docs_db_, record_set_service_destroy(service); return NULL);
  return service;
}

service_status_t record_set_service_run(record_set_service_t* service){
  HERMES_CHECK(service && service->docs_db_ && service->config_, return SERVICE_INVALID_PARAM);
  HERMES_CHECK(PROTOCOL_SUCCESS==record_set_bind(service->ctx_, service->config_->record_set.endpoint, (void*)service), return SERVICE_FAIL);
  return SERVICE_SUCCESS;  
}

service_status_t record_set_service_stop(record_set_service_t* service){
  HERMES_CHECK(service && service->docs_db_ && service->config_, return SERVICE_INVALID_PARAM);
  return SERVICE_FAIL;
}






