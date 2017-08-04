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



#include <hermes/mid_hermes_ll/interfaces/key_store.h>
#include "../utils/utils.h"
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h> 

#define HERMES_KEY_STORE_PATH "db/key_store"

struct hermes_key_store_type{
  char path[256];
};

hermes_key_store_t* hermes_key_store_create(){
  hermes_key_store_t* ks=calloc(1, sizeof(hermes_key_store_t));
  assert(ks);
  memcpy(ks->path, HERMES_KEY_STORE_PATH, strlen(HERMES_KEY_STORE_PATH)+1);
  create_directory(ks->path);
  return ks;
}

uint32_t hermes_key_store_destroy(hermes_key_store_t** ks){
  if(!ks || !(*ks)){
    return HM_FAIL;
  }
  free(*ks);
  *ks=NULL;
  return HM_SUCCESS;
}

uint32_t hermes_key_store_set_rtoken(hermes_key_store_t* ks,
                              const uint8_t* user_id,
                              const size_t user_id_length,
                              const uint8_t* block_id,
                              const size_t block_id_length,
                              const uint8_t* token,
                              const size_t token_length,
                              const uint8_t* owner_id,
                              const size_t owner_id_length){
  char fpath[10*1024];
  if(token){
    BUILD_TYPED_PATH(fpath, C(ks->path), E(block_id, block_id_length), E(user_id, user_id_length), C("r"));
    create_directory(fpath);
    BUILD_TYPED_PATH(fpath, C(ks->path), E(block_id, block_id_length), E(user_id, user_id_length), C("r"), C("token"));
    write_whole_file(fpath, token, token_length);
    BUILD_TYPED_PATH(fpath, C(ks->path), E(block_id, block_id_length), E(user_id, user_id_length), C("r"), C("owner"));
    write_whole_file(fpath, owner_id, owner_id_length);
  }else{
    BUILD_TYPED_PATH(fpath, C(ks->path), E(block_id, block_id_length), E(user_id, user_id_length));
    remove_directory(fpath);
  }
  return 0;
}

uint32_t hermes_key_store_set_wtoken(hermes_key_store_t* ks,
                              const uint8_t* user_id,
                              const size_t user_id_length,
                              const uint8_t* block_id,
                              const size_t block_id_length,
                              const uint8_t* token,
                              const size_t token_length,
                              const uint8_t* owner_id,
                              const size_t owner_id_length){
  char fpath[10*1024];
  if(token){
    BUILD_TYPED_PATH(fpath, C(ks->path), E(block_id, block_id_length), E(user_id, user_id_length), C("w"));
    create_directory(fpath);
    BUILD_TYPED_PATH(fpath, C(ks->path), E(block_id, block_id_length), E(user_id, user_id_length), C("w"), C("token"));
    write_whole_file(fpath, token, token_length);
    BUILD_TYPED_PATH(fpath, C(ks->path), E(block_id, block_id_length), E(user_id, user_id_length), C("w"), C("owner"));
    write_whole_file(fpath, owner_id, owner_id_length);
  }else{
    BUILD_TYPED_PATH(fpath, C(ks->path), E(block_id, block_id_length), E(user_id, user_id_length), C("w"));
    remove_directory(fpath);
  }
  return 0;
}

uint32_t hermes_key_store_get_rtoken(hermes_key_store_t* ks,
                              const uint8_t* user_id,
                              const size_t user_id_length,
                              const uint8_t* block_id,
                              const size_t block_id_length,
                              uint8_t** token,
                              size_t* token_length,
                              uint8_t** owner_id,
                              size_t* owner_id_length){
  char fpath[10*1024];
  BUILD_TYPED_PATH(fpath, C(ks->path), E(block_id, block_id_length), E(user_id, user_id_length), C("r"), C("token"));
  if(0!=read_whole_file(fpath, token, token_length)){
    return 1;
  }
  BUILD_TYPED_PATH(fpath, C(ks->path), E(block_id, block_id_length), E(user_id, user_id_length), C("r"), C("owner"));
  if(0!=read_whole_file(fpath, owner_id, owner_id_length)){
    return 1;
  }
  return 0;
}

uint32_t hermes_key_store_get_wtoken(hermes_key_store_t* ks,
                              const uint8_t* user_id,
                              const size_t user_id_length,
                              const uint8_t* block_id,
                              const size_t block_id_length,
                              uint8_t** token,
                              size_t* token_length,
                              uint8_t** owner_id,
                              size_t* owner_id_length){
  char fpath[10*1024];
  BUILD_TYPED_PATH(fpath, C(ks->path), E(block_id, block_id_length), E(user_id, user_id_length), C("w"), C("token"));
  if(0!=read_whole_file(fpath, token, token_length)){
    return 1;
  }
  BUILD_TYPED_PATH(fpath, C(ks->path), E(block_id, block_id_length), E(user_id, user_id_length), C("w"), C("owner"));
  if(0!=read_whole_file(fpath, owner_id, owner_id_length)){
    return 1;
  }
  return 0;
}

