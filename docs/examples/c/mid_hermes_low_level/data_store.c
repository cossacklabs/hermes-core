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

#include <hermes/mid_hermes_ll/interfaces/data_store.h>

#include "../utils/utils.h"
#include <assert.h>
#include <soter/soter.h>
#include <string.h>

#define DATA_STORE_PATH "db/data_store"
#define DS_SUCCESS 0
#define DS_FAIL 1

#define DEFAULT_BLOCK_ID_LENGTH 16

struct hermes_data_store_type{
  char path[1024];
};

hermes_data_store_t* hermes_data_store_create(){
  hermes_data_store_t* ds=calloc(1, sizeof(hermes_data_store_t));
  assert(ds);
  memcpy(ds->path, DATA_STORE_PATH, strlen(DATA_STORE_PATH)+1);
  create_directory(ds->path);
  return ds;
}

hermes_status_t hermes_data_store_set_block(hermes_data_store_t* ds,
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
  if(!ds || !id || !id_length){
    return DS_FAIL; 
  }
  char fpath[10*1024];
  uint8_t* stored_mac=NULL;
  size_t stored_mac_length=0;
  if(!(*id)){
    *id=malloc(DEFAULT_BLOCK_ID_LENGTH);
    assert(*id);
    if(SOTER_SUCCESS!=soter_rand(*id,  DEFAULT_BLOCK_ID_LENGTH)){
      return DS_FAIL;
    }
    *id_length=DEFAULT_BLOCK_ID_LENGTH;
  }else{
    BUILD_TYPED_PATH(fpath, C(ds->path), E(*id, *id_length), C("mac"));
    if(0!=read_whole_file(fpath, &stored_mac, &stored_mac_length)){
      stored_mac=NULL;
    }
  }
  /*very important part*/
  if(stored_mac){
    if(!old_mac || stored_mac_length!=old_mac_length || 0!=memcmp(stored_mac, old_mac, stored_mac_length)){
      free(stored_mac);
      return DS_FAIL;
    }    
  }
  free(stored_mac);
  BUILD_TYPED_PATH(fpath, C(ds->path), E(*id, *id_length));
  create_directory(fpath);
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

hermes_status_t hermes_data_store_get_block(hermes_data_store_t* ds,
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
    free(*data);
    *data=NULL;
    return DS_FAIL;
  }
  return DS_SUCCESS;
}

hermes_status_t hermes_data_store_rem_block(hermes_data_store_t* ds,
                                    const uint8_t* id,
                                    const size_t id_length,
                                    const uint8_t* old_mac,
                                    const size_t old_mac_length){
  if(!ds || !id || !id_length || !old_mac || !old_mac_length){
    return DS_FAIL; 
  }
  char fpath[10*1024];
  uint8_t* stored_mac=NULL;
  size_t stored_mac_length=0;
  BUILD_TYPED_PATH(fpath, C(ds->path), E(id, id_length), C("mac"));
  if(0!=read_whole_file(fpath, &stored_mac, &stored_mac_length)){
    return DS_FAIL;;
  }
  /*very important part*/
  if(!old_mac || stored_mac_length!=old_mac_length || 0!=memcmp(stored_mac, old_mac, stored_mac_length)){
    free(stored_mac);
    return DS_FAIL;
  }    
  free(stored_mac);
  BUILD_TYPED_PATH(fpath, C(ds->path), E(id, id_length));
  if(0!=remove_directory(fpath)){
    return HM_FAIL;
  }
  return HM_SUCCESS;
}


hermes_status_t hermes_data_store_destroy(hermes_data_store_t** ds){
  if(!ds || !(*ds)){
    return DS_FAIL;
  }
  free(*ds);
  *ds=NULL;
  return DS_SUCCESS;
}
