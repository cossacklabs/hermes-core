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
#include <hermes/mid_hermes/mid_hermes_ll_buffer.h>
#include <hermes/mid_hermes/mid_hermes_ll_block.h>
#include <hermes/mid_hermes/mid_hermes_ll_rights_list.h>
#include <hermes/mid_hermes/interfaces/key_store.h>

#include <hermes/common/errors.h>

#include "utils.h"
#include <assert.h>
#include <string.h>

mid_hermes_ll_block_t* mid_hermes_ll_block_init_empty(mid_hermes_ll_block_t* bl,
                                                      mid_hermes_ll_user_t* user){
  bl->user=user;
  return bl;
}
  
mid_hermes_ll_block_t* mid_hermes_ll_block_init(mid_hermes_ll_block_t* bl,
                                                mid_hermes_ll_user_t* user,
                                                mid_hermes_ll_buffer_t* id,
                                                mid_hermes_ll_buffer_t* block,
                                                mid_hermes_ll_buffer_t* meta,
                                                mid_hermes_ll_token_t* rtoken,
                                                mid_hermes_ll_token_t* wtoken){
  bl->user=user;
  if(!rtoken){
    bl->rtoken=mid_hermes_ll_token_generate(mid_hermes_ll_user_copy(user));
    bl->wtoken=mid_hermes_ll_token_generate(mid_hermes_ll_user_copy(user));
    if(!(bl->rtoken) || !(bl->wtoken)){
      return NULL;
    }
  }
  mid_hermes_ll_buffer_t* rt=mid_hermes_ll_token_get_data(rtoken?rtoken:(bl->rtoken));
  if(!rt){
    return NULL;
  }
  if(rtoken){
    bl->data=mid_hermes_ll_buffer_create(NULL, 0);
    if(!(bl->data) || 0!=hm_decrypt(rt->data, rt->length, block->data, block->length, meta->data, meta->length, &(bl->data->data), &(bl->data->length))){
      mid_hermes_ll_buffer_destroy(&rt);
      return NULL;
    }
  }else{
    bl->block=mid_hermes_ll_buffer_create(NULL, 0);
    if(!(bl->block) || HM_SUCCESS!=hm_encrypt(rt->data, rt->length, block->data, block->length, meta->data, meta->length, &(bl->block->data), &(bl->block->length))){
      mid_hermes_ll_buffer_destroy(&rt);
      return NULL;
    }    
  }
  mid_hermes_ll_buffer_destroy(&rt);
  if(wtoken || bl->wtoken){
    mid_hermes_ll_buffer_t* wt=mid_hermes_ll_token_get_data(wtoken?wtoken:(bl->wtoken));
    if(!wt){
      return NULL;
    }
    bl->mac=mid_hermes_ll_buffer_create(NULL,0);
    if(!(bl->mac) || HM_SUCCESS!=hm_mac_create(wt->data, wt->length, rtoken?(bl->data->data):block->data, rtoken?(bl->data->length):block->length, meta->data, meta->length, &(bl->mac->data), &(bl->mac->length))){
      mid_hermes_ll_buffer_destroy(&wt);
      return NULL;
    }
    mid_hermes_ll_buffer_destroy(&wt);
  }
  bl->id=id;
  bl->meta=meta;
  if(rtoken){
    bl->block=block;
    bl->rtoken=rtoken;
    bl->wtoken=wtoken;
  }else{
    bl->data=block;
  }
  return bl;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_update(mid_hermes_ll_block_t* bl,
                                                  mid_hermes_ll_buffer_t* data,
                                                  mid_hermes_ll_buffer_t* meta){
  if(!bl || !data || !meta || !(bl->rtoken) || !(bl->wtoken)){
    return NULL;
  }
  mid_hermes_ll_buffer_t* rt=mid_hermes_ll_token_get_data(bl->rtoken);
  mid_hermes_ll_buffer_t* new_block=mid_hermes_ll_buffer_create(NULL, 0);
  mid_hermes_ll_buffer_t* wt=mid_hermes_ll_token_get_data(bl->wtoken);
  mid_hermes_ll_buffer_t* new_mac=mid_hermes_ll_buffer_create(NULL,0);
  if(!rt
     || !wt
     || !new_block
     || !new_mac
     || (HM_SUCCESS!=hm_encrypt(rt->data, rt->length, data->data, data->length, meta->data, meta->length, &(new_block->data), &(new_block->length)))
     || (HM_SUCCESS!=hm_mac_create(wt->data, wt->length, data->data, data->length, meta->data, meta->length, &(new_mac->data), &(new_mac->length)))){
      mid_hermes_ll_buffer_destroy(&rt);
      mid_hermes_ll_buffer_destroy(&new_block);
      mid_hermes_ll_buffer_destroy(&wt);
      mid_hermes_ll_buffer_destroy(&new_mac);
      return NULL;
  }
  mid_hermes_ll_buffer_destroy(&rt);
  mid_hermes_ll_buffer_destroy(&wt);  
  mid_hermes_ll_buffer_destroy(&(bl->data));
  mid_hermes_ll_buffer_destroy(&(bl->meta));
  mid_hermes_ll_buffer_destroy(&(bl->block));
  bl->meta=meta;
  bl->data=data;
  bl->block=new_block;
  if(bl->old_mac){
    mid_hermes_ll_buffer_destroy(&(bl->mac));
  }else{
    bl->old_mac=bl->mac;
  }
  bl->mac=new_mac;
  return bl;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_rotate(mid_hermes_ll_block_t* bl,
                                                  mid_hermes_ll_rights_list_t* rights){
  if(!bl || !rights || !(bl->rtoken) || !(bl->wtoken)){
    return NULL;
  }
  mid_hermes_ll_token_t* new_rtoken=mid_hermes_ll_token_generate(bl->user);
  mid_hermes_ll_token_t* new_wtoken=mid_hermes_ll_token_generate(bl->user);
  mid_hermes_ll_buffer_t* new_block=mid_hermes_ll_buffer_create(NULL,0);
  mid_hermes_ll_buffer_t* new_mac=mid_hermes_ll_buffer_create(NULL,0);
  mid_hermes_ll_buffer_t* rt=mid_hermes_ll_token_get_data(new_rtoken);
  mid_hermes_ll_buffer_t* wt=mid_hermes_ll_token_get_data(new_wtoken);
  if(!new_rtoken
     || !new_wtoken
     || !new_block
     || !new_mac
     || !rt
     || !wt
     || (HM_SUCCESS!=hm_encrypt(rt->data, rt->length, bl->data->data, bl->data->length, bl->meta->data, bl->meta->length, &(new_block->data), &(new_block->length)))
     || (HM_SUCCESS!=hm_mac_create(wt->data, wt->length, bl->data->data, bl->data->length, bl->meta->data, bl->meta->length, &(new_mac->data), &(new_mac->length)))){
    mid_hermes_ll_token_destroy(&new_rtoken);
    mid_hermes_ll_token_destroy(&new_wtoken);
    mid_hermes_ll_buffer_destroy(&new_block);
    mid_hermes_ll_buffer_destroy(&new_mac);
    mid_hermes_ll_buffer_destroy(&rt);
    mid_hermes_ll_buffer_destroy(&wt);
    return NULL;
  }
  mid_hermes_ll_buffer_destroy(&rt);
  mid_hermes_ll_buffer_destroy(&wt);
  mid_hermes_ll_token_destroy(&(bl->rtoken));
  mid_hermes_ll_token_destroy(&(bl->wtoken)); 
  mid_hermes_ll_buffer_destroy(&(bl->block));
  bl->rtoken=new_rtoken;
  bl->wtoken=new_wtoken;
  bl->block=new_block;
  if(bl->old_mac){
    mid_hermes_ll_buffer_destroy(&(bl->mac));
  }else{
    bl->old_mac=bl->mac;
  }
  bl->mac=new_mac;

  mid_hermes_ll_rights_list_node_t* node=NULL;
  mid_hermes_ll_rights_list_iterator_t* i=mid_hermes_ll_rights_list_iterator_create(rights);
  if(!i){
    return NULL;
  }
  while((node=mid_hermes_ll_rights_list_iterator_next(i))){
    mid_hermes_ll_token_t* nrt=mid_hermes_ll_token_get_token_for_user(new_rtoken, node->user);
    mid_hermes_ll_token_t* nwt=mid_hermes_ll_token_get_token_for_user(new_wtoken, node->user);
    if(!nrt || !nwt){
      mid_hermes_ll_rights_list_iterator_destroy(&i);
      mid_hermes_ll_token_destroy(&nrt);
      mid_hermes_ll_token_destroy(&nwt);
      return NULL;
    }
    mid_hermes_ll_token_destroy(&(node->rtoken));
    mid_hermes_ll_token_destroy(&(node->wtoken));
    node->rtoken=nrt;
    node->rtoken=nwt;
  }
  mid_hermes_ll_rights_list_iterator_destroy(&i);
  return bl;
}

//store dependent functions
mid_hermes_ll_rights_list_t* mid_hermes_ll_block_access_rights(mid_hermes_ll_block_t* bl,
                                                               hermes_key_store_t* ks,
                                                               hermes_credential_store_t* cs){
  if(!bl || !(bl->id) || !(bl->mac) || !ks || !cs){
    return NULL;
  }
  mid_hermes_ll_rights_list_t* rights=mid_hermes_ll_rights_list_create();
  assert(rights);
  hermes_key_store_iterator_t* i=hermes_key_store_iterator_create(ks, bl->id->data, bl->id->length);
  assert(i);
  while(HM_SUCCESS==hermes_key_store_iterator_next(i)){
    mid_hermes_ll_user_t* user=mid_hermes_ll_user_load_c(hermes_key_store_iterator_get_user_id(i),
                                                         hermes_key_store_iterator_get_user_id_length(i),
                                                         cs);
    if(user && mid_hermes_ll_user_is_equal(user, bl->user)){
      mid_hermes_ll_user_destroy(&user);
      continue;
    }
    mid_hermes_ll_token_t* rtoken=mid_hermes_ll_rtoken_load_c(hermes_key_store_iterator_get_user_id(i),
                                                              hermes_key_store_iterator_get_user_id_length(i),
                                                              bl->id->data, bl->id->length,
                                                              ks,
                                                              cs);
    mid_hermes_ll_token_t* wtoken=mid_hermes_ll_wtoken_load_c(hermes_key_store_iterator_get_user_id(i),
                                                               hermes_key_store_iterator_get_user_id_length(i),
                                                               bl->id->data, bl->id->length,
                                                               ks,
                                                               cs);
    if(!user
       || !rtoken
       || (HM_SUCCESS != mid_hermes_ll_rights_list_rpush_val(rights, user, rtoken, wtoken))){
      mid_hermes_ll_user_destroy(&user);
      mid_hermes_ll_token_destroy(&rtoken);
      mid_hermes_ll_token_destroy(&wtoken);
      mid_hermes_ll_rights_list_create(&rights);
      hermes_key_store_iterator_destroy(&i);
      return NULL;
    }
  }
  hermes_key_store_iterator_destroy(&i);
  return rights;
}

mid_hermes_ll_token_t* mid_hermes_ll_token_load(mid_hermes_ll_user_t* user,
                                                mid_hermes_ll_buffer_t* block_id,
                                                hermes_key_store_t* ks,
                                                hermes_credential_store_t* cs,
                                                bool is_update){
  if(!user || !block_id || !ks || !cs){
    return NULL;
  }
  mid_hermes_ll_buffer_t* owner_id=mid_hermes_ll_buffer_create(NULL, 0);
  mid_hermes_ll_buffer_t* owner_pub_key=mid_hermes_ll_buffer_create(NULL, 0);
  mid_hermes_ll_buffer_t* token=mid_hermes_ll_buffer_create(NULL, 0);
  mid_hermes_ll_user_t* owner=NULL;
  mid_hermes_ll_token_t* rtoken=NULL;
  if(!owner_id
     || !owner_pub_key
     || !token
     || (HM_SUCCESS!=(is_update?hermes_key_store_get_wtoken:hermes_key_store_get_rtoken)(ks,
                                                                                         user->id->data,
                                                                                         user->id->length,
                                                                                         block_id->data,
                                                                                         block_id->length,
                                                                                         &(token->data),
                                                                                         &(token->length),
                                                                                         &(owner_id->data),
                                                                                         &owner_id->length))
     || (HM_SUCCESS != hermes_credential_store_get_public_key(cs,
                                                              owner_id->data,
                                                              owner_id->length,
                                                              &(owner_pub_key->data),
                                                              &(owner_pub_key->length)))
     || !(owner=mid_hermes_ll_user_create(owner_id, owner_pub_key))){
    mid_hermes_ll_buffer_destroy(&owner_id);
    mid_hermes_ll_buffer_destroy(&owner_pub_key);
    mid_hermes_ll_buffer_destroy(&token);
    return NULL;
  }
  if(!(rtoken=mid_hermes_ll_token_create(user, owner, token))){
    mid_hermes_ll_user_destroy(&owner);
    mid_hermes_ll_buffer_destroy(&token);
    return NULL;
  }
  return rtoken;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_load(mid_hermes_ll_block_t* bl, 
                                                mid_hermes_ll_buffer_t* id,
                                                hermes_data_store_t* ds,
                                                hermes_key_store_t* ks,
                                                hermes_credential_store_t* cs){
  if(!bl || !id || !ds || !ks || !cs){
    return NULL;
  }
  mid_hermes_ll_buffer_t* data=mid_hermes_ll_buffer_create(NULL, 0);
  mid_hermes_ll_buffer_t* meta=mid_hermes_ll_buffer_create(NULL, 0);
  mid_hermes_ll_token_t* rtoken=mid_hermes_ll_token_load(mid_hermes_ll_user_copy(bl->user), id, ks, cs, false);
  mid_hermes_ll_token_t* wtoken=mid_hermes_ll_token_load(mid_hermes_ll_user_copy(bl->user), id, ks, cs, true);
  if(!data
     || !meta
     || !rtoken
     || (HM_SUCCESS!=hermes_data_store_get_block(ds,
                                                 id->data,
                                                 id->length,
                                                 &(data->data),
                                                 &(data->length),
                                                 &(meta->data),
                                                 &(meta->length)))
     || !(bl->init(bl, bl->user, id, data, meta, rtoken, wtoken))){
    mid_hermes_ll_buffer_destroy(&data);
    mid_hermes_ll_buffer_destroy(&meta);
    mid_hermes_ll_token_destroy(&rtoken);
    mid_hermes_ll_token_destroy(&wtoken);
    return NULL;
  }
  return bl;
}

hermes_status_t mid_hermes_ll_rtoken_save(mid_hermes_ll_block_t* bl, mid_hermes_ll_token_t* t, hermes_key_store_t* ks){
  if(!t || !ks){
    return HM_FAIL;
  }
  return hermes_key_store_set_rtoken(ks,
                                     t->user->id->data,
                                     t->user->id->length,
                                     bl->id->data,
                                     bl->id->length,
                                     t->token->data,
                                     t->token->length,
                                     t->owner->id->data,
                                     t->owner->id->length);
}

hermes_status_t mid_hermes_ll_wtoken_save(mid_hermes_ll_block_t* bl, mid_hermes_ll_token_t* t, hermes_key_store_t* ks){
  if(!t || !ks){
    return HM_FAIL;
  }
  return hermes_key_store_set_wtoken(ks,
                                     t->user->id->data,
                                     t->user->id->length,
                                     bl->id->data,
                                     bl->id->length,
                                     t->token->data,
                                     t->token->length,
                                     t->owner->id->data,
                                     t->owner->id->length);
}

mid_hermes_ll_block_t* mid_hermes_ll_block_save(mid_hermes_ll_block_t* bl,
                                                mid_hermes_ll_rights_list_t* rights,
                                                hermes_data_store_t* ds,
                                                hermes_key_store_t* ks){
  if(!bl || (!ds && !ks)){
    return NULL;
  }
  if(ds){
    if(!(bl->id)){
      bl->id=mid_hermes_ll_buffer_create(NULL, 0);
      if(!(bl->id)){
        return NULL;
      }
    }
    if(HM_SUCCESS!=hermes_data_store_set_block(ds,
                                               &(bl->id->data),
                                               &(bl->id->length),
                                               bl->block->data,
                                               bl->block->length,
                                               bl->meta->data,
                                               bl->meta->length,
                                               bl->mac->data,
                                               bl->mac->length,
                                               (bl->old_mac)?bl->old_mac->data:NULL,
                                               (bl->old_mac)?bl->old_mac->length:0)){
      return NULL;
    }
    mid_hermes_ll_buffer_destroy(&(bl->old_mac));
  }
  if(ks){
    if(HM_SUCCESS!=mid_hermes_ll_rtoken_save(bl, bl->rtoken, ks)){
      return NULL;
    }
    if(HM_SUCCESS!=mid_hermes_ll_wtoken_save(bl, bl->wtoken, ks)){
      return NULL;
    }
  }
  if(rights && ks){
    mid_hermes_ll_rights_list_node_t* node;
    mid_hermes_ll_rights_list_iterator_t* i=mid_hermes_ll_rights_list_iterator_create(rights);
    if(!i){
      return NULL;
    }
    while((node=mid_hermes_ll_rights_list_iterator_next(i))){
      if(HM_SUCCESS!=mid_hermes_ll_rtoken_save(bl, node->rtoken, ks)){
        mid_hermes_ll_rights_list_iterator_destroy(&i);
        return NULL;
      }
      if(node->wtoken && HM_SUCCESS!=mid_hermes_ll_wtoken_save(bl, node->wtoken, ks)){
        mid_hermes_ll_rights_list_iterator_destroy(&i);
        return NULL;
      }
    }
    mid_hermes_ll_rights_list_iterator_destroy(&i);
  }
  return bl;
}

hermes_status_t mid_hermes_ll_rtoken_delete(mid_hermes_ll_block_t* bl, mid_hermes_ll_token_t* t, hermes_key_store_t* ks){
  if(!t || !ks){
    return HM_FAIL;
  }
  return hermes_key_store_set_rtoken(ks,
                                     t->user->id->data,
                                     t->user->id->length,
                                     bl->id->data,
                                     bl->id->length,
                                     NULL,
                                     0,
                                     t->owner->id->data,
                                     t->owner->id->length);
}

hermes_status_t mid_hermes_ll_wtoken_delete(mid_hermes_ll_block_t* bl, mid_hermes_ll_token_t* t, hermes_key_store_t* ks){
  if(!t || !ks){
    return HM_FAIL;
  }
  return hermes_key_store_set_wtoken(ks,
                                     t->user->id->data,
                                     t->user->id->length,
                                     bl->id->data,
                                     bl->id->length,
                                     NULL,
                                     0,
                                     t->owner->id->data,
                                     t->owner->id->length);
}

mid_hermes_ll_block_t* mid_hermes_ll_block_delete(mid_hermes_ll_block_t* bl,
                                                  mid_hermes_ll_rights_list_t* rights,
                                                  hermes_data_store_t* ds,
                                                  hermes_key_store_t* ks){
  if(!bl || !(bl->id) || !(bl->mac) || (!ds && !ks)){
    return NULL;
  }
  if(ds
     && HM_SUCCESS!=hermes_data_store_rem_block(ds,
                                                bl->id->data,
                                                bl->id->length,
                                                bl->old_mac?bl->old_mac->data:bl->mac->data,
                                                bl->old_mac?bl->old_mac->length:bl->mac->length)){
    return NULL;
  }
  if(ks
     && ((HM_SUCCESS!=mid_hermes_ll_rtoken_delete(bl, bl->rtoken, ks))
         || (bl->wtoken && HM_SUCCESS!=mid_hermes_ll_wtoken_delete(bl, bl->wtoken, ks)))){
    return NULL;
  }
  if(rights && ks){
    mid_hermes_ll_rights_list_node_t* node;
    mid_hermes_ll_rights_list_iterator_t* i=mid_hermes_ll_rights_list_iterator_create(rights);
    if(!i){
      return NULL;
    }
    while((node=mid_hermes_ll_rights_list_iterator_next(i))){
      if(HM_SUCCESS!=mid_hermes_ll_rtoken_delete(bl, node->rtoken, ks)){
        mid_hermes_ll_rights_list_iterator_destroy(&i);
        return NULL;
      }
      if(node->wtoken && HM_SUCCESS!=mid_hermes_ll_wtoken_delete(bl, node->wtoken, ks)){
        mid_hermes_ll_rights_list_iterator_destroy(&i);
        return NULL;
      }
    }
    mid_hermes_ll_rights_list_iterator_destroy(&i);
  }
  return bl;
}

//

mid_hermes_ll_block_t* mid_hermes_ll_block_create_empty(mid_hermes_ll_user_t* user){
  mid_hermes_ll_block_t* bl=calloc(1, sizeof(mid_hermes_ll_block_t));
  assert(bl);
  bl->init_empty=mid_hermes_ll_block_init_empty;
  bl->init=mid_hermes_ll_block_init;
  bl->update=mid_hermes_ll_block_update;
  bl->rotate=mid_hermes_ll_block_rotate;
  bl->access_rights=mid_hermes_ll_block_access_rights;
  bl->load=mid_hermes_ll_block_load;
  bl->save=mid_hermes_ll_block_save;
  bl->delete=mid_hermes_ll_block_delete;
  if(!(bl->init_empty(bl, user))){
    mid_hermes_ll_block_destroy(&bl);
  }
  return bl;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_create(mid_hermes_ll_user_t* user,
                                                  mid_hermes_ll_buffer_t* id,
                                                  mid_hermes_ll_buffer_t* block,
                                                  mid_hermes_ll_buffer_t* meta,
                                                  mid_hermes_ll_token_t* rtoken,
                                                  mid_hermes_ll_token_t* wtoken){
  mid_hermes_ll_block_t* bl=calloc(1, sizeof(mid_hermes_ll_block_t));
  assert(bl);
  bl->init_empty=mid_hermes_ll_block_init_empty;
  bl->init=mid_hermes_ll_block_init;
  bl->update=mid_hermes_ll_block_update;
  bl->rotate=mid_hermes_ll_block_rotate;
  bl->access_rights=mid_hermes_ll_block_access_rights;
  bl->load=mid_hermes_ll_block_load;
  bl->save=mid_hermes_ll_block_save;
  bl->delete=mid_hermes_ll_block_delete;
  if(!(bl->init(bl, user, id, block, meta, rtoken, wtoken))){
    mid_hermes_ll_block_destroy(&bl);
  }
  return bl;
}

hermes_status_t mid_hermes_ll_block_destroy(mid_hermes_ll_block_t** b){
  if(!b || !(*b)){
    return HM_FAIL;
  }
  mid_hermes_ll_buffer_destroy(&((*b)->id));
  mid_hermes_ll_buffer_destroy(&((*b)->data));
  mid_hermes_ll_buffer_destroy(&((*b)->block));
  mid_hermes_ll_buffer_destroy(&((*b)->meta));
  mid_hermes_ll_buffer_destroy(&((*b)->mac));
  mid_hermes_ll_user_destroy(&((*b)->user));
  mid_hermes_ll_buffer_destroy(&((*b)->old_mac));
  mid_hermes_ll_token_destroy(&((*b)->rtoken));
  mid_hermes_ll_token_destroy(&((*b)->wtoken));
  free(*b);
  *b=NULL;
  return HM_SUCCESS;
}

#if 0

mid_hermes_ll_block_t* mid_hermes_ll_block_create_new(mid_hermes_ll_buffer_t* data,
                                                      mid_hermes_ll_buffer_t* meta,
                                                      mid_hermes_ll_user_t* user){
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
  mid_hermes_ll_token_t* ert=mid_hermes_ll_token_generate(NULL, user);
  if(!ert){
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  mid_hermes_ll_token_t* ewt=mid_hermes_ll_token_generate(NULL, user);
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
  res=hm_encrypt(rt->data, rt->length, data->data, data->length, meta->data, meta->length, &(b->block->data), &(b->block->length));
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
                                                              mid_hermes_ll_user_t* user){
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
                                                   mid_hermes_ll_user_t* user,
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
  b->id=id;
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

#endif
