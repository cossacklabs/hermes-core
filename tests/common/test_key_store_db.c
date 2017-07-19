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

#include "test_key_store_db.h"

#include <hermes/common/errors.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

typedef struct hm_test_ks_db_token_type{
  
};

typedef struct hm_test_ks_db_type{
  char db_dir[256];
} hm_test_ks_db_t;

hm_ks_db_t* hm_test_ks_db_create(const char* data_dir_name){
  if(!data_dir_name && strlen(data_dir_name)>=(256-1)){
    return NULL;
  }
  hm_ks_db_t* db=calloc(sizeof(hm_ks_db_t), 1);
  assert(db);
  struct stat sb;
  if (stat(data_dir_name, &sb) != 0 || !(S_ISDIR(sb.st_mode))){
    hm_test_ks_db_destroy(&db);
    return NULL;
  }
  memcpy(db->db_dir, data_dir_name, strlen(data_dir_name));
  return db;
}

uint32_t hm_test_ks_db_destroy(hm_ks_db_t** db){
  if(!db || !(*db)){
    return HM_INVALID_PARAMETER;
  }
  free(*db);
  *db=NULL;
  return HM_SUCCESS;
}

uint32_t hm_ks_db_set_rtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length, const uint8_t* rtoken, const size_t rtoken_length){
  return HM_FAIL;
}

uint32_t hm_ks_db_set_wtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length, const uint8_t* wtoken, const size_t wtoken_length){
  return HM_FAIL;
}

uint32_t hm_ks_db_get_rtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, uint8_t** rtoken, size_t* rtoken_id_length, uint8_t** owner_id, size_t* owner_id_length){
  return HM_FAIL;
}

uint32_t hm_ks_db_get_wtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, uint8_t** wtoken, size_t* wtoken_id_length, uint8_t** owner_id, size_t* owner_id_length){
  return HM_FAIL;
}

uint32_t hm_ks_db_del_rtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length){
  return HM_FAIL;
}

uint32_t hm_ks_db_del_wtoken(hm_ks_db_t* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length){
  return HM_FAIL;
}
