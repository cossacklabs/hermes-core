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

#include <hermes/common/hash_table.h>
#include <hermes/common/errors.h>

#include <assert.h>
#include <stdint.h>
#include <string.h>

#define HM_HASH_TABLE_CAP 1024  //2^10

typedef struct hm_hash_table_entry_type hm_hash_table_entry_t;

struct hm_hash_table_entry_type{
  uint8_t* key;
  size_t key_length;
  uint8_t* val;
  size_t val_length;
  hm_hash_table_entry_t* next;
};


hm_hash_table_entry_t* hm_hash_table_entry_create(const uint8_t* key, const size_t key_length, const uint8_t* val, const size_t val_length){
  hm_hash_table_entry_t *e = calloc(sizeof(hm_hash_table_entry_t), 1);
  assert(e);
  e->key=malloc(key_length);
  assert(e->key);
  memcpy(e->key, key, key_length);
  e->key_length=key_length;
  e->val=malloc(val_length);
  assert(e->val);
  memcpy(e->val, val, val_length);
  e->val_length=val_length;
  return e;
}

uint32_t hm_hash_table_entry_destroy(hm_hash_table_entry_t* e){
  if(!e){
    return HM_INVALID_PARAMETER;
  }
  if(e->next){
    hm_hash_table_entry_destroy(e->next);
    free(e->next);
  }
  free(e->key);
  free(e->val);
  free(e);
  return HM_SUCCESS;
}

int hm_hash_(const uint8_t *key, const size_t key_length) {
  unsigned long hash = 5381;
  int i=0;
  for(;i<key_length;++i){
    hash = ((hash << 5) + hash) + key[i]; /* hash * 33 + car */
  }
  return hash%HM_HASH_TABLE_CAP;
}

struct hm_hash_table_type{
  hm_hash_table_entry_t* t[HM_HASH_TABLE_CAP];
};

hm_hash_table_t* hm_hash_table_create(){
  hm_hash_table_t* t = calloc(sizeof(hm_hash_table_t),1);
  assert(t);
  return t;
}

uint32_t hm_hash_table_destroy(hm_hash_table_t** t){
  if(!t || !(*t)){
    return HM_INVALID_PARAMETER;
  }
  uint32_t i=0;
  for(;i<HM_HASH_TABLE_CAP;++i){
    hm_hash_table_entry_destroy((*t)->t[i]);
  }
  free(*t);
  *t=NULL;
  return HM_SUCCESS;
}

uint32_t hm_hash_table_set(hm_hash_table_t* t, const uint8_t* key, const size_t key_length, const uint8_t* val, const size_t val_length){
  if(!t || !key || !key_length || !val || !val_length){
    return HM_INVALID_PARAMETER;
  }
  int hash=hm_hash_(key, key_length); 
  hm_hash_table_entry_t *last=NULL, *next = t->t[hash];
  while(next && next->key && memcmp(key, next->key, key_length)>0){
    last = next;
    next = next->next;
  }
  if(next && next->key && 0==memcmp(key, next->key, key_length)){
    return HM_FAIL; //key already exist
  } else {
    hm_hash_table_entry_t* e = hm_hash_table_entry_create(key, key_length, val, val_length);
    if(next==t->t[hash] ) {
      e->next=next;
      t->t[hash]=e;
    }else if(next==NULL){
      last->next=e;
    }else{
      e->next = next;
      last->next = e;
    }
  }
  return HM_SUCCESS;
}

uint32_t hm_hash_table_get(hm_hash_table_t* t, const uint8_t* key, const size_t key_length, uint8_t** val, size_t* val_length){
  if(!t || !key || !key_length || !val || !val_length){
    return HM_INVALID_PARAMETER;
  }
  int hash=hm_hash_(key, key_length);
  hm_hash_table_entry_t* e = t->t[hash];
  while(e && e->key && memcmp(key, e->key, key_length)>0){
    e = e->next;
  }
  if(!e || !(e->key) || 0!=memcmp(key, e->key, key_length)) {
    return HM_FAIL;
  }
  *val=e->val;
  *val_length=e->val_length;
  return HM_SUCCESS;
}

