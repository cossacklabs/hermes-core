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

#include "utils.h"
#include <assert.h>

mid_hermes_ll_block_t* mid_hermes_ll_block_create_new(mid_hermes_ll_buffer_t* data;
                                                      mid_hermes_ll_buffer_t* meta,
                                                      const mid_hermes_ll_user_t* user){
  HERMES_CHECK_IN_PARAM(data);
  HERMES_CHECK_IN_PARAM(meta);
  HERMES_CHECK_IN_PARAM(user);
  
}

mid_hermes_ll_block_t* mid_hermes_ll_block_create_new_with_id(mid_hermes_ll_buffer_t* id;
                                                              mid_hermes_ll_buffer_t* data;
                                                              mid_hermes_ll_buffer_t* meta,
                                                              const mid_hermes_ll_user_t* user){

}

mid_hermes_ll_block_t* mid_hermes_ll_block_restore(mid_hermes_ll_buffer_t* id;
                                                   mid_hermes_ll_buffer_t* block;
                                                   mid_hermes_ll_buffer_t* meta,
                                                   mid_hermes_ll_token_t* rtoken,
                                                   mid_hermes_ll_token_t* wtoken){
  HERMES_CHECK_IN_PARAM(id);
  HERMES_CHECK_IN_PARAM(block);
  HERMES_CHECK_IN_PARAM(meta);
  HERMES_CHECK_IN_PARAM(rtoren);
  mid_hermes_ll_block_t* b=calloc(1, sizeof(mid_hermes_ll_block_t));
  assert(b);
  b->id=id;
  b->block=block;
  b->meta=meta;
  b->rtoken=rtoken;
  b->wtoken=wtoken;
}

mid_hermes_ll_buffer_t* mid_hermes_ll_block_get_data(mid_hermes_ll_block_t* b){
  HERMES_CHECK_IN_PARAM(b);
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
  if(HM_SUCCESS!=res){
    
  }

}

mid_hermes_ll_buffer_t* mid_hermes_ll_block_set_data(mid_hermes_ll_block_t* b, const mid_hermes_ll_buffer_t* data, const mid_hermes_ll_buffer_t* meta){

}

mid_hermes_ll_buffer_t* mid_hermes_ll_block_set_id(mid_hermes_ll_block_t* b, const mid_hermes_ll_buffer_t* id){

}

hermes_status_t mid_hermes_ll_block_destroy(mid_hermes_ll_block_t** b){

}
