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
