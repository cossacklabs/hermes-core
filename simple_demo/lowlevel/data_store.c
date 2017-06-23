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

#include "data_store.h"

#include "utils.h"
#include <assert.h>
#include <soter/soter.h>
#include <string.h>

#define DATA_STORE_PATH "db/data_store"
#define DS_SUCCESS 0
#define DS_FAIL 1

#define DEFAULT_BLOCK_ID_LENGTH 16

struct data_store_type{
  char path[1024];
};

data_store_t* data_store_create(){
  data_store_t* ds=calloc(1, sizeof(data_store_t));
  assert(ds);
  memcpy(ds->path, DATA_STORE_PATH, strlen(DATA_STORE_PATH)+1);
  create_directory(ds->path);
  return ds;
}

uint32_t data_store_add_new_block(data_store_t* ds,
                                 uint8_t** id,
                                 size_t* id_length,
                                 const uint8_t* data,
                                 const size_t data_length,
                                 const uint8_t* meta,
                                 const size_t meta_length,
                                 const uint8_t* mac,
                                 const size_t mac_length){
  if(!id || !id_length){
    return DS_FAIL; 
  }
  if(!(*id) || !(*id_length)){
    *id=malloc(DEFAULT_BLOCK_ID_LENGTH);
    assert(*id);
    if(SOTER_SUCCESS!=soter_rand(*id,  DEFAULT_BLOCK_ID_LENGTH)){
      free(*id);
      return DS_FAIL;
    }
    *id_length=DEFAULT_BLOCK_ID_LENGTH;
  }
  char fpath[10*1024];
  BUILD_TYPED_PATH(fpath, C(ds->path), E(*id, *id_length), C("data"));
  if(0!=write_whole_file(fpath, data, data_length)){
    return DS_FAIL;
  }
  BUILD_TYPED_PATH(fpath, C(ds->path), E(*id, *id_length), C("meta"));
  if(0!=write_whole_file(fpath, meta, meta_length)){
    return DS_FAIL;
  }
  BUILD_TYPED_PATH(fpath, C(ds->path), E(*id, *id_length), C("mac"));
  return write_whole_file(fpath, mac, mac_length);
}

uint8_t data_store_update_block(data_store_t* ds,
                                const uint8_t* id,
                                const size_t id_length,
                                const uint8_t* data,
                                const size_t data_length,
                                const uint8_t* meta,
                                const size_t meta_length,
                                const uint8_t* mac,
                                const size_t mac_length,
                                const uint8_t* old_mac,
                                const size_t old_mac_length){
  if(!ds || !id || !id_length){
    return DS_FAIL;
  }
  uint8_t* stored_mac=NULL;
  size_t stored_mac_length=0;
  char fpath[10*1024];
  /*very important part*/
  BUILD_TYPED_PATH(fpath, C(ds->path), E(id, id_length), C("mac"));
  if(0!=read_whole_file(fpath, &stored_mac, &stored_mac_length)){
    return DS_FAIL;
  }
  if(stored_mac_length!=old_mac_length || 0!=memcmp(stored_mac, old_mac, stored_mac_length)){
    return DS_FAIL;
  }
  /*end of very important part*/
  if(data){
    BUILD_TYPED_PATH(fpath, C(ds->path), E(id, id_length), C("data"));
    if(0!=write_whole_file(fpath, data, data_length)){
      return DS_FAIL;
    }
    BUILD_TYPED_PATH(fpath, C(ds->path), E(id, id_length), C("meta"));
    if(0!=write_whole_file(fpath, meta, meta_length)){
      return DS_FAIL;
    }
    BUILD_TYPED_PATH(fpath, C(ds->path), E(id, id_length), C("mac"));
    return write_whole_file(fpath, mac, mac_length);
  }else{
    BUILD_TYPED_PATH(fpath, C(ds->path), E(id, id_length));
    remove_directory(fpath);
  }
}

uint8_t data_store_set_block(data_store_t* ds,
                             uint8_t** id,
                             size_t* id_length,
                             const uint8_t* data,
                             const size_t data_length,
                             const uint8_t* meta,
                             const size_t meta_length,
                             const uint8_t* mac,
                             const size_t mac_length,
                             const uint8_t* old_mac,
                             const size_t old_mac_length){
  if(!ds || !id || !data || !data_length || !meta || !meta_length || !mac || !mac_length){
    return DS_FAIL;
  }
  char fpath[10*1024];
  if(*id && *id_length){
    BUILD_TYPED_PATH(fpath, C(ds->path), E(*id, *id_length));
    if(!check_file_exist(fpath) && 0!=data_store_add_new_block(ds, id, id_length, data, data_length, meta, meta_length, mac,mac_length)){
      return DS_FAIL;
    }
    return data_store_update_block(ds, *id, *id_length, data, data_length, meta, meta_length, mac, mac_length, old_mac, old_mac_length);
  }
  return data_store_add_new_block(ds, id, id_length, data, data_length, meta, meta_length, mac,mac_length);
}

uint8_t data_store_get_block(data_store_t* ds,
                             const uint8_t* id,
                             const size_t id_length,
                             uint8_t** data,
                             size_t* data_length,
                             uint8_t** meta,
                             size_t* meta_length){
  if(!ds || !id || !id_length || !data || !data_length || !meta || !meta_length){
    return DS_FAIL;
  }
  char fpath[10*1024];
  BUILD_TYPED_PATH(fpath, C(ds->path), E(id, id_length), C("data"));
  if(0!=read_whole_file(fpath, data, data_length)){
    return DS_FAIL;
  }
  BUILD_TYPED_PATH(fpath, C(ds->path), E(id, id_length), C("meta"));
  if(0!=read_whole_file(fpath, meta, meta_length)){
    free(data);
    return DS_FAIL;
  }
  return DS_SUCCESS;
}

uint8_t data_store_rem_block(data_store_t* ds,
                             const uint8_t* id,
                             const size_t id_length,
                             const uint8_t* old_mac,
                             const size_t old_mac_length){
  return data_store_update_block(ds, id, id_length, NULL, 0, NULL, 0, NULL, 0, old_mac, old_mac_length);
}


uint32_t data_store_destroy(data_store_t** ds){
  if(!ds || !(*ds)){
    return DS_FAIL;
  }
  free(*ds);
  *ds=NULL;
  return DS_SUCCESS;
}

