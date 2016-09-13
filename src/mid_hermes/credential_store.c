/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#include <stdlib.h>
#include <string.h>
#include <hermes/utils.h>
#include <hermes/buffer.h>
#include <hermes/credential_store.h>

credential_store_client_t* credential_store_client_create(const config_t* config){
  credential_store_t* ctx=credential_store_create();
  HERMES_CHECK(ctx, return NULL);
  HERMES_CHECK(PROTOCOL_SUCCESS==credential_store_sync_connect(ctx, config->credential_store_client.endpoint, config->id), credential_store_destroy(ctx); return NULL);
  return ctx;
}

int credential_store_client_get_pub_key(credential_store_client_t* ctx, const char* id, uint8_t** key, size_t* key_length){
  uint8_t* res=NULL;
  size_t res_length;
  HERMES_CHECK(0==credential_store_get_public_key_sync_proxy(ctx, id, strlen(id)+1, &res, &res_length), return -1);
  buffer_t* buffer=buffer_create_with_data(res, res_length);
  free(res);
  assert(buffer);
  int status=0;
  const uint8_t* k;
  size_t k_length;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_status(buffer, &status) && 0==status, buffer_destroy(buffer); return -1);
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_data(buffer, &k, &k_length), buffer_destroy(buffer); return -1);
  *key=malloc(k_length);
  assert(*key);
  *key_length=k_length;
  memcpy(*key, k, k_length);
  buffer_destroy(buffer);
  return 0;
}

int credential_store_client_get_users_list(credential_store_client_t* ctx, const char* id, char** r){
  uint8_t* res=NULL;
  size_t res_length;
  HERMES_CHECK(0==credential_store_get_users_list_sync_proxy(ctx, id, strlen(id)+1, &res, &res_length), return -1);
  buffer_t* buffer=buffer_create_with_data(res, res_length);
  free(res);
  assert(buffer);
  int status=0;
  HERMES_CHECK(BUFFER_SUCCESS==buffer_pop_status(buffer, &status) && 0==status, buffer_destroy(buffer); return -1);
  const char *name;
  *r = calloc(res_length*2, 1);
  strcpy(*r, "[");
  assert(*r);
  if(BUFFER_SUCCESS==buffer_pop_string(buffer, &name)){
    sprintf(*r, "%s%s", *r, name);
    while(BUFFER_SUCCESS==buffer_pop_string(buffer, &name))
      sprintf(*r, "%s,%s", *r, name);
  }
  sprintf(*r, "%s]", *r);
  buffer_destroy(buffer);
  return 0;
}


int credential_store_client_destroy(credential_store_client_t* ctx){
  return credential_store_destroy(ctx);
}


