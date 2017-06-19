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
#include <hermes/mid_hermes/mid_hermes_ll_token.h>
#include "utils.h"

#include <themis/themis.h>

#include <assert.h>

mid_hermes_ll_token_t* mid_hermes_ll_token_create(const mid_hermes_ll_buffer_t* block_id,
                                                  const mid_hermes_ll_user_t* user,
                                                  const mid_hermes_ll_user_t* owner,
                                                  mid_hermes_ll_buffer_t* token){
  HERMES_CHECK_IN_PARAM_RET_NULL(block_id);
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  HERMES_CHECK_IN_PARAM_RET_NULL(token);
  mid_hermes_ll_token_t* t = calloc(1, sizeof(mid_hermes_ll_token_t));
  assert(t);
  t->block_id=block_id;
  t->user=user;
  t->owner=owner;
  t->token=token;
  return t;
}

mid_hermes_ll_token_t* mid_hermes_ll_token_generate(const mid_hermes_ll_buffer_t* id,
                                                    const mid_hermes_ll_user_t* user_and_owner){
  HERMES_CHECK_IN_PARAM_RET_NULL(id);
  HERMES_CHECK_IN_PARAM_RET_NULL(user_and_owner);
  uint8_t token[HM_TOKEN_LEN];
  if(SOTER_SUCCESS!=soter_rand(token, HM_TOKEN_LEN)){
    return NULL;
  }
  uint32_t res;
  mid_hermes_ll_buffer_t* enc_token=mid_hermes_ll_buffer_create(NULL, 0);
  res=hm_asym_encrypt(user_and_owner->sk->data, user_and_owner->sk->length, user_and_owner->pk->data, user_and_owner->pk->length, token, sizeof(token), &(enc_token->data), &(enc_token->length));
  if(HM_SUCCESS!=res){
    return NULL;
  }
  return mid_hermes_ll_token_create(id, user_and_owner, user_and_owner, enc_token);
}

mid_hermes_ll_buffer_t* mid_hermes_ll_token_get_token(mid_hermes_ll_token_t* t){
  HERMES_CHECK_IN_PARAM_RET_NULL(t);
  mid_hermes_ll_buffer_t* b = mid_hermes_ll_buffer_create(NULL,0);
  if(HM_SUCCESS!=hm_asym_decrypt(t->user->sk->data, t->user->sk->length, t->owner->pk->data, t->owner->pk->length, t->token->data, t->token->length, &(b->data),&(b->length))){
    mid_hermes_ll_buffer_destroy(&b);
    return NULL;
  }
  return b;
}

mid_hermes_ll_token_t* mid_hermes_ll_token_get_token_for_user(mid_hermes_ll_token_t* t,
                                                              const mid_hermes_ll_user_t* for_user){
  HERMES_CHECK_IN_PARAM_RET_NULL(t);
  HERMES_CHECK_IN_PARAM_RET_NULL(for_user);
  mid_hermes_ll_buffer_t* b=mid_hermes_ll_token_get_token(t);
  if(!b){
    return NULL;
  }
  mid_hermes_ll_buffer_t* eb=mid_hermes_ll_buffer_create(NULL, 0);
  hermes_status_t res=hm_asym_encrypt(t->user->sk->data, t->user->sk->length, for_user->pk->data, for_user->pk->length, b->data, b->length, &(eb->data), &(eb->length));
  if(HM_SUCCESS!=res){
    mid_hermes_ll_buffer_destroy(&b);
    mid_hermes_ll_buffer_destroy(&eb);
    return NULL;
  }
  mid_hermes_ll_buffer_destroy(&b);
  return mid_hermes_ll_token_create(t->block_id, for_user, t->user, eb);
}

hermes_status_t mid_hermes_ll_token_destroy(mid_hermes_ll_token_t** t){
  HERMES_CHECK_IN_PARAM(t);
  HERMES_CHECK_IN_PARAM(*t);
  free((*t)->token);
  free(*t);
  *t=NULL;
  return HM_SUCCESS;
}
