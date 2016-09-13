/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#include <stdlib.h>
#include <string.h>
#include <hermes/utils.h>
#include <hermes/buffer.h>
#include <themis/themis.h>
#include <hermes/record_set.h>

struct record_set_client_t_{
  record_set_t* ctx_;
  const config_t* config_;
};

int record_set_client_mac_data_(const uint8_t* token, const uint8_t* data, const size_t data_length, uint8_t** enc_data, size_t* enc_data_length){
  size_t dcl_=0;
  HERMES_CHECK(THEMIS_BUFFER_TOO_SMALL==themis_secure_cell_encrypt_context_imprint(token, HERMES_TOKEN_LENGTH, data, data_length, "1", 1, NULL, &dcl_), return -1);
  if(dcl_>(*enc_data_length)){
    *enc_data=realloc((*enc_data), dcl_);
  }
  HERMES_CHECK(*enc_data, return -1);
  *enc_data_length=dcl_;
  HERMES_CHECK(THEMIS_SUCCESS==themis_secure_cell_encrypt_context_imprint(token, HERMES_TOKEN_LENGTH, data, data_length, "1", 1, *enc_data, enc_data_length), return -1);
  return 0;
}

int record_set_client_encrypt_data_(const uint8_t* token, const uint8_t* data, const size_t data_length, uint8_t** enc_data, size_t* enc_data_length){
  size_t dcl_=0;
  HERMES_CHECK(THEMIS_BUFFER_TOO_SMALL==themis_secure_cell_encrypt_seal(token, HERMES_TOKEN_LENGTH, NULL, 0, data, data_length, NULL, &dcl_), return -1);
  if(dcl_>(*enc_data_length)){
    *enc_data=realloc((*enc_data), dcl_);
  }
  HERMES_CHECK(*enc_data, return -1);
  *enc_data_length=dcl_;
  HERMES_CHECK(THEMIS_SUCCESS==themis_secure_cell_encrypt_seal(token, HERMES_TOKEN_LENGTH, NULL, 0, data, data_length, *enc_data, enc_data_length), return -1);
  return 0;
}

int record_set_client_get_docs_list(record_set_client_t* rsc, char** r){
  assert(rsc && rsc->ctx_);
  uint8_t* res=NULL;
  size_t res_length;
  HERMES_CHECK(0==record_set_get_docs_sync_proxy(rsc->ctx_, rsc->config_->id, strlen(rsc->config_->id), &res, &res_length), return -1);
  buffer_t* buffer=buffer_create_with_data(res, res_length);
  free(res);
  assert(buffer);
  int status=0;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_status(buffer, &status) && 0==status, buffer_destroy(buffer); return -1);
  const char *id, *data; 
  *r = calloc(1, res_length*2);
  strcpy(*r, "[");
  assert(*r);
  if(BUFFER_SUCCESS==buffer_pop_string(buffer, &id) && BUFFER_SUCCESS==buffer_pop_string(buffer, &data)){
    sprintf(*r, "%s{\"id\":\"%s\", \"name\":\"%s\"}", *r, id, data);
    while(BUFFER_SUCCESS==buffer_pop_string(buffer, &id) && BUFFER_SUCCESS==buffer_pop_string(buffer, &data))
      sprintf(*r, "%s,{\"id\":\"%s\", \"name\":\"%s\"}", *r, id, data);
  }
  sprintf(*r, "%s]", *r);
  buffer_destroy(buffer);
  return 0;
}

int record_set_client_read_doc(record_set_client_t* rsc, const char* user_id, const char* id, char** doc_data){
  assert(rsc && rsc->ctx_);
  uint8_t* res=NULL;
  size_t res_length;
  buffer_t* params=BUFFER_NEW(BUFFER_NODE_STR(user_id), BUFFER_NODE_STR(id));
  assert(params);
  HERMES_CHECK(0==record_set_get_doc_breaf_sync_proxy(rsc->ctx_, buffer_get_data(params), buffer_get_size(params), &res, &res_length), buffer_destroy(params); return -1);
  buffer_destroy(params);
  params=buffer_create_with_data(res, res_length);
  free(res);
  assert(params);
  int status=0;
  const char *data; 
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_status(params, &status) && 0==status && BUFFER_SUCCESS==buffer_pop_string(params, &data), buffer_destroy(params); return -1);
 *doc_data=malloc(strlen(data)+1);
  assert(*doc_data);
  memcpy(*doc_data, data, strlen(data)+1);
  buffer_destroy(params);
  return 0;
}

