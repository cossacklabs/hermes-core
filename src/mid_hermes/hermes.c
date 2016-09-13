/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#include <string.h>
#include <hermes/mid_hermes.h>
#include <stdlib.h>
#include <hermes/utils.h>
#include <hermes/config.h>
#include <hermes/buffer.h>
#include <hermes/credential_store.h>
#include <hermes/record_set.h>

struct hermes_client_t_{
  credential_store_client_t* credential_store_;
  //  key_service_client_t* key_service_;
  record_set_client_t* record_set_;
  const config_t* config_;
};

hermes_client_t* hermes_client_create(const config_t* config){
  assert(config);
  hermes_client_t* client=malloc(sizeof(hermes_client_t));
  assert(client);
  client->credential_store_=credential_store_client_create(config);
  //  client->key_service_=key_service_client_create(config);
  client->record_set_=record_set_client_create(config);
  client->config_=config;
  assert(client->credential_store_);
  return client;
}

void hermes_client_destroy(hermes_client_t** ctx){
  assert(ctx);
  assert(*ctx);
  credential_store_client_destroy((*ctx)->credential_store_);
  //  key_service_client_destroy(ctx->key_service_);
  record_set_client_destroy((*ctx)->record_set_);
  *ctx=NULL;
}

int hermes_client_get_public_key(hermes_client_t* ctx, const char* user_id, uint8_t** res, size_t* res_length){
  assert(ctx);
  return credential_store_client_get_pub_key(ctx->credential_store_, user_id, res, res_length);
}

int hermes_client_get_docs_list(hermes_client_t* ctx, char** res){
  assert(ctx);
  return record_set_client_get_docs_list(ctx->record_set_, res);
}

int hermes_client_read_doc(hermes_client_t* ctx, const char* user_id, const char* id, char** res){
  assert(ctx);
  return record_set_client_read_doc(ctx->record_set_, user_id, id, res);
}

int hermes_client_add_doc(hermes_client_t* ctx, const char* user_id, const char* doc, const uint8_t* mac, const size_t mac_length, const uint8_t* rt, const size_t rt_length, const uint8_t* ut, const size_t ut_length, const uint8_t* pub_key, const size_t pub_key_length){
  assert(ctx);
  return record_set_client_add_doc(ctx->record_set_, user_id, doc, mac, mac_length, rt, rt_length, ut, ut_length, pub_key, pub_key_length);
}

int hermes_client_update_doc(hermes_client_t* ctx, const char* user_id, const char* doc, const uint8_t* mac, const size_t mac_length, const char* doc_id, const uint8_t* old_mac, const size_t old_mac_length){
  assert(ctx);
  return record_set_client_update_doc(ctx->record_set_, user_id, doc, mac, mac_length, doc_id, old_mac, old_mac_length);
}

int hermes_client_get_users_list(hermes_client_t* ctx, const char* user_id, char** res){
  assert(ctx);
  return credential_store_client_get_users_list(ctx->credential_store_, user_id, res);
}

int hermes_client_add_public_block(hermes_client_t* ctx, const char* user_id, const char* doc_id, const uint8_t* mac, const size_t mac_length, const char* name, const uint8_t* block, const size_t block_length){
  assert(ctx);
  return record_set_client_add_public_block(ctx->record_set_, user_id, doc_id, mac, mac_length, name, block, block_length);
}

int hermes_client_add_private_block(hermes_client_t* ctx, const char* user_id, const char* doc_id, const uint8_t* mac, const size_t mac_length, const char* name, const uint8_t* block, const size_t block_length){
  assert(ctx);
  return record_set_client_add_private_block(ctx->record_set_, user_id, doc_id, mac, mac_length, name, block, block_length);
}

int hermes_client_add_access(hermes_client_t* ctx, const char* user_id, const char* doc_id, const uint8_t* mac, const size_t mac_length, const char* new_user_name, const uint8_t* ut, const size_t ut_length, const uint8_t* rt, const size_t rt_length, const uint8_t* pubk, const size_t pubk_length){
  assert(ctx);
  return record_set_client_add_access(ctx->record_set_, user_id, doc_id, mac, mac_length, new_user_name, ut, ut_length, rt, rt_length, pubk, pubk_length);
}

