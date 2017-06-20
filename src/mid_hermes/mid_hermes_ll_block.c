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

#include <hermes/mid_hermes/mid_hermes_ll.h>
#include <hermes/mid_hermes/mid_hermes_ll_block.h>

#include <hermes/mid_hermes/mid_hermes_ll_rights_list.h>

#include <hermes/common/errors.h>

#include "utils.h"
#include <assert.h>
#include <string.h>

mid_hermes_ll_block_t* mid_hermes_ll_block_create_new(mid_hermes_ll_buffer_t* data,
                                                      mid_hermes_ll_buffer_t* meta,
                                                      const mid_hermes_ll_user_t* user){
  HERMES_CHECK_IN_PARAM_RET_NULL(data);
  HERMES_CHECK_IN_PARAM_RET_NULL(meta);
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  mid_hermes_ll_block_t* b=calloc(1, sizeof(mid_hermes_ll_block_t));
  assert(b);
  b->rights=mid_hermes_ll_rights_list_create();
  if(!(b->rights)){
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  mid_hermes_ll_token_t* ert=mid_hermes_ll_token_generate(mid_hermes_ll_buffer_create(NULL, 0), user);
  if(!ert){
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  mid_hermes_ll_token_t* ewt=mid_hermes_ll_token_generate(mid_hermes_ll_buffer_create(NULL, 0), user);
  if(!ewt){
    mid_hermes_ll_token_destroy(&ert);
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  if(HM_SUCCESS!=mid_hermes_ll_rights_list_rpush(b->rights, mid_hermes_ll_rights_list_node_create(user, ert,ewt))){
    mid_hermes_ll_token_destroy(&ert);
    mid_hermes_ll_token_destroy(&ewt);    
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  mid_hermes_ll_buffer_t* wt=mid_hermes_ll_token_get_data(ewt);
  if(!wt){
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  b->mac=mid_hermes_ll_buffer_create(NULL, 0);
  hermes_status_t res=hm_mac_create(wt->data, wt->length, data->data, data->length, meta->data, meta->length, &(b->mac->data), &(b->mac->length));
  mid_hermes_ll_buffer_destroy(&wt);
  if(HM_SUCCESS!=res){
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  mid_hermes_ll_buffer_t* rt=mid_hermes_ll_token_get_data(ert);
  if(!rt){
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  b->block=mid_hermes_ll_buffer_create(NULL, 0);
  res=hm_encrypt(rt->data, rt->length, data->data, data->length, meta->data, meta->length, &(b->block->data), &(b->block->length));\
  mid_hermes_ll_buffer_destroy(&rt);
  if(HM_SUCCESS!=res){
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  b->meta=meta;
  b->data=data;
  b->user=user;
  return b;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_create_new_with_id(mid_hermes_ll_buffer_t* id,
                                                              mid_hermes_ll_buffer_t* data,
                                                              mid_hermes_ll_buffer_t* meta,
                                                              const mid_hermes_ll_user_t* user){
  HERMES_CHECK_IN_PARAM_RET_NULL(id);
  HERMES_CHECK_IN_PARAM_RET_NULL(data);
  HERMES_CHECK_IN_PARAM_RET_NULL(meta);
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  mid_hermes_ll_block_t* b=mid_hermes_ll_block_create_new(data, meta, user);
  if(!b){
    return NULL;
  }
  if(!(b=mid_hermes_ll_block_set_id(b, id))){
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  return b;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_restore(mid_hermes_ll_buffer_t* id,
                                                   mid_hermes_ll_buffer_t* block,
                                                   mid_hermes_ll_buffer_t* meta,
                                                   const mid_hermes_ll_user_t* user,
                                                   mid_hermes_ll_rights_list_t* rights){
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  HERMES_CHECK_IN_PARAM_RET_NULL(id);
  HERMES_CHECK_IN_PARAM_RET_NULL(block);
  HERMES_CHECK_IN_PARAM_RET_NULL(meta);
  HERMES_CHECK_IN_PARAM_RET_NULL(rights);
  HERMES_CHECK_IN_PARAM_RET_NULL(rights->len>0);
  HERMES_CHECK_IN_PARAM_RET_NULL(mid_hermes_ll_rights_list_find(rights, user->id));
  mid_hermes_ll_block_t* b=calloc(1, sizeof(mid_hermes_ll_block_t));
  assert(b);
  b->id=id;
  b->block=block;
  b->meta=meta;
  b->rights=rights;
  b->user=user;
  return b;
}

mid_hermes_ll_buffer_t* mid_hermes_ll_block_get_data(mid_hermes_ll_block_t* b){
  HERMES_CHECK_IN_PARAM_RET_NULL(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->rights);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->rights->len>0);
  mid_hermes_ll_rights_list_node_t* ln=mid_hermes_ll_rights_list_find(b->rights, b->user->id);
  HERMES_CHECK_IN_PARAM_RET_NULL(ln);
  mid_hermes_ll_buffer_t* rt=mid_hermes_ll_token_get_data(ln->rtoken);
  HERMES_CHECK_IN_PARAM_RET_NULL(rt);
  mid_hermes_ll_buffer_t* bl=mid_hermes_ll_buffer_create(NULL,0);
  if(!bl){
    return NULL;
  }
  hermes_status_t res=hm_decrypt(rt->data, rt->length, b->block->data, b->block->length, b->meta->data, b->meta->length, &(bl->data), &(bl->length));
  mid_hermes_ll_buffer_destroy(&rt);
  if(HM_SUCCESS!=res){
    mid_hermes_ll_buffer_destroy(&bl);
    return NULL;
  }
  return bl;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_set_data(mid_hermes_ll_block_t* b, mid_hermes_ll_buffer_t* data, mid_hermes_ll_buffer_t* meta){
  HERMES_CHECK_IN_PARAM_RET_NULL(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->user);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->mac);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->rights);
  mid_hermes_ll_rights_list_node_t* ln=mid_hermes_ll_rights_list_find(b->rights, b->user->id);
  HERMES_CHECK_IN_PARAM_RET_NULL(ln);
  mid_hermes_ll_buffer_t* rt=mid_hermes_ll_token_get_data(ln->rtoken);
  HERMES_CHECK_IN_PARAM_RET_NULL(rt);
  mid_hermes_ll_buffer_t* wt=mid_hermes_ll_token_get_data(ln->wtoken);
  if(!wt){
    mid_hermes_ll_buffer_destroy(&rt);
    return NULL;
  }
  mid_hermes_ll_block_t* new_bl=mid_hermes_ll_block_create_new_with_id(b->id, data, meta, b->user);
  if(!new_bl){
    return NULL;
  }
  mid_hermes_ll_rights_list_iterator_t* i=mid_hermes_ll_rights_list_iterator_create(b->rights);
  mid_hermes_ll_rights_list_node_t* node=NULL;
  while((node=mid_hermes_ll_rights_list_iterator_next(i))){
    if(node->user->id->length != b->user->id->length || 0!=memcmp(node->user->id->data, b->user->id->data, b->user->id->length)){
      if(node->wtoken){
        if(HM_SUCCESS!=mid_hermes_ll_block_grant_update_access(new_bl, node->user)){
          new_bl->id=NULL;
          mid_hermes_ll_block_destroy(&new_bl);
          mid_hermes_ll_rights_list_iterator_destroy(&i);
          return NULL;
        }
      }else{
        if(HM_SUCCESS!=mid_hermes_ll_block_grant_read_access(new_bl, node->user)){
          new_bl->id=NULL;
          mid_hermes_ll_block_destroy(&new_bl);
          mid_hermes_ll_rights_list_iterator_destroy(&i);
          return NULL;
        }        
      }
    }
  }
  b->id=NULL;
  if(b->old_mac){
    new_bl->old_mac=b->old_mac;
    b->old_mac=NULL;    
  }else{
    new_bl->old_mac=b->mac;
    b->mac=NULL;
  }
  mid_hermes_ll_block_destroy(&b);
  return new_bl;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_grant_read_access(mid_hermes_ll_block_t* b, const mid_hermes_ll_user_t* user){
  HERMES_CHECK_IN_PARAM_RET_NULL(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->rights);
  HERMES_CHECK_IN_PARAM_RET_NULL(!mid_hermes_ll_rights_list_find(b->rights, user->id));
  mid_hermes_ll_rights_list_node_t* orig_tokens = mid_hermes_ll_rights_list_find(b->rights, b->user->id);
  HERMES_CHECK_IN_PARAM_RET_NULL(orig_tokens);
  HERMES_CHECK_IN_PARAM_RET_NULL(orig_tokens->rtoken);
  mid_hermes_ll_token_t* t=mid_hermes_ll_token_get_token_for_user(orig_tokens->rtoken, user);
  if(!t || HM_SUCCESS!=mid_hermes_ll_rights_list_rpush_val(b->rights, user, t, NULL)){
    mid_hermes_ll_token_destroy(&t);
    return NULL;
  }
  return b;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_grant_update_access_(mid_hermes_ll_block_t* b, const mid_hermes_ll_user_t* user){
  HERMES_CHECK_IN_PARAM_RET_NULL(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->mac);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->rights);
  mid_hermes_ll_rights_list_node_t* orig_tokens = mid_hermes_ll_rights_list_find(b->rights, b->user->id);
  HERMES_CHECK_IN_PARAM_RET_NULL(orig_tokens);
  HERMES_CHECK_IN_PARAM_RET_NULL(orig_tokens->rtoken);
  HERMES_CHECK_IN_PARAM_RET_NULL(orig_tokens->wtoken);
  mid_hermes_ll_rights_list_node_t* user_tokens = mid_hermes_ll_rights_list_find(b->rights, user->id);
  if(!user_tokens){
    if(HM_SUCCESS!=mid_hermes_ll_block_grant_read_access(b, user)){
      return NULL;
    }
    user_tokens = mid_hermes_ll_rights_list_find(b->rights, user->id);
  }
  HERMES_CHECK_IN_PARAM_RET_NULL(!(user_tokens->wtoken));
  user_tokens->wtoken=mid_hermes_ll_token_get_token_for_user(orig_tokens->wtoken, user);
  if(!(user_tokens->wtoken)){
    return NULL;
  }
  return b;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_grant_update_access(mid_hermes_ll_block_t* b, const mid_hermes_ll_user_t* user){
  HERMES_CHECK_IN_PARAM_RET_NULL(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  mid_hermes_ll_buffer_t* d=mid_hermes_ll_block_get_data(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(d);
  mid_hermes_ll_block_t* new_bl=mid_hermes_ll_block_set_data(b, d, b->meta);
  if(!new_bl || HM_SUCCESS!=mid_hermes_ll_block_grant_update_access_(new_bl, user)){
    new_bl->id=NULL;
    new_bl->meta=NULL;
    mid_hermes_ll_block_destroy(&new_bl);
    return NULL;
  }
  b->id=NULL;
  b->meta=NULL;
  mid_hermes_ll_block_destroy(&b);
  return new_bl;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_deny_read_access(mid_hermes_ll_block_t* b, const mid_hermes_ll_user_t* user){
  HERMES_CHECK_IN_PARAM_RET_NULL(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->rights);
  if(HM_SUCCESS!=mid_hermes_ll_rights_list_remove(b->rights, mid_hermes_ll_rights_list_find(b->rights, user->id))){
    return NULL;
  }
  mid_hermes_ll_buffer_t* d=mid_hermes_ll_block_get_data(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(d);
  mid_hermes_ll_block_t* new_bl=mid_hermes_ll_block_set_data(b, d, b->meta);
  HERMES_CHECK_IN_PARAM_RET_NULL(new_bl);
  b->id=NULL;
  b->meta=NULL;
  mid_hermes_ll_block_destroy(&b);
  return new_bl;  
}

mid_hermes_ll_block_t* mid_hermes_ll_block_deny_update_access(mid_hermes_ll_block_t* b, const mid_hermes_ll_user_t* user){
  HERMES_CHECK_IN_PARAM_RET_NULL(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->rights);
  mid_hermes_ll_rights_list_node_t* n=mid_hermes_ll_rights_list_find(b->rights, user->id);
  HERMES_CHECK_IN_PARAM_RET_NULL(n);
  mid_hermes_ll_token_destroy(&(n->wtoken));
  
  mid_hermes_ll_buffer_t* d=mid_hermes_ll_block_get_data(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(d);
  mid_hermes_ll_block_t* new_bl=mid_hermes_ll_block_set_data(b, d, b->meta);
  HERMES_CHECK_IN_PARAM_RET_NULL(new_bl);
  b->id=NULL;
  b->meta=NULL;
  mid_hermes_ll_block_destroy(&b);
  return new_bl;  
}

mid_hermes_ll_block_t* mid_hermes_ll_block_set_id(mid_hermes_ll_block_t* b, mid_hermes_ll_buffer_t* id){
  HERMES_CHECK_IN_PARAM_RET_NULL(b);
  HERMES_CHECK_IN_PARAM_RET_NULL(id);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->id);
  b->id=id;
  mid_hermes_ll_rights_list_node_t* node=NULL;
  mid_hermes_ll_rights_list_iterator_t* i=mid_hermes_ll_rights_list_iterator_create(b->rights);
  HERMES_CHECK_IN_PARAM_RET_NULL(i);
  while((node=mid_hermes_ll_rights_list_iterator_next(i))){
    if(node->rtoken){
      node->rtoken->block_id=id;
    }
    if(node->wtoken){
      node->wtoken->block_id=id;
    }
  }
  return b;
}

hermes_status_t mid_hermes_ll_block_destroy(mid_hermes_ll_block_t** b){
  HERMES_CHECK_IN_PARAM(b);
  HERMES_CHECK_IN_PARAM(*b);
  mid_hermes_ll_buffer_destroy(&((*b)->id));
  mid_hermes_ll_buffer_destroy(&((*b)->block));
  mid_hermes_ll_buffer_destroy(&((*b)->data));
  mid_hermes_ll_buffer_destroy(&((*b)->meta));
  mid_hermes_ll_buffer_destroy(&((*b)->old_mac));
  mid_hermes_ll_buffer_destroy(&((*b)->mac));
  mid_hermes_ll_rights_list_destroy(&((*b)->rights));
  free(*b);
  *b=NULL;
  return HM_SUCCESS;
}
