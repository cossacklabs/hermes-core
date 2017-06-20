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


struct mid_hermes_ll_block_type{
  const mid_hermes_ll_user_t* user;
  mid_hermes_ll_buffer_t* id;
  mid_hermes_ll_buffer_t* block;
  mid_hermes_ll_buffer_t* data;
  mid_hermes_ll_buffer_t* meta;
  mid_hermes_ll_buffer_t* old_mac;
  mid_hermes_ll_buffer_t* mac;
  mid_hermes_ll_rights_list_t* rights;
};

typedef struct mid_hermes_ll_block_type mid_hermes_ll_block_t;

mid_hermes_ll_block_t* mid_hermes_ll_block_create_new(mid_hermes_ll_buffer_t* block,
                                                      mid_hermes_ll_buffer_t* meta,
                                                      const mid_hermes_ll_user_t* user);

mid_hermes_ll_block_t* mid_hermes_ll_block_create_new_with_id(mid_hermes_ll_buffer_t* id,
                                                              mid_hermes_ll_buffer_t* block,
                                                              mid_hermes_ll_buffer_t* meta,
                                                              const mid_hermes_ll_user_t* user);

mid_hermes_ll_block_t* mid_hermes_ll_block_restore(mid_hermes_ll_buffer_t* id,
                                                   mid_hermes_ll_buffer_t* block,
                                                   mid_hermes_ll_buffer_t* meta,
                                                   const mid_hermes_ll_user_t* user,
                                                   mid_hermes_ll_rights_list_t* rights);

mid_hermes_ll_block_t* mid_hermes_ll_block_set_id(mid_hermes_ll_block_t* b, mid_hermes_ll_buffer_t* id);

mid_hermes_ll_buffer_t* mid_hermes_ll_block_get_data(mid_hermes_ll_block_t* b);
mid_hermes_ll_block_t* mid_hermes_ll_block_set_data(mid_hermes_ll_block_t* b, mid_hermes_ll_buffer_t* data, mid_hermes_ll_buffer_t* meta);

mid_hermes_ll_block_t* mid_hermes_ll_block_grant_read_access(mid_hermes_ll_block_t* b, const mid_hermes_ll_user_t* user);
mid_hermes_ll_block_t* mid_hermes_ll_block_grant_update_access(mid_hermes_ll_block_t* b, const mid_hermes_ll_user_t* user);
mid_hermes_ll_block_t* mid_hermes_ll_block_deny_read_access(mid_hermes_ll_block_t* b, const mid_hermes_ll_user_t* user);
mid_hermes_ll_block_t* mid_hermes_ll_block_deny_update_access(mid_hermes_ll_block_t* b, const mid_hermes_ll_user_t* user);

hermes_status_t mid_hermes_ll_block_destroy(mid_hermes_ll_block_t** b);

#endif //MID_HERMES_LL_BLOCK_H