int hermes_client_rm_access(hermes_client_t* ctx, const char* user_id, const char* doc_id, const uint8_t* mac, const size_t mac_length, const char* new_user_name){
  assert(ctx);
  return record_set_client_rm_access(ctx->record_set_, user_id, doc_id, mac, mac_length, new_user_name);
}

int hermes_client_get_public_block(hermes_client_t* ctx, const char* user_id, const char* block_id, const char* doc_id, uint8_t** block, size_t* block_length){
  assert(ctx);
  return record_set_client_get_public_block(ctx->record_set_, user_id, block_id, doc_id, block, block_length);
}

int hermes_client_get_private_block(hermes_client_t* ctx, const char* user_id, const char* block_id, const char* doc_id, uint8_t** block, size_t* block_length){
  assert(ctx);
  return record_set_client_get_private_block(ctx->record_set_, user_id, block_id, doc_id, block, block_length);
}

int hermes_client_rm_public_block(hermes_client_t* ctx, const char* user_id, const char* block_id, const char* doc_id, const uint8_t* mac, const size_t mac_length){
  assert(ctx);
  return record_set_client_rm_public_block(ctx->record_set_, user_id, block_id, doc_id, mac, mac_length);
}
int hermes_client_rm_private_block(hermes_client_t* ctx, const char* user_id, const char* block_id, const char* doc_id, const uint8_t* mac, const size_t mac_length){
  assert(ctx);
  return record_set_client_rm_private_block(ctx->record_set_, user_id, block_id, doc_id, mac, mac_length);
}


/* void hermes_client_grand_self_read_access_handler_(void* context, const int status, const char* mess){ */
/*   hermes_client_ctx_t* ctx=(hermes_client_ctx_t*)context; */
/*   HERMES_CHECK(ctx && ctx->client_ && ctx->success_handler_ && ctx->error_handler_, return); */
/*   if(0!=status) */
/*     ctx->error_handler_(ctx->handler_call_ctx_, -1, "error"); */
/*   else */
/*     ((get_json_handler_t)(ctx->success_handler_))(ctx->handler_call_ctx_, ctx->id_);     */
/*   hermes_client_ctx_destroy(ctx); */
/* } */

/* void hermes_client_grand_self_update_access_handler_(void* context, const int status, const char* mess){ */
/*   hermes_client_ctx_t* ctx=(hermes_client_ctx_t*)context; */
/*   HERMES_CHECK(ctx && ctx->client_ && ctx->success_handler_ && ctx->error_handler_, return); */
/*   if(0!=status) */
/*     ctx->error_handler_(ctx->handler_call_ctx_, -1, "error"); */
/*   else */
/*     HERMES_CHECK(0==key_service_client_grand_self_read_access(ctx->client_->key_service_, ctx->id_, ctx->read_token_, HERMES_TOKEN_LENGTH, hermes_client_grand_self_read_access_handler_, context), ctx->error_handler_(ctx->handler_call_ctx_, -1, "error")); */
/* } */

/* void hermes_client_get_id_handler_(void* context, const char* id, const uint8_t* read_token, const uint8_t* write_token){ */
/*   hermes_client_ctx_t* ctx=(hermes_client_ctx_t*)context; */
/*   HERMES_CHECK(ctx && ctx->client_ && ctx->success_handler_ && ctx->error_handler_, return); */
/*   if(!id){ */
/*     ctx->error_handler_(ctx->handler_call_ctx_, -1, "error"); */
/*   }else{ */
/*     ctx->id_=id; */
/*     ctx->read_token_=read_token; */
/*     HERMES_CHECK(0==key_service_client_grand_self_update_access(ctx->client_->key_service_, id, write_token, HERMES_TOKEN_LENGTH, hermes_client_grand_self_update_access_handler_, context), ctx->error_handler_(ctx->handler_call_ctx_, -1, "error")); */
/*   } */
/* } */

/* hermes_status_t hermes_client_create_document(hermes_client_t* ctx, const char* document, get_json_handler_t success_handler, error_handler_t error_handler, void *handler_ctx){ */
/*   hermes_client_ctx_t* client_ctx=hermes_client_ctx_create(ctx, (void*)success_handler, error_handler, handler_ctx); */
/*   HERMES_CHECK(client_ctx, return HERMES_FAIL); */
/*   return record_set_client_add_doc(ctx->record_set_, document, hermes_client_get_id_handler_, (void*)client_ctx);   */
/* } */