struct hermes_key_store_iterator_type{
  char path[1024];
  DIR           *d;
  struct dirent *dir;
  uint8_t user_id[2048];
  size_t user_id_length;
  uint8_t rtoken[2048];
  size_t rtoken_length;
  uint8_t rtoken_owner[2048];
  size_t rtoken_owner_length;
  uint8_t wtoken[2048];
  size_t wtoken_length;
  uint8_t wtoken_owner[2048];
  size_t wtoken_owner_length;
};

hermes_key_store_iterator_t* hermes_key_store_iterator_create(hermes_key_store_t* ks, const uint8_t* block_id, const size_t block_id_length){
  if(!ks || !block_id || !block_id_length){
    return NULL;
  }
  hermes_key_store_iterator_t* i=calloc(1, sizeof(hermes_key_store_iterator_t));
  assert(i);
  char fpath[10*1024];
  BUILD_TYPED_PATH(fpath, C(ks->path), E(block_id, block_id_length));
  i->d=opendir(fpath);
  if(!(i->d)){
    hermes_key_store_iterator_destroy(&i);
    return NULL;
  }
  strcpy(i->path, fpath);
  return i;
}

uint32_t hermes_key_store_iterator_next(hermes_key_store_iterator_t* i){
  if(!i || !(i->d)){
    return HM_FAIL;
  }
  for(;;){
    i->dir=readdir(i->d);
    if(!(i->dir)){
      return HM_FAIL;
    }
    if(0==strcmp(i->dir->d_name,".") || 0==strcmp(i->dir->d_name,"..")){
      continue;
    }
    break;
  }
  i->user_id_length=string_to_buf(i->dir->d_name, i->user_id);
  char fpath[10*1024];
  BUILD_TYPED_PATH(fpath, C(i->path), C(i->dir->d_name), C("w"), C("token"));
  FILE* f=fopen(fpath, "rb");
  if(!f){
    i->wtoken_length=0;
  }else{
    i->wtoken_length=fread(i->wtoken, 1, sizeof(i->wtoken), f);
  }
  fclose(f);
  BUILD_TYPED_PATH(fpath, C(i->path), C(i->dir->d_name), C("w"), C("owner"));
  f=fopen(fpath, "rb");
  if(!f){
    i->wtoken_owner_length=0;
  }else{
    i->wtoken_owner_length=fread(i->wtoken_owner, 1, sizeof(i->wtoken_owner), f);
  }
  fclose(f);

  BUILD_TYPED_PATH(fpath, C(i->path), C(i->dir->d_name), C("r"), C("token"));
  f=fopen(fpath, "rb");
  if(!f){
    i->rtoken_length=0;
  }else{
    i->rtoken_length=fread(i->rtoken, 1, sizeof(i->rtoken), f);
  }
  fclose(f);
  BUILD_TYPED_PATH(fpath, C(i->path), C(i->dir->d_name), C("r"), C("owner"));
  f=fopen(fpath, "rb");
  if(!f){
    i->rtoken_owner_length=0;
  }else{
    i->rtoken_owner_length=fread(i->rtoken, 1, sizeof(i->rtoken_owner), f);
  }
  fclose(f);
  return HM_SUCCESS;
}

uint8_t* hermes_key_store_iterator_get_user_id(hermes_key_store_iterator_t* i){
  return i->user_id;
}
size_t hermes_key_store_iterator_get_user_id_length(hermes_key_store_iterator_t* i){
  return i->user_id_length;
}

uint8_t* hermes_key_store_iterator_get_rtoken(hermes_key_store_iterator_t* i){
  return i->rtoken;
}
size_t hermes_key_store_iterator_get_rtoken_length(hermes_key_store_iterator_t* i){
  return i->rtoken_length;
}

uint8_t* hermes_key_store_iterator_get_rtoken_owner(hermes_key_store_iterator_t* i){
  return i->rtoken_owner;
}
size_t hermes_key_store_iterator_get_rtoken_owner_length(hermes_key_store_iterator_t* i){
  return i->rtoken_owner_length;
}

uint8_t* hermes_key_store_iterator_get_wtoken(hermes_key_store_iterator_t* i){
  return i->wtoken;
}
size_t hermes_key_store_iterator_get_wtoken_length(hermes_key_store_iterator_t* i){
  return i->wtoken_length;
}

uint8_t* hermes_key_store_iterator_get_wtoken_owner(hermes_key_store_iterator_t* i){
  return i->wtoken_owner;
}
size_t hermes_key_store_iterator_get_wtoken_owner_length(hermes_key_store_iterator_t* i){
  return i->wtoken_owner_length;
}
uint32_t hermes_key_store_iterator_destroy(hermes_key_store_iterator_t** i){
  if(!i || !(*i)){
    return HM_FAIL;
  }
  if((*i)->d){
    closedir((*i)->d);
  }
  free(*i);
  *i=NULL;
  return HM_SUCCESS;
}
