/*
 * Copyright (c) 2017 Cossack Labs Limited
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

#include "credential_store.h"
#include <string.h>
#include "utils.h"
#include <assert.h>

#define CREDENTIAL_STORE_PATH "db/credential_store"
#define CS_SUCCESS 0
#define CS_FAIL 1

struct credential_store_type{
  char path[1024];
};

credential_store_t* credential_store_create(){
  credential_store_t* cs=calloc(1, sizeof(credential_store_t));
  assert(cs);
  memcpy(cs->path, CREDENTIAL_STORE_PATH, strlen(CREDENTIAL_STORE_PATH)+1);
  create_directory(cs->path);
  return cs;
}

uint32_t credential_store_get_public_key(credential_store_t* cs, const uint8_t* user_id, const size_t user_id_length, uint8_t** pub_key, size_t* pub_key_length){
  char fpath[10*1024];
  BUILD_TYPED_PATH(fpath, C(cs->path), E(user_id, user_id_length));
  return read_whole_file(fpath, pub_key, pub_key_length);  
}

uint32_t credential_store_destroy(credential_store_t** cs){
  if(!cs || !(*cs)){
    return CS_FAIL;
  }
  free(*cs);
  *cs=NULL;
  return CS_SUCCESS;
}