/* hermes_status_t hermes_client_add_public_block_to_document(hermes_client_t* ctx, const char* doc_id, const uint8_t* block, const size_t block_length, get_json_handler_t success_handler, error_handler_t error_handler, void *handler_ctx){ */
/*   hermes_client_ctx_t* client_ctx=hermes_client_ctx_create(ctx, (void*)success_handler, error_handler, handler_ctx); */
/*   HERMES_CHECK(client_ctx, return HERMES_FAIL); */
/*   return record_set_client_add_public_block(ctx->record_set_, doc_id, block, block_length, hermes_client_get_id_handler_, (void*)client_ctx);   */
/* } */

/* hermes_status_t hermes_client_add_private_block_to_document(hermes_client_t* ctx, const char* doc_id, const uint8_t* block, const size_t block_length, get_json_handler_t success_handler, error_handler_t error_handler, void *handler_ctx){ */
/*   hermes_client_ctx_t* client_ctx=hermes_client_ctx_create(ctx, (void*)success_handler, error_handler, handler_ctx); */
/*   HERMES_CHECK(client_ctx, return HERMES_FAIL); */
/*   return record_set_client_add_private_block(ctx->record_set_, doc_id, block, block_length, hermes_client_get_id_handler_, (void*)client_ctx);     */
/* } */


/* void hermes_client_get_status_handler_(void* context, const int status, const char* mes){ */
/*   hermes_client_ctx_t* ctx=(hermes_client_ctx_t*)context; */
/*   HERMES_CHECK(ctx && ctx->client_ && ctx->success_handler_ && ctx->error_handler_, return); */
/*   if(0 == status) */
/*     ((get_status_handler_t)(ctx->success_handler_))(ctx->handler_call_ctx_, status, mes); */
/*   else */
/*     ctx->error_handler_(ctx->handler_call_ctx_, status, mes); */
/*   hermes_client_ctx_destroy(ctx);   */
/* } */

/* void hermes_client_get_read_key_handler_(void* context, const uint8_t* key, const size_t key_length, const char* owner_id){ */
/*   hermes_client_ctx_t* ctx=(hermes_client_ctx_t*)context; */
/*   HERMES_CHECK(ctx && ctx->client_ && ctx->success_handler_ && ctx->error_handler_, return); */
/*   if(!key || !key_length){ */
/*     ctx->error_handler_(ctx->handler_call_ctx_, -1, "error"); */
/*   }else{ */
/*     fprintf(stderr, "get update key with %i bytes\n", key_length); */
/*     record_set_client_update_doc(ctx->client_->record_set_, ctx->id_, key, ctx->old_doc_, ctx->doc_, hermes_client_get_status_handler_, context); */
/*   } */
/* } */

/* void hermes_client_update_get_doc_handler_(void* context, const char* doc, const char* owner_id){ */
/*   fprintf(stderr, "get doc data: %s\n", doc); */
/*   hermes_client_ctx_t* ctx=(hermes_client_ctx_t*)context; */
/*   HERMES_CHECK(ctx && ctx->client_ && ctx->success_handler_ && ctx->error_handler_, return); */
/*   if(!doc){ */
/*     ctx->error_handler_(ctx->handler_call_ctx_, -1, "error"); */
/*   }else{ */
/*     ctx->old_doc_=realloc(ctx->old_doc_, strlen(doc)+1); */
/*     memcpy(ctx->old_doc_, doc, strlen(doc)+1); */
/*     key_service_client_get_update_key(ctx->client_->key_service_, ctx->id_, hermes_client_get_read_key_handler_, context); */
/*   } */
/* } */

/* hermes_status_t hermes_client_update_document(hermes_client_t* ctx, const char* doc_id, const char* document, get_status_handler_t success_handler, error_handler_t error_handler, void *handler_ctx){ */
/*   hermes_client_ctx_t* client_ctx=hermes_client_ctx_create(ctx, (void*)success_handler, error_handler, handler_ctx); */
/*   HERMES_CHECK(client_ctx, return HERMES_FAIL); */
/*   client_ctx->id_=doc_id; */
/*   client_ctx->doc_=document; */
/*   return record_set_client_get_doc_data(ctx->record_set_, doc_id, hermes_client_update_get_doc_handler_, (void*)client_ctx); */
/* } */

