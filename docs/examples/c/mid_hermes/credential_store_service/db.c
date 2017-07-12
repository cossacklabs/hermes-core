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

#include <hermes/credential_store/db.h>
#include <string.h>
#include "../../utils/utils.h"
#include <assert.h>

#define CREDENTIAL_STORE_PATH "db/credential_store"
#define CS_SUCCESS 0
#define CS_FAIL 1

typedef struct db_type{
  char path[1024];
}db_t;

uint32_t db_get_public_key(void* cs, const uint8_t* user_id, const size_t user_id_length, uint8_t** pub_key, size_t* pub_key_length){
  char fpath[10*1024];
  BUILD_TYPED_PATH(fpath, C(((db_t*)cs)->path), E(user_id, user_id_length));
  return read_whole_file(fpath, pub_key, pub_key_length);  
}

hm_cs_db_t* db_create(){
  hm_cs_db_t* cs=calloc(1, sizeof(hm_cs_db_t));
  assert(cs);
  cs->user_data=calloc(1, sizeof(db_t));
  assert(cs->user_data);
  cs->get_pub=db_get_public_key;
  memcpy(((db_t*)(((hm_cs_db_t*)cs)->user_data))->path, CREDENTIAL_STORE_PATH, strlen(CREDENTIAL_STORE_PATH)+1);
  create_directory(((db_t*)(((hm_cs_db_t*)cs)->user_data))->path);
  return cs;
}


uint32_t db_destroy(hm_cs_db_t** cs){
  if(!cs || !(*cs)){
    return CS_FAIL;
  }
  free((*cs)->user_data);
  free(*cs);
  *cs=NULL;
  return CS_SUCCESS;
}

