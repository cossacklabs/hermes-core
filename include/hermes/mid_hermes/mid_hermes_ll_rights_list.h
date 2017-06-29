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


#ifndef MID_HERMES_LL_RIGHTS_LIST_H
#define MID_HERMES_LL_RIGHTS_LIST_H

#include <hermes/mid_hermes/mid_hermes_ll_user.h>
#include <hermes/mid_hermes/mid_hermes_ll_token.h>
#include <hermes/common/errors.h>


typedef struct mid_hermes_ll_rights_list_node_type{
  struct mid_hermes_ll_rights_list_node_type *prev;
  struct mid_hermes_ll_rights_list_node_type *next;
  mid_hermes_ll_user_t* user;
  mid_hermes_ll_token_t* rtoken;
  mid_hermes_ll_token_t* wtoken;
}mid_hermes_ll_rights_list_node_t;

mid_hermes_ll_rights_list_node_t* mid_hermes_ll_rights_list_node_create(mid_hermes_ll_user_t* user, mid_hermes_ll_token_t* rtoken, mid_hermes_ll_token_t* wtoken);
hermes_status_t mid_hermes_ll_rights_list_node_destroy(mid_hermes_ll_rights_list_node_t** n);

typedef struct mid_hermes_ll_rights_list_type{
  mid_hermes_ll_rights_list_node_t* head;
  mid_hermes_ll_rights_list_node_t* tail;
  size_t len;
} mid_hermes_ll_rights_list_t;

mid_hermes_ll_rights_list_t* mid_hermes_ll_rights_list_create();
hermes_status_t mid_hermes_ll_rights_list_rpush(mid_hermes_ll_rights_list_t* l, mid_hermes_ll_rights_list_node_t* n);
hermes_status_t mid_hermes_ll_rights_list_rpush_val(mid_hermes_ll_rights_list_t* l, mid_hermes_ll_user_t* user, mid_hermes_ll_token_t* rtoken, mid_hermes_ll_token_t* wtoken);
mid_hermes_ll_rights_list_node_t* mid_hermes_ll_rights_list_rpop(mid_hermes_ll_rights_list_t* l);
mid_hermes_ll_rights_list_node_t* mid_hermes_ll_rights_list_find(mid_hermes_ll_rights_list_t* l, mid_hermes_ll_buffer_t* user_id);
hermes_status_t mid_hermes_ll_rights_list_remove(mid_hermes_ll_rights_list_t* l, mid_hermes_ll_rights_list_node_t* n);
hermes_status_t mid_hermes_ll_rights_list_destroy(mid_hermes_ll_rights_list_t** l);

typedef struct {
  mid_hermes_ll_rights_list_node_t *next;
} mid_hermes_ll_rights_list_iterator_t;


mid_hermes_ll_rights_list_iterator_t* mid_hermes_ll_rights_list_iterator_create(mid_hermes_ll_rights_list_t *l);

mid_hermes_ll_rights_list_node_t* mid_hermes_ll_rights_list_iterator_next(mid_hermes_ll_rights_list_iterator_t *i);

hermes_status_t mid_hermes_ll_rights_list_iterator_destroy(mid_hermes_ll_rights_list_iterator_t **self);
#endif //MID_HERMES_LL_RIGHTS_LIST_H