/* hermes_status_t hermes_client_update_public_block_in_document(hermes_client_t* ctx, const char* doc_id, const char* block_id, const uint8_t* block, const size_t block_length, get_status_handler_t handler, error_handler_t error_handler, void *handler_ctx){ */
  
/* } */

/* hermes_status_t hermes_client_update_private_block_in_document(hermes_client_t* ctx, const char* doc_id, const char* block_id, const uint8_t* block, const size_t block_length, get_status_handler_t handler, error_handler_t error_handler, void *handler_ctx){ */

/* } */

/* void hermes_client_get_doc_handler_(void* context, const char* doc, const char* owner){ */
/*   hermes_client_ctx_t* ctx=(hermes_client_ctx_t*)context; */
/*   HERMES_CHECK(ctx && ctx->client_ && ctx->success_handler_ && ctx->error_handler_, return); */
/*   if(!doc){ */
/*     ctx->error_handler_(ctx->handler_call_ctx_, -1, "error"); */
/*   }else{ */
/*     ((get_json_handler_t)(ctx->success_handler_))(ctx->handler_call_ctx_, doc);     */
/*   } */
/*   hermes_client_ctx_destroy(ctx); */
/* } */

/* hermes_status_t hermes_client_read_document(hermes_client_t* ctx, const char* doc_id, get_json_handler_t success_handler, error_handler_t error_handler, void *handler_ctx){ */
/*   hermes_client_ctx_t* client_ctx=hermes_client_ctx_create(ctx, (void*)success_handler, error_handler, handler_ctx); */
/*   HERMES_CHECK(client_ctx, return HERMES_FAIL); */
/*   return record_set_client_get_doc_breaf(ctx->record_set_, doc_id, hermes_client_get_doc_handler_, (void*)client_ctx);     */
/* } */

/* void hermes_client_get_block_handler_(void* context, const uint8_t* block, const size_t block_length, const char* owner){ */
/*   hermes_client_ctx_t* ctx=(hermes_client_ctx_t*)context; */
/*   HERMES_CHECK(ctx && ctx->client_ && ctx->success_handler_ && ctx->error_handler_, return); */
/*   if(!block){ */
/*     ctx->error_handler_(ctx->handler_call_ctx_, -1, "error"); */
/*   }else{ */
/*     ((get_buffer_handler_t)(ctx->success_handler_))(ctx->handler_call_ctx_, block, block_length);     */
/*   } */
/*   hermes_client_ctx_destroy(ctx); */
/* } */


/* hermes_status_t hermes_client_read_public_block_of_document(hermes_client_t* ctx, const char* doc_id, const char* block_id, get_buffer_handler_t handler, error_handler_t error_handler, void *handler_ctx){ */
/*   hermes_client_ctx_t* client_ctx=hermes_client_ctx_create(ctx, (void*)handler, error_handler, handler_ctx); */
/*   HERMES_CHECK(client_ctx, return HERMES_FAIL); */
/*   return record_set_client_get_public_block(ctx->record_set_, block_id, hermes_client_get_block_handler_, (void*)client_ctx);       */
/* } */

/* hermes_status_t hermes_client_read_private_block_of_document(hermes_client_t* ctx, const char* doc_id, const char* block_id, get_buffer_handler_t handler, error_handler_t error_handler, void *handler_ctx){ */
/*   hermes_client_ctx_t* client_ctx=hermes_client_ctx_create(ctx, (void*)handler, error_handler, handler_ctx); */
/*   HERMES_CHECK(client_ctx, return HERMES_FAIL); */
/*   return record_set_client_get_private(ctx->record_set_, block_id, hermes_client_get_block_handler_, (void*)client_ctx);     */
/* } */

/* hermes_status_t hermes_client_delete_document(hermes_client_t* ctx, const char* doc_id, get_status_handler_t handler, error_handler_t error_handler, void *handler_ctx){ */

/* } */

/* hermes_status_t hermes_client_delete_public_block_of_document(hermes_client_t* ctx, const char* doc_id, const char* block_id, get_status_handler_t handler, error_handler_t error_handler, void *handler_ctx){ */

/* } */

/* hermes_status_t hermes_client_delete_private_block_of_document(hermes_client_t* ctx, const char* doc_id, const char* block_id, get_status_handler_t handler, error_handler_t error_handler, void *handler_ctx){ */

