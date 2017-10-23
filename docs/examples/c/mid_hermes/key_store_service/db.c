/*
* Copyright (c) 2017 Cossack Labs Limited
*
* This file is a part of Hermes-core.
*
* Hermes-core is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Hermes-core is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Hermes-core.  If not, see <http://www.gnu.org/licenses/>.
*
*/



#include "db.h"
#include "../../utils/utils.h"
#include "../../utils/base64.h"
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h> 
#include <stdio.h>

#define HERMES_KEY_STORE_PATH "db/key_store"

#define HM_KEY_STORE_READ_ACCESS_MASK 1
#define HM_KEY_STORE_WRITE_ACCESS_MASK 2

typedef struct db_type{
  char path[256];
}db_t;

uint32_t db_set_rtoken(void* ks,
                              const uint8_t* block_id,
                              const size_t block_id_length,
                              const uint8_t* user_id,
                              const size_t user_id_length,
                              const uint8_t* owner_id,
                              const size_t owner_id_length,
                              const uint8_t* token,
                              const size_t token_length){
  char fpath[10*1024];
  if(token){
    BUILD_TYPED_PATH(fpath, C(((db_t*)ks)->path), E(block_id, block_id_length), E(user_id, user_id_length), C("r"));
    create_directory(fpath);
    BUILD_TYPED_PATH(fpath, C(((db_t*)ks)->path), E(block_id, block_id_length), E(user_id, user_id_length), C("r"), C("token"));
    write_whole_file(fpath, token, token_length);
    BUILD_TYPED_PATH(fpath, C(((db_t*)ks)->path), E(block_id, block_id_length), E(user_id, user_id_length), C("r"), C("owner"));
    write_whole_file(fpath, owner_id, owner_id_length);
  }else{
    BUILD_TYPED_PATH(fpath, C(((db_t*)ks)->path), E(block_id, block_id_length), E(user_id, user_id_length));
    remove_directory(fpath);
  }
  return 0;
}

uint32_t db_set_wtoken(void* ks,
                              const uint8_t* block_id,
                              const size_t block_id_length,
                              const uint8_t* user_id,
                              const size_t user_id_length,
                              const uint8_t* owner_id,
                              const size_t owner_id_length,
                              const uint8_t* token,
                              const size_t token_length){
  char fpath[10*1024];
  if(token){
    BUILD_TYPED_PATH(fpath, C(((db_t*)ks)->path), E(block_id, block_id_length), E(user_id, user_id_length), C("w"));
    create_directory(fpath);
    BUILD_TYPED_PATH(fpath, C(((db_t*)ks)->path), E(block_id, block_id_length), E(user_id, user_id_length), C("w"), C("token"));
    write_whole_file(fpath, token, token_length);
    BUILD_TYPED_PATH(fpath, C(((db_t*)ks)->path), E(block_id, block_id_length), E(user_id, user_id_length), C("w"), C("owner"));
    write_whole_file(fpath, owner_id, owner_id_length);
  }else{
    BUILD_TYPED_PATH(fpath, C(((db_t*)ks)->path), E(block_id, block_id_length), E(user_id, user_id_length), C("w"));
    remove_directory(fpath);
  }
  return 0;
}

uint32_t db_get_rtoken(void* ks,
                              const uint8_t* block_id,
                              const size_t block_id_length,
                              const uint8_t* user_id,
                              const size_t user_id_length,
                              uint8_t** token,
                              size_t* token_length,
                              uint8_t** owner_id,
                              size_t* owner_id_length){
  char fpath[10*1024];
  BUILD_TYPED_PATH(fpath, C(((db_t*)ks)->path), E(block_id, block_id_length), E(user_id, user_id_length), C("r"), C("token"));
  if(0!=read_whole_file(fpath, token, token_length)){
    return 1;
  }
  BUILD_TYPED_PATH(fpath, C(((db_t*)ks)->path), E(block_id, block_id_length), E(user_id, user_id_length), C("r"), C("owner"));
  if(0!=read_whole_file(fpath, owner_id, owner_id_length)){
    return 1;
  }
  return 0;
}

