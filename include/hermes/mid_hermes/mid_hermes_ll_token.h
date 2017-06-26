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


#ifndef MID_HERMES_LL_TOKEN_H
#define MID_HERMES_LL_TOKEN_H

#include <hermes/common/errors.h>

#include <hermes/mid_hermes/mid_hermes_ll_buffer.h>
#include <hermes/mid_hermes/mid_hermes_ll_user.h>

typedef struct mid_hermes_ll_token_type{
  const mid_hermes_ll_user_t* user;
  const mid_hermes_ll_user_t* owner;
  mid_hermes_ll_buffer_t* token;
} mid_hermes_ll_token_t;

mid_hermes_ll_token_t* mid_hermes_ll_token_create(const mid_hermes_ll_user_t* user,
                                                  const mid_hermes_ll_user_t* owner,
                                                  mid_hermes_ll_buffer_t* token);

mid_hermes_ll_token_t* mid_hermes_ll_token_generate(const mid_hermes_ll_user_t* user_and_owner);

mid_hermes_ll_buffer_t* mid_hermes_ll_token_get_data(mid_hermes_ll_token_t* t);

mid_hermes_ll_token_t* mid_hermes_ll_token_get_token_for_user(mid_hermes_ll_token_t* t,
                                                              const mid_hermes_ll_user_t* for_user);

hermes_status_t mid_hermes_ll_token_destroy(mid_hermes_ll_token_t** t);

#endif //MID_HERMES_LL_TOKEN_H