/* } */

/* void hermes_client_get_set_rights_status_handler_(void* context, const int status, const char* mes){ */
/*   hermes_client_ctx_t* ctx=(hermes_client_ctx_t*)context; */
/*   HERMES_CHECK(ctx && ctx->client_ && ctx->success_handler_ && ctx->error_handler_, return); */
/*   if(0==status){ */
/*     ((get_status_handler_t)(ctx->success_handler_))(ctx->handler_call_ctx_, status, mes); */
/*   }else{ */
/*     ctx->error_handler_(ctx->handler_call_ctx_, status, mes); */
/*   } */
/*   hermes_client_ctx_destroy(ctx);   */
/* } */

/* void hermes_client_set_rights_pub_key_handler_(void* context, const uint8_t* pub_key, const size_t pub_key_length){ */
/*   hermes_client_ctx_t* ctx=(hermes_client_ctx_t*)context; */
/*   HERMES_CHECK(ctx && ctx->client_ && ctx->success_handler_ && ctx->error_handler_, return); */
/*   if(!pub_key || !pub_key_length){ */
/*     ctx->error_handler_(ctx->handler_call_ctx_, -1, "error"); */
/*   }else{ */
/*     switch(ctx->rights_flag_){ */
/*     case 1://read */
/*       key_service_client_grand_read_access(ctx->client_->key_service_, ctx->user_id_, ctx->id_, pub_key, pub_key_length, hermes_client_get_set_rights_status_handler_, context); */
/*       break; */
/*     case 2://update */
/*       key_service_client_grand_update_access(ctx->client_->key_service_, ctx->user_id_, ctx->id_, pub_key, pub_key_length, hermes_client_get_set_rights_status_handler_, context); */
/*       break; */
/*     default://revoke all */
/*       break; */
/*     } */
/*   } */
/* } */

/* hermes_status_t hermes_client_set_document_rights(hermes_client_t* ctx, const char* user_id, const char* doc_id, int is_update, get_status_handler_t success_handler, error_handler_t error_handler, void *handler_ctx){ */
/*   hermes_client_ctx_t* client_ctx=hermes_client_ctx_create(ctx, (void*)success_handler, error_handler, handler_ctx); */
/*   HERMES_CHECK(client_ctx, return HERMES_FAIL); */
/*   client_ctx->id_=doc_id; */
/*   client_ctx->user_id_=user_id; */
/*   client_ctx->rights_flag_=is_update; */
/*   return credential_store_client_get_pub_key(ctx->credential_store_, user_id, hermes_client_set_rights_pub_key_handler_, (void*)client_ctx); */
/* } */

/* hermes_status_t hermes_client_set_private_block_rights(hermes_client_t* ctx, const char* user_id, const char* doc_id, const char* block_id, const char access_flags, get_status_handler_t handler, error_handler_t error_handler, void *handler_ctx){ */
/*   hermes_client_ctx_t* client_ctx=hermes_client_ctx_create(ctx, (void*)handler, error_handler, handler_ctx); */
/*   HERMES_CHECK(client_ctx, return HERMES_FAIL); */
/*   client_ctx->id_=block_id; */
/*   client_ctx->user_id_=user_id; */
/*   client_ctx->rights_flag_=access_flags; */
/*   return credential_store_client_get_pub_key(ctx->credential_store_, user_id, hermes_client_set_rights_pub_key_handler_, (void*)client_ctx); */
/* } */

/* hermes_status_t hermes_client_set_public_block_rights(hermes_client_t* ctx, const char* user_id, const char* doc_id, const char* block_id, const char access_flags, get_status_handler_t handler, error_handler_t error_handler, void *handler_ctx){ */
/*   hermes_client_ctx_t* client_ctx=hermes_client_ctx_create(ctx, (void*)handler, error_handler, handler_ctx); */
/*   HERMES_CHECK(client_ctx, return HERMES_FAIL); */
/*   client_ctx->id_=block_id; */
/*   client_ctx->user_id_=user_id; */
/*   client_ctx->rights_flag_=access_flags; */
/*   return credential_store_client_get_pub_key(ctx->credential_store_, user_id, hermes_client_set_rights_pub_key_handler_, (void*)client_ctx); */
/* } */
