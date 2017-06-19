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

#include <hermes/common/errors.h>

#include "utils.h"
#include <assert.h>

mid_hermes_ll_block_t* mid_hermes_ll_block_create_new(mid_hermes_ll_buffer_t* data,
                                                      mid_hermes_ll_buffer_t* meta,
                                                      const mid_hermes_ll_user_t* user){
  HERMES_CHECK_IN_PARAM_RET_NULL(data);
  HERMES_CHECK_IN_PARAM_RET_NULL(meta);
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  mid_hermes_ll_block_t* b=calloc(1, sizeof(mid_hermes_ll_block_t));
  assert(b);
  b->rtoken = mid_hermes_ll_token_generate(mid_hermes_ll_buffer_create(NULL, 0), user);
  b->wtoken = mid_hermes_ll_token_generate(mid_hermes_ll_buffer_create(NULL, 0), user);
  if(!(b->rtoken) || !(b->wtoken)){
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  mid_hermes_ll_buffer_t* wt=mid_hermes_ll_token_get_data(b->wtoken);
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
  mid_hermes_ll_buffer_t* rt=mid_hermes_ll_token_get_data(b->rtoken);
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
  hermes_status_t res=mid_hermes_ll_block_set_id(b, id);
  if(HM_SUCCESS!=res){
    mid_hermes_ll_block_destroy(&b);
    return NULL;
  }
  return b;
}

mid_hermes_ll_block_t* mid_hermes_ll_block_restore(mid_hermes_ll_buffer_t* id,
                                                   mid_hermes_ll_buffer_t* block,
                                                   mid_hermes_ll_buffer_t* meta,
                                                   const mid_hermes_ll_user_t* user,
                                                   mid_hermes_ll_token_t* rtoken,
                                                   mid_hermes_ll_token_t* wtoken){
  HERMES_CHECK_IN_PARAM_RET_NULL(id);
  HERMES_CHECK_IN_PARAM_RET_NULL(block);
  HERMES_CHECK_IN_PARAM_RET_NULL(meta);
  HERMES_CHECK_IN_PARAM_RET_NULL(rtoken);
  mid_hermes_ll_block_t* b=calloc(1, sizeof(mid_hermes_ll_block_t));
  assert(b);
  b->id=id;
  b->block=block;
  b->meta=meta;
  b->rtoken=rtoken;
  b->wtoken=wtoken;
  return b;
}

mid_hermes_ll_buffer_t* mid_hermes_ll_block_get_data(mid_hermes_ll_block_t* b){
  HERMES_CHECK_IN_PARAM_RET_NULL(b);
  if(!(b->rtoken)){
    return NULL;
  }
  mid_hermes_ll_buffer_t* rt=mid_hermes_ll_token_get_data(b->rtoken);
  if(!rt){
    return NULL;
  }
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
  HERMES_CHECK_IN_PARAM_RET_NULL(b->rtoken);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->wtoken);
  HERMES_CHECK_IN_PARAM_RET_NULL(b->mac);
  mid_hermes_ll_block_t* new_bl=mid_hermes_ll_block_create_new_with_id(b->id, data, meta, b->wtoken->user);
  if(!new_bl){
    return NULL;
  }
  b->id=NULL;
  new_bl->old_mac=b->mac;
  b->mac=NULL;
  mid_hermes_ll_block_destroy(&b);
  return new_bl;
}

hermes_status_t mid_hermes_ll_block_set_id(mid_hermes_ll_block_t* b, mid_hermes_ll_buffer_t* id){
  HERMES_CHECK_IN_PARAM(b);
  HERMES_CHECK_IN_PARAM(id);
  HERMES_CHECK_IN_PARAM(b->id);
  HERMES_CHECK_IN_PARAM(b->rtoken);
  HERMES_CHECK_IN_PARAM(b->wtoken);
  b->rtoken->block_id=id;
  b->wtoken->block_id=id;
  b->id=id;
  return HM_SUCCESS;
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
  mid_hermes_ll_token_destroy(&((*b)->rtoken));
  mid_hermes_ll_token_destroy(&((*b)->wtoken));
  free(*b);
  *b=NULL;
  return HM_SUCCESS;
}
