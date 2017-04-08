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

#include "test_credential_store_db.h"

#include <hermes/common/errors.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct hm_cs_test_db_type{
  char name[10][255];
  char key[10][255];
  uint32_t count;
}hm_cs_test_db_t;

uint32_t hm_cs_test_db_get_pub_by_id(void* db, const uint8_t* id, const size_t id_length, uint8_t** key, size_t* key_length){
  if(!db || !id || !id_length || !key){
    return HM_INVALID_PARAMETER;
  }
  int i=0;
  for(;i<((hm_cs_test_db_t*)db)->count;++i){
    if(0==strcmp((const char*)id, ((hm_cs_test_db_t*)db)->name[i])){
      *key=(uint8_t*)(((hm_cs_test_db_t*)db)->key[i]);
      *key_length=strlen((const char*)(*key))+1;
      return HM_SUCCESS;
    }
  }
  return HM_FAIL;
}


hm_cs_db_t* hm_test_cs_db_create(const char* filename){
  if(!filename){
    return NULL;
  }
  hm_cs_db_t* db=calloc(sizeof(hm_cs_db_t), 1);
  assert(db);
  db->user_data=calloc(sizeof(hm_cs_test_db_t),1);
  assert(db->user_data);
  FILE* file=fopen(filename, "r");
  if(!file){
    free(db->user_data);
    free(db);
    return NULL;
  }
  while(fscanf(file, "%s %s", ((hm_cs_test_db_t*)(db->user_data))->name[((hm_cs_test_db_t*)(db->user_data))->count], ((hm_cs_test_db_t*)(db->user_data))->key[((hm_cs_test_db_t*)(db->user_data))->count])==2){
    ++(((hm_cs_test_db_t*)(db->user_data))->count);
  }
  fclose(file);
  db->get_pub=hm_cs_test_db_get_pub_by_id;
  return db;
}

uint32_t hm_test_cs_db_destroy(hm_cs_db_t** db){
  if(!db || !(*db)){
    return HM_INVALID_PARAMETER;
  }
  free((*db)->user_data);
  free(*db);
  *db=NULL;
  return HM_SUCCESS;
}

