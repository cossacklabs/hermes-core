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

#include "commands.h"
#include "key_store.h"
#include "credential_store.h"
#include "data_store.h"
#include "utils.h"
#include <hermes/mid_hermes/mid_hermes_ll_user.h>
#include <hermes/mid_hermes/mid_hermes_ll_block.h>
#include <hermes/mid_hermes/mid_hermes_ll_rights_list.h>

#include <assert.h>
#include <string.h>

extern key_store_t* ks;
extern credential_store_t* cs;
extern data_store_t* ds;

int write_block(mid_hermes_ll_block_t* bl){
  if(!bl){
    return 1;
  }
  if(0!=data_store_set_block(ds, &(bl->id->data), &(bl->id->length), bl->block->data, bl->block->length, bl->meta->data, bl->meta->length, bl->mac->data, bl->mac->length, bl->old_mac->data, bl->old_mac->length)){
    return 1;
  }
  mid_hermes_ll_rights_list_iterator_t* i=mid_hermes_ll_rights_list_iterator_create(bl->rights);
  mid_hermes_ll_rights_list_node_t* node;
  while((node=mid_hermes_ll_rights_list_iterator_next(i))){
    if(node->rtoken && 0!=key_store_set_rtoken(ks, node->user->id->data, node->user->id->length, bl->id->data, bl->id->length, node->rtoken->token->data, node->rtoken->token->length, node->rtoken->owner->id->data, node->rtoken->owner->id->length)){
      return 1;
    }
    if(node->wtoken && 0!=key_store_set_wtoken(ks, node->user->id->data, node->user->id->length, bl->id->data, bl->id->length, node->wtoken->token->data, node->wtoken->token->length, node->wtoken->owner->id->data, node->wtoken->owner->id->length)){
      return 1;
    }    
  }
  return 0;
}

mid_hermes_ll_user_t* create_user(const char* user_id, const char* user_sk){
  mid_hermes_ll_buffer_t* pk=mid_hermes_ll_buffer_create(NULL,0);
  assert(pk);
  if(0!=credential_store_get_public_key(cs, user_id, strlen(user_id)+1, &(pk->data), &(pk->length))){
    mid_hermes_ll_buffer_destroy(&pk);
    return NULL;
  }
  uint8_t ssk[2048];
  size_t ssk_length=string_to_buf(user_sk, ssk);
  if(!ssk_length){
    mid_hermes_ll_buffer_destroy(&pk);
    return NULL;
  }
  return mid_hermes_ll_local_user_create(mid_hermes_ll_buffer_create(user_id, strlen(user_id)+1),
                                         mid_hermes_ll_buffer_create(ssk, ssk_length),
                                         pk);
}

int add_block(const char* user_id, const char* user_sk, const char* block_file_name, const char* block_meta_data){
  mid_hermes_ll_user_t* u=create_user(user_id, user_sk);
  assert(u);
  mid_hermes_ll_buffer_t* data=mid_hermes_ll_buffer_create(NULL, 0);
  if(0!=read_whole_file(block_file_name, &(data->data), &(data->length))){
    mid_hermes_ll_buffer_destroy(&data);
    return 1;
  }
  mid_hermes_ll_block_t* bl=mid_hermes_ll_block_create_new_with_id(mid_hermes_ll_buffer_create(block_file_name, strlen(block_file_name)+1), data, mid_hermes_ll_buffer_create(block_meta_data, strlen(block_meta_data)+1), u);
  if(!bl){
    mid_hermes_ll_user_destroy(&u);
    mid_hermes_ll_buffer_destroy(&data);
    return 1;
  }
  if(0!=write_block(bl)){
    mid_hermes_ll_block_destroy(&bl);
    return 1;
  }
  mid_hermes_ll_block_destroy(&bl);
  return 0;
}

int get_block(const char* user_id, const char* user_sk, const char* block_file_name){
  mid_hermes_ll_user_t* u=create_user(user_id, user_sk);
  assert(u);
}