int record_set_client_add_doc(record_set_client_t* rsc, const char* user_id, const char* doc, const uint8_t* mac, const size_t mac_length, const uint8_t* rt, const size_t rt_length, const uint8_t* ut, const size_t ut_length, const uint8_t* pub_key, const size_t pub_key_length){
  assert(rsc && rsc->ctx_);
  buffer_t* params=buffer_create();
  assert(params);
  assert(BUFFER_SUCCESS==buffer_push_string(params, user_id));
  assert(BUFFER_SUCCESS==buffer_push_string(params, doc));
  assert(BUFFER_SUCCESS==buffer_push_data(params, mac, mac_length));
  assert(BUFFER_SUCCESS==buffer_push_data(params, rt, rt_length));  
  assert(BUFFER_SUCCESS==buffer_push_data(params, ut, ut_length));  
  assert(BUFFER_SUCCESS==buffer_push_data(params, pub_key, pub_key_length));  
  fprintf(stderr, "aaa\n");
  uint8_t* res=NULL;
  size_t res_length;
  HERMES_CHECK(0==record_set_add_document_sync_proxy(rsc->ctx_, buffer_get_data(params), buffer_get_size(params), &res, &res_length), buffer_destroy(params); return -1);
  buffer_destroy(params);
  params=buffer_create_with_data(res, res_length);
  free(res);
  assert(params);
  int status=0;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_status(params, &status) && 0==status, buffer_destroy(params); return -1);
  buffer_destroy(params);
  return 0;
}

int record_set_client_update_doc(record_set_client_t* rsc, const char* user_id, const char* doc, const uint8_t* mac, const size_t mac_length, const char* doc_id, const uint8_t* old_mac, const size_t old_mac_length){
  assert(rsc && rsc->ctx_);
  buffer_t* params=buffer_create();
  assert(params);
  assert(BUFFER_SUCCESS==buffer_push_string(params, user_id));
  assert(BUFFER_SUCCESS==buffer_push_string(params, doc));
  assert(BUFFER_SUCCESS==buffer_push_data(params, mac, mac_length));
  assert(BUFFER_SUCCESS==buffer_push_string(params, doc_id));  
  assert(BUFFER_SUCCESS==buffer_push_data(params, old_mac, old_mac_length));  
  uint8_t* res=NULL;
  size_t res_length;
  HERMES_CHECK(0==record_set_update_document_sync_proxy(rsc->ctx_, buffer_get_data(params), buffer_get_size(params), &res, &res_length), buffer_destroy(params); return -1);
  buffer_destroy(params);
  params=buffer_create_with_data(res, res_length);
  free(res);
  assert(params);
  int status=0;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_status(params, &status) && 0==status, buffer_destroy(params); return -1);
  buffer_destroy(params);
  return 0;
}

int record_set_client_add_public_block(record_set_client_t* rsc, const char* user_id, const char* doc_id, const uint8_t* mac, const size_t mac_length, const char* name, const uint8_t* block, const size_t block_length){
  assert(rsc && rsc->ctx_);
  buffer_t* params=buffer_create();
  assert(params);
  assert(BUFFER_SUCCESS==buffer_push_string(params, user_id));
  assert(BUFFER_SUCCESS==buffer_push_string(params, doc_id));
  assert(BUFFER_SUCCESS==buffer_push_data(params, mac, mac_length));
  assert(BUFFER_SUCCESS==buffer_push_string(params, name));
  assert(BUFFER_SUCCESS==buffer_push_data(params, block, block_length));  
  uint8_t* res=NULL;
  size_t res_length;
  HERMES_CHECK(0==record_set_add_public_block_sync_proxy(rsc->ctx_, buffer_get_data(params), buffer_get_size(params), &res, &res_length), buffer_destroy(params); return -1);
  buffer_destroy(params);
  params=buffer_create_with_data(res, res_length);
  free(res);
  assert(params);
  int status=0;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_status(params, &status) && 0==status, buffer_destroy(params); return -1);
  buffer_destroy(params);
  return 0;
}

