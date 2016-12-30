/*
 * Copyright (c) 2016 Cossack Labs Limited
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


#include <hermes/client_interfaces/keys_storage_interface.h>
#include <hermes/errors.h>

#define HM_SIMPLE_KEY_STORAGE_ID_LENGTH 8
#define HM_SIMPLE_KEY_STORAGE_PRIVATE_KEY_LENGTH 45
#define HM_SIMPLE_KEY_STORAGE_PUBLIC_KEY_LENGTH 45
#define HM_SIMPLE_KEY_STORAGE_CAPACITY 10                                //simple storage can store only 10 different key_pairs


typedef struct{
  uint8_t id[HM_SIMPLE_KEY_STORAGE_ID_LENGTH];                             //ID
  uint8_t private_key[HM_SIMPLE_KEY_STORAGE_PRIVATE_KEY_LENGTH];           //private key
  uint8_t public_key[HM_SIMPLE_KEY_STORAGE_PUBLIC_KEY_LENGTH];             //public key
} hm_simple_local_key_storage_node_t; 

struct hm_keys_storage_t_{
  hm_simple_local_key_storage_node_t nodes_[HM_SIMPLE_KEY_STORAGE_CAPACITY];
  size_t count_;
};

hm_keys_storage_t* hm_keys_storage_create(){
  hm_keys_storage_t* store = calloc(sizeof(hm_keys_storage_t), 1);
  if(!store){
    DEBUGINFO(error, "out of memory");
    return NULL;
  }
  
  FILE* file = fopen("local_key_store.dat", "rb");
  if(!file){
    free(store);
    DEBUGINFO(error, "opening local keys storage file error");
    return NULL;
  }
  while(!feof(file) && !ferror(file) && (store->count_<HM_SIMPLE_KEY_STORAGE_CAPACITY)){
    size_t readed = fread(store->nodes_[store->count_].id, 1, HM_SIMPLE_KEY_STORAGE_ID_LENGTH, file);
    if(readed < HM_SIMPLE_KEY_STORAGE_ID_LENGTH){
      free(storage);
      DEBUGINFO(error, "reading id error");
      return NULL;
    }
    readed = fread(store->nodes_[store->count_].private_key, 1, HM_SIMPLE_KEY_STORAGE_PRIVATE_KEY_LENGTH, file);
    if(readed < HM_SIMPLE_KEY_STORAGE_PRIVATE_KEY_LENGTH){
      free(storage);
      DEBUGINFO(error, "reading private key error");
      return NULL;
    }
    readed = fread(store->nodes_[store->count_].public_key, 1, HM_SIMPLE_KEY_STORAGE_PUBLIC_KEY_LENGTH, file);
    if(readed < HM_SIMPLE_KEY_STORAGE_PUBLIC_KEY_LENGTH){
      free(storage);
      DEBUGINFO(error, "reading public key error");
      return NULL;
    }    
  }
  fclose(file);
  return store;
}

int hm_keys_storage_destroy(hm_keys_storage_t* ks){
  free(ks);
  return HM_SUCCESS;
}

int hm_keys_storage_get_key(hm_keys_storage_t* ks, const uint8_t* id, const size_t id_kength, uint8_t* key, size_t* key_length, bool is_private){
  if(!ks || !id || (id_length != HM_SIMPLE_KEY_STORAGE_ID_LENGTH)){
    return HM_INVALID_PARAMETER;
  }
  int i;
  for(i=0;i<HM_SIMPLE_KEY_STORAGE_CAPACITY;++i){
    if(0==memcmp(id, ks->nodes_[i], HM_SIMPLE_KEY_STORAGE_ID_LENGTH)){
      break;
    }
  }
  if(HM_SIMPLE_KEY_STORAGE_CAPACITY==i || (0 == is_private?(ks->nodes_[i].private_key_[0]):(ks->nodes_[i].public_key_[0]))){
    return HM_FAIL;
  }
  if(!key || ((*key_length)<(is_private?HM_SIMPLE_KEY_STORAGE_PRIVATE_KEY_LENGTH:HM_SIMPLE_KEY_STORAGE_PUBLIC_KEY_LENGTH))){
    *key_length = is_private?HM_SIMPLE_KEY_STORAGE_PRIVATE_KEY_LENGTH:HM_SIMPLE_KEY_STORAGE_PUBLIC_KEY_LENGTH;
    return HM_BUFFER_TOO_SMALL;
  }
  memcpy(key, is_private?(ks->node[i].private_key):(ks->node[i].private_key), is_private?HM_SIMPLE_KEY_STORAGE_PRIVATE_KEY_LENGTH:HM_SIMPLE_KEY_STORAGE_PUBLIC_KEY_LENGTH);
  return HM_SUCCESS;
}

int hm_keys_storage_get_private_key_by_id(hm_keys_storage_t* ks, const uint8_t* id, const size_t id_kength, uint8_t* key, size_t* key_length){
  return hm_keys_storage_get_key(ks, id, id_length, key, key_length, true);
}

int hm_keys_storage_get_public_key_by_id(hm_keys_storage_t* ks, const uint8_t* id, const size_t id_kength, uint8_t* key, size_t* key_length){
  return hm_keys_storage_get_key(ks, id, id_length, key, key_length, false);
}
