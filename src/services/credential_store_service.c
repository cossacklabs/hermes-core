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

#include <string.h>

#include <hermes/utils.h>
#include <hermes/credential_store_protocol.h>
#include <hermes/users_db_interface.h>

#include <hermes/credential_store_service.h>


struct credential_store_service_t_{
  credential_store_t* ctx_;
  users_db_t* db_;
};


/*
 * param must be user_id Hex string  
 */
int credential_store_build_error_res(uint8_t** res_buf, size_t* res_buf_length, int32_t error_number, const char* error_message){
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

int credential_store_build_success_res(uint8_t** res_buf, size_t* res_buf_length, const char* message){
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


#define RETURN_SUCCESS(res, res_length, mes) record_set_build_success_res(res_buf, res_buf_length, mes); return 0
#define RETURN_SUCCESS_BUFF(res, res_length, mes) record_set_build_success_res(res_buf, res_buf_length, mes); return 0
#define RETURN_ERROR(res, res_length, err_code, mes) record_set_build_error_res(res_buf, res_buf_length, err_code, mes); return -1

int credential_store_get_public_key(void* ctx, const char* user_id, const uint8_t* param_buf, const size_t param_buf_length, uint8_t** res_buf, size_t* res_buf_length){
  HERMES_CHECK(ctx && param_buf && param_buf_length>0, return -2);
  credential_store_service_t* css_ctx=(credential_store_service_t*)ctx;
  HERMES_CHECK(css_ctx->ctx_ && css_ctx->db_, return -2);
  buffer_t* res=buffer_create();
  assert(res);
  assert(BUFFER_SUCCESS==buffer_push_status(res, 0));
  HERMES_CHECK(BUFFER_SUCCESS==users_db_get_pub_key(css_ctx->db_, param_buf, res), buffer_destroy(res); RETURN_ERROR(res_buf, res_buf_length, 2, "db error"); return -1);
  *res_buf=realloc(*res_buf, buffer_get_size(res));
  assert(*res_buf);
  memcpy(*res_buf, buffer_get_data(res), buffer_get_size(res));
  *res_buf_length = buffer_get_size(res);
  buffer_destroy(res);
  return 0;
}

int credential_store_get_users_list(void* ctx, const char* user_id, const uint8_t* param_buf, const size_t param_buf_length, uint8_t** res_buf, size_t* res_buf_length){
  HERMES_CHECK(ctx && param_buf && param_buf_length>0, return -2);
  credential_store_service_t* css_ctx=(credential_store_service_t*)ctx;
  HERMES_CHECK(css_ctx->ctx_ && css_ctx->db_, return -2);
  buffer_t* res=buffer_create();
  assert(res);
  assert(BUFFER_SUCCESS==buffer_push_status(res, 0));
  HERMES_CHECK(DB_SUCCESS==users_db_get_users_list(css_ctx->db_, res), buffer_destroy(res); RETURN_ERROR(res_buf, res_buf_length, 2, "db error"); return -1);
  *res_buf=realloc(*res_buf, buffer_get_size(res));
  assert(*res_buf);
  memcpy(*res_buf, buffer_get_data(res), buffer_get_size(res));
  *res_buf_length = buffer_get_size(res);
  buffer_destroy(res);
  return 0;
}

service_status_t credential_store_service_destroy(credential_store_service_t* service){
  HERMES_CHECK(service, return SERVICE_INVALID_PARAM);
  if(service->ctx_)
    credential_store_destroy(service->ctx_);
  if(service->db_)
    users_db_destroy(service->db_);
  return SERVICE_SUCCESS;
}

credential_store_service_t* credential_store_service_create(){
  credential_store_service_t* service=malloc(sizeof(credential_store_service_t));
  HERMES_CHECK(service, return NULL);
  service->db_=NULL;
  service->ctx_=credential_store_create();
  HERMES_CHECK(service->ctx_, credential_store_service_destroy(service); return NULL);
  service->db_=users_db_create();
  HERMES_CHECK(service->db_, credential_store_service_destroy(service); return NULL);
  return service;
}

service_status_t credential_store_service_run(credential_store_service_t* service){
  HERMES_CHECK(service && service->ctx_ && service->db_, return SERVICE_INVALID_PARAM);
  HERMES_CHECK(PROTOCOL_SUCCESS==credential_store_bind(service->ctx_, (void*)service), return SERVICE_FAIL);
  return SERVICE_SUCCESS;  
}

service_status_t credential_store_service_stop(credential_store_service_t* service){
  HERMES_CHECK(service && service->ctx_ && service->db_, return SERVICE_INVALID_PARAM);
  return SERVICE_FAIL;
}



