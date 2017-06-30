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


#ifndef MID_HERMES_LL_BLOCK_H
#define MID_HERMES_LL_BLOCK_H

#include <hermes/mid_hermes/mid_hermes_ll.h>
#include <hermes/mid_hermes/mid_hermes_ll_user.h>
#include <hermes/mid_hermes/mid_hermes_ll_token.h>
#include <hermes/mid_hermes/mid_hermes_ll_buffer.h>
#include <hermes/mid_hermes/mid_hermes_ll_rights_list.h>

#include <hermes/mid_hermes/interfaces/key_store.h>
#include <hermes/mid_hermes/interfaces/credential_store.h>
#include <hermes/mid_hermes/interfaces/data_store.h>

typedef struct mid_hermes_ll_block_type mid_hermes_ll_block_t;

struct mid_hermes_ll_block_type{
  mid_hermes_ll_user_t* user;
  mid_hermes_ll_buffer_t* id;
  mid_hermes_ll_buffer_t* block;
  mid_hermes_ll_buffer_t* data;
  mid_hermes_ll_buffer_t* meta;
  mid_hermes_ll_buffer_t* old_mac;
  mid_hermes_ll_buffer_t* mac;
  mid_hermes_ll_token_t* rtoken;
  mid_hermes_ll_token_t* wtoken;

  mid_hermes_ll_block_t*(*init_empty)(mid_hermes_ll_block_t* bl,
                                      mid_hermes_ll_user_t* user);
  
  mid_hermes_ll_block_t*(*init)(mid_hermes_ll_block_t* bl,
                                mid_hermes_ll_user_t* user,
                                mid_hermes_ll_buffer_t* id,
                                mid_hermes_ll_buffer_t* block,
                                mid_hermes_ll_buffer_t* meta,
                                mid_hermes_ll_token_t* rtoken,
                                mid_hermes_ll_token_t* wtoken);

  mid_hermes_ll_block_t*(*update)(mid_hermes_ll_block_t* bl,
                                  mid_hermes_ll_buffer_t* block,
                                  mid_hermes_ll_buffer_t* meta);
  
  mid_hermes_ll_block_t*(*rotate)(mid_hermes_ll_block_t* bl,
                                  mid_hermes_ll_rights_list_t* rights);


  mid_hermes_ll_token_t*(*rtoken_for)(mid_hermes_ll_block_t* bl,
                                      mid_hermes_ll_user_t* for_user);

  mid_hermes_ll_token_t*(*wtoken_for)(mid_hermes_ll_block_t* bl,
                                      mid_hermes_ll_user_t* for_user);

  //store dependent functions
  mid_hermes_ll_rights_list_t*(*access_rights)(mid_hermes_ll_block_t* bl,
                                               hermes_key_store_t* ks,
                                               hermes_credential_store_t* cs);
  
  mid_hermes_ll_block_t*(*load)(mid_hermes_ll_block_t* bl, 
                                mid_hermes_ll_buffer_t* id,
                                hermes_data_store_t* ds,
                                hermes_key_store_t* ks,
                                hermes_credential_store_t* cs);
  
  mid_hermes_ll_block_t*(*save)(mid_hermes_ll_block_t* bl,
                                mid_hermes_ll_rights_list_t* rights,
                                hermes_data_store_t* ds,
                                hermes_key_store_t* ks);

  mid_hermes_ll_block_t*(*delete)(mid_hermes_ll_block_t* bl,
                                  mid_hermes_ll_rights_list_t* rights,
                                  hermes_data_store_t* ds,
                                  hermes_key_store_t* ks);
};

mid_hermes_ll_block_t* mid_hermes_ll_block_create_empty(mid_hermes_ll_user_t* user);

mid_hermes_ll_block_t* mid_hermes_ll_block_create(mid_hermes_ll_user_t* user,
                                                  mid_hermes_ll_buffer_t* id,
                                                  mid_hermes_ll_buffer_t* block,
                                                  mid_hermes_ll_buffer_t* meta,
                                                  mid_hermes_ll_token_t* rtoken,
                                                  mid_hermes_ll_token_t* wtoken);

hermes_status_t mid_hermes_ll_block_destroy(mid_hermes_ll_block_t** b);

#endif //MID_HERMES_LL_BLOCK_H