uint32_t db_get_wtoken(void* ks,
                              const uint8_t* block_id,
                              const size_t block_id_length,
                              const uint8_t* user_id,
                              const size_t user_id_length,
                              uint8_t** token,
                              size_t* token_length,
                              uint8_t** owner_id,
                              size_t* owner_id_length){
  char fpath[10*1024];
  BUILD_TYPED_PATH(fpath, C(((db_t*)ks)->path), E(block_id, block_id_length), E(user_id, user_id_length), C("w"), C("token"));
  if(0!=read_whole_file(fpath, token, token_length)){
    return 1;
  }
  BUILD_TYPED_PATH(fpath, C(((db_t*)ks)->path), E(block_id, block_id_length), E(user_id, user_id_length), C("w"), C("owner"));
  if(0!=read_whole_file(fpath, owner_id, owner_id_length)){
    return 1;
  }
  return 0;
}

uint32_t  db_get_indexed_rights(void* db, const uint8_t* block_id, const size_t block_id_length, const size_t index, uint8_t** user_id, size_t* user_id_length, uint32_t* rights_mask){
  if(!db || !block_id || !block_id_length || !user_id || !user_id_length || !rights_mask){
    return 1;
  }
  DIR *dir;
  struct dirent *ent;
  char fpath[10*1024];
  BUILD_TYPED_PATH(fpath, C(((db_t*)db)->path), E(block_id, block_id_length));
  if((dir=opendir(fpath))!=NULL) {
    size_t i=index;
    while((ent=readdir(dir)) != NULL) {
	if(0==strcmp(ent->d_name, ".") || 0==strcmp(ent->d_name, "..")){
	    continue;
        }
	if(!(i--)){
             *user_id=malloc(strlen(ent->d_name));
	     *user_id_length=strlen(ent->d_name);
	     *user_id_length=base64_decode(*user_id, ent->d_name, strlen(ent->d_name));
	     if(!(*user_id_length)){
		 free(*user_id);
		 closedir(dir);
		 return 1;
	     }
	     *rights_mask=HM_KEY_STORE_READ_ACCESS_MASK;
             BUILD_TYPED_PATH(fpath, C(((db_t*)db)->path), E(block_id, block_id_length), C(ent->d_name), C("w"));
	     if(0==check_file_exist(fpath)){
		*rights_mask|=HM_KEY_STORE_WRITE_ACCESS_MASK;
	     }
	     closedir(dir);
	     return 0;
        }
    }
    closedir (dir);
    return 1;
  } else {
  /* could not open directory */
  perror ("");
  return 1;
}
}


uint32_t db_del_rtoken(void* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length){
  return db_set_rtoken(db,  block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length, NULL, 0);
}

uint32_t db_del_wtoken(void* db, const uint8_t* block_id, const size_t block_id_length, const uint8_t* user_id, const size_t user_id_length, const uint8_t* owner_id, const size_t owner_id_length){
  return db_set_wtoken(db,  block_id, block_id_length, user_id, user_id_length, owner_id, owner_id_length, NULL, 0);
}

hm_ks_db_t* db_create(){
  hm_ks_db_t* ks=calloc(1, sizeof(hm_ks_db_t));
  assert(ks);
  ks->user_data=calloc(1, sizeof(db_t));
  assert(ks->user_data);
  memcpy(((db_t*)(ks->user_data))->path, HERMES_KEY_STORE_PATH, strlen(HERMES_KEY_STORE_PATH)+1);
  create_directory(((db_t*)(ks->user_data))->path);
  ks->set_rtoken=db_set_rtoken;
  ks->set_wtoken=db_set_wtoken;
  ks->get_rtoken=db_get_rtoken;
  ks->get_wtoken=db_get_wtoken;
  ks->get_indexed_rights=db_get_indexed_rights;
  ks->del_rtoken=db_del_rtoken;
  ks->del_wtoken=db_del_wtoken;
  return ks;
}

uint32_t db_destroy(hm_ks_db_t** ks){
  if(!ks || !(*ks)){
    return HM_FAIL;
  }
  free((*ks)->user_data);
  free(*ks);
  *ks=NULL;
  return HM_SUCCESS;
}