int record_set_client_add_private_block(record_set_client_t* rsc, const char* user_id, const char* doc_id, const uint8_t* mac, const size_t mac_length, const char* name, const uint8_t* block, const size_t block_length){
  assert(rsc && rsc->ctx_);
  buffer_t* params=buffer_create();
  assert(params);
  assert(BUFFER_SUCCESS==buffer_push_string(params, user_id));
  assert(BUFFER_SUCCESS==buffer_push_string(params, doc_id));
  assert(BUFFER_SUCCESS==buffer_push_data(params, mac, mac_length));
  assert(BUFFER_SUCCESS==buffer_push_string(params, name));
  assert(BUFFER_SUCCESS==buffer_push_data(params, block, block_length));  
  uint8_t* res=NULL;
  size_t res_length;
  HERMES_CHECK(0==record_set_add_private_block_sync_proxy(rsc->ctx_, buffer_get_data(params), buffer_get_size(params), &res, &res_length), buffer_destroy(params); return -1);
  buffer_destroy(params);
  params=buffer_create_with_data(res, res_length);
  free(res);
  assert(params);
  int status=0;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_status(params, &status) && 0==status, buffer_destroy(params); return -1);
  buffer_destroy(params);
  return 0;
}

int record_set_client_add_access(record_set_client_t* rsc, const char* user_id, const char* doc_id, const uint8_t* mac, const size_t mac_length, const char* new_user_name, const uint8_t* ut, const size_t ut_length, const uint8_t* rt, const size_t rt_length, const uint8_t* pubk, const size_t pubk_length){
  assert(rsc && rsc->ctx_);
  buffer_t* params=buffer_create();
  assert(params);
  assert(BUFFER_SUCCESS==buffer_push_string(params, user_id));
  assert(BUFFER_SUCCESS==buffer_push_string(params, doc_id));
  assert(BUFFER_SUCCESS==buffer_push_data(params, mac, mac_length));
  assert(BUFFER_SUCCESS==buffer_push_string(params, new_user_name));
  assert(BUFFER_SUCCESS==buffer_push_data(params, ut, ut_length));  
  assert(BUFFER_SUCCESS==buffer_push_data(params, rt, rt_length));  
  assert(BUFFER_SUCCESS==buffer_push_data(params, pubk, pubk_length));  
  uint8_t* res=NULL;
  size_t res_length;
  HERMES_CHECK(0==record_set_add_access_sync_proxy(rsc->ctx_, buffer_get_data(params), buffer_get_size(params), &res, &res_length), buffer_destroy(params); return -1);
  buffer_destroy(params);
  params=buffer_create_with_data(res, res_length);
  free(res);
  assert(params);
  int status=0;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_status(params, &status) && 0==status, buffer_destroy(params); return -1);
  buffer_destroy(params);
  return 0;
}

int record_set_client_rm_access(record_set_client_t* rsc, const char* user_id, const char* doc_id, const uint8_t* mac, const size_t mac_length, const char* new_user_name){
  assert(rsc && rsc->ctx_);
  buffer_t* params=buffer_create();
  assert(params);
  assert(BUFFER_SUCCESS==buffer_push_string(params, user_id));
  assert(BUFFER_SUCCESS==buffer_push_string(params, doc_id));
  assert(BUFFER_SUCCESS==buffer_push_data(params, mac, mac_length));
  assert(BUFFER_SUCCESS==buffer_push_string(params, new_user_name));
  uint8_t* res=NULL;
  size_t res_length;
  HERMES_CHECK(0==record_set_rm_access_sync_proxy(rsc->ctx_, buffer_get_data(params), buffer_get_size(params), &res, &res_length), buffer_destroy(params); return -1);
  buffer_destroy(params);
  params=buffer_create_with_data(res, res_length);
  free(res);
  assert(params);
  int status=0;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_status(params, &status) && 0==status, buffer_destroy(params); return -1);
  buffer_destroy(params);
  return 0;
}

int record_set_client_get_public_block(record_set_client_t* rsc, const char* user_id, const char* block_id, const char* doc_id, uint8_t** block, size_t* block_length){
  assert(rsc && rsc->ctx_);
  buffer_t* params=buffer_create();
  assert(params);
  assert(BUFFER_SUCCESS==buffer_push_string(params, user_id));
  assert(BUFFER_SUCCESS==buffer_push_string(params, block_id));
  assert(BUFFER_SUCCESS==buffer_push_string(params, doc_id));
  uint8_t* res=NULL;
  size_t res_length;
  HERMES_CHECK(0==record_set_get_public_block_sync_proxy(rsc->ctx_, buffer_get_data(params), buffer_get_size(params), &res, &res_length), buffer_destroy(params); return -1);
  buffer_destroy(params);
  params=buffer_create_with_data(res, res_length);
  free(res);
  assert(params);
  int status=0;
  const uint8_t* block_;
  size_t block_length_;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_status(params, &status) && 0==status, buffer_destroy(params); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(params, &block_, &block_length_), buffer_destroy(params); return -1);
  *block=malloc(block_length_);
  memcpy(*block, block_, block_length_);
  *block_length=block_length_;
  buffer_destroy(params);
  return 0;
}

