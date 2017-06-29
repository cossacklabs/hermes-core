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

#include <hermes/common/errors.h>
#include <hermes/mid_hermes/mid_hermes_ll_user.h>

#include "utils.h"
#include <assert.h>

mid_hermes_ll_user_t* mid_hermes_ll_local_user_create(mid_hermes_ll_buffer_t* id,
                                                      mid_hermes_ll_buffer_t* sk,
                                                      mid_hermes_ll_buffer_t* pk){
  HERMES_CHECK_IN_PARAM_RET_NULL(id);
  HERMES_CHECK_IN_PARAM_RET_NULL(!mid_hermes_ll_buffer_is_empty(id));
  HERMES_CHECK_IN_PARAM_RET_NULL(sk);
  HERMES_CHECK_IN_PARAM_RET_NULL(!mid_hermes_ll_buffer_is_empty(sk));
  HERMES_CHECK_IN_PARAM_RET_NULL(pk);
  HERMES_CHECK_IN_PARAM_RET_NULL(!mid_hermes_ll_buffer_is_empty(pk));
  mid_hermes_ll_user_t* u=mid_hermes_ll_user_create(id, pk);
  u->sk=sk;
  return u;
}

mid_hermes_ll_user_t* mid_hermes_ll_user_create(mid_hermes_ll_buffer_t* id,
                                                mid_hermes_ll_buffer_t* pk){
  HERMES_CHECK_IN_PARAM_RET_NULL(id);
  HERMES_CHECK_IN_PARAM_RET_NULL(!mid_hermes_ll_buffer_is_empty(id));
  HERMES_CHECK_IN_PARAM_RET_NULL(pk);
  HERMES_CHECK_IN_PARAM_RET_NULL(!mid_hermes_ll_buffer_is_empty(pk));
  mid_hermes_ll_user_t* u=calloc(1, sizeof(mid_hermes_ll_user_t));
  assert(u);
  u->id=id;
  u->pk=pk;
  return u;
}

hermes_status_t mid_hermes_ll_user_destroy(mid_hermes_ll_user_t** u){
  HERMES_CHECK_IN_PARAM(u);
  HERMES_CHECK_IN_PARAM(*u);
  mid_hermes_ll_buffer_destroy(&((*u)->id));
  mid_hermes_ll_buffer_destroy(&((*u)->sk));
  mid_hermes_ll_buffer_destroy(&((*u)->pk));
  free(*u);
  *u=NULL;
  return HM_SUCCESS;
}

mid_hermes_ll_user_t* mid_hermes_ll_user_load(mid_hermes_ll_buffer_t* id,
                                              hermes_credential_store_t* cs){
  if(!id || !cs){
    return NULL;
  }
  mid_hermes_ll_user_t* u=NULL;
  mid_hermes_ll_buffer_t* u_pk=mid_hermes_ll_buffer_create(NULL, 0);
  if(!u_pk
     || (HM_SUCCESS!=hermes_credential_store_get_public_key(cs,
                                                            id->data,
                                                            id->length,
                                                            &(u_pk->data),
                                                            &(u_pk->length)))
     || !(u=mid_hermes_ll_user_create(id, u_pk))){
    mid_hermes_ll_buffer_destroy(&u_pk);
    return NULL;
  }
  return u;
}

mid_hermes_ll_user_t* mid_hermes_ll_user_load_c(const uint8_t* id,
                                                const size_t id_length,
                                                hermes_credential_store_t* cs){
  if(!id || !id_length || !cs){
    return NULL;
  }
  mid_hermes_ll_buffer_t* b_id=mid_hermes_ll_buffer_create(id, id_length);
  mid_hermes_ll_user_t* u=NULL;
  if(!b_id
     || !(u=mid_hermes_ll_user_load(b_id, cs))){
    mid_hermes_ll_buffer_destroy(&b_id);
  }
  return u;
}

bool mid_hermes_ll_user_is_equal(const mid_hermes_ll_user_t* u, const mid_hermes_ll_user_t* v){
  assert(u);
  assert(v);
  return mid_hermes_ll_buffer_is_equal(u->id, v->id);
}

mid_hermes_ll_user_t* mid_hermes_ll_user_create_c(const uint8_t* id,
                                                  const size_t id_length,
                                                  const uint8_t* pk,
                                                  const size_t pk_length){
  if(!id || !id_length || !pk || !pk_length){
    return NULL;
  }
  mid_hermes_ll_buffer_t* idbuf=mid_hermes_ll_buffer_create(id, id_length);
  mid_hermes_ll_buffer_t* pkbuf=mid_hermes_ll_buffer_create(pk, pk_length);
  mid_hermes_ll_user_t* u=NULL;
  if(!idbuf
     || !pkbuf
     || !(u=mid_hermes_ll_user_create(idbuf, pkbuf))){
    mid_hermes_ll_buffer_destroy(&idbuf);
    mid_hermes_ll_buffer_destroy(&pkbuf);
    return NULL;
  }
  return u;
}

mid_hermes_ll_user_t* mid_hermes_ll_user_copy(mid_hermes_ll_user_t* u){
  if(!u){
    return NULL;
  }
  mid_hermes_ll_user_t* res=mid_hermes_ll_user_create_c(u->id->data, u->id->length, u->pk->data, u->pk->length);
  if(res && u->sk){
    res->sk=mid_hermes_ll_buffer_create(u->sk->data, u->sk->length);
    if(!(res->sk)){
      mid_hermes_ll_user_destroy(&res);
      return NULL;
    }
  }
  return res;
}