int record_set_client_get_private_block(record_set_client_t* rsc, const char* user_id, const char* block_id, const char* doc_id, uint8_t** block, size_t* block_length){
  assert(rsc && rsc->ctx_);
  buffer_t* params=buffer_create();
  assert(params);
  assert(BUFFER_SUCCESS==buffer_push_string(params, user_id));
  assert(BUFFER_SUCCESS==buffer_push_string(params, block_id));
  assert(BUFFER_SUCCESS==buffer_push_string(params, doc_id));
  uint8_t* res=NULL;
  size_t res_length;
  HERMES_CHECK(0==record_set_get_private_block_sync_proxy(rsc->ctx_, buffer_get_data(params), buffer_get_size(params), &res, &res_length), buffer_destroy(params); return -1);
  buffer_destroy(params);
  params=buffer_create_with_data(res, res_length);
  free(res);
  assert(params);
  int status=0;
  const uint8_t* block_;
  size_t block_length_;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_status(params, &status) && 0==status, buffer_destroy(params); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(params, &block_, &block_length_), buffer_destroy(params); return -1);
  *block=malloc(block_length_);
  memcpy(*block, block_, block_length_);
  *block_length=block_length_;
  buffer_destroy(params);
  return 0;
}

int record_set_client_rm_public_block(record_set_client_t* rsc, const char* user_id, const char* block_id, const char* doc_id, const uint8_t* mac, const size_t mac_length){
  assert(rsc && rsc->ctx_);
  buffer_t* params=buffer_create();
  assert(params);
  assert(BUFFER_SUCCESS==buffer_push_string(params, user_id));
  assert(BUFFER_SUCCESS==buffer_push_string(params, block_id));
  assert(BUFFER_SUCCESS==buffer_push_string(params, doc_id));
  assert(BUFFER_SUCCESS==buffer_push_data(params, mac, mac_length));
  uint8_t* res=NULL;
  size_t res_length;
  HERMES_CHECK(0==record_set_delete_public_block_sync_proxy(rsc->ctx_, buffer_get_data(params), buffer_get_size(params), &res, &res_length), buffer_destroy(params); return -1);
  buffer_destroy(params);
  params=buffer_create_with_data(res, res_length);
  free(res);
  assert(params);
  int status=0;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_status(params, &status) && 0==status, buffer_destroy(params); return -1);
  buffer_destroy(params);
  return 0;
}

int record_set_client_rm_private_block(record_set_client_t* rsc, const char* user_id, const char* block_id, const char* doc_id, const uint8_t* mac, const size_t mac_length){
  assert(rsc && rsc->ctx_);
  buffer_t* params=buffer_create();
  assert(params);
  assert(BUFFER_SUCCESS==buffer_push_string(params, user_id));
  assert(BUFFER_SUCCESS==buffer_push_string(params, block_id));
  assert(BUFFER_SUCCESS==buffer_push_string(params, doc_id));
  assert(BUFFER_SUCCESS==buffer_push_data(params, mac, mac_length));
  uint8_t* res=NULL;
  size_t res_length;
  HERMES_CHECK(0==record_set_delete_private_block_sync_proxy(rsc->ctx_, buffer_get_data(params), buffer_get_size(params), &res, &res_length), buffer_destroy(params); return -1);
  buffer_destroy(params);
  params=buffer_create_with_data(res, res_length);
  free(res);
  assert(params);
  int status=0;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_status(params, &status) && 0==status, buffer_destroy(params); return -1);
  buffer_destroy(params);
  return 0;
}


record_set_client_t* record_set_client_create(const config_t* config){
  HERMES_CHECK(config, return NULL);
  record_set_client_t* ctx=malloc(sizeof(record_set_client_t));
  HERMES_CHECK(ctx, return NULL);
  ctx->ctx_=record_set_create();
  HERMES_CHECK(ctx, record_set_client_destroy(ctx); return NULL);
  HERMES_CHECK(PROTOCOL_SUCCESS==record_set_sync_connect(ctx->ctx_, config->record_set_client.endpoint, config->id), record_set_client_destroy(ctx); return NULL);
  ctx->config_=config;
  return ctx;
}

int record_set_client_destroy(record_set_client_t* ctx){
  HERMES_CHECK(ctx, return -1);
  if(ctx->ctx_){
    record_set_destroy(ctx->ctx_);
    ctx->ctx_=NULL;
  }
  free(ctx);
  return 0;
}




