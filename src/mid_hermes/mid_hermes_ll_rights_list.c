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

#include <hermes/mid_hermes/mid_hermes_ll_rights_list.h>

#include <assert.h>
#include <string.h>

mid_hermes_ll_rights_list_node_t* mid_hermes_ll_rights_list_node_create(const mid_hermes_ll_user_t* user, mid_hermes_ll_token_t* rtoken, mid_hermes_ll_token_t* wtoken){
  HERMES_CHECK_IN_PARAM_RET_NULL(user);
  HERMES_CHECK_IN_PARAM_RET_NULL(rtoken);
  mid_hermes_ll_rights_list_node_t* n=calloc(1, sizeof(mid_hermes_ll_rights_list_node_t));
  assert(n);
  n->user=user;
  n->rtoken=rtoken;
  n->wtoken=wtoken;
  return n;
}

hermes_status_t mid_hermes_ll_rights_list_node_destroy(mid_hermes_ll_rights_list_node_t** n){
  HERMES_CHECK_IN_PARAM(n);
  HERMES_CHECK_IN_PARAM(*n);
  mid_hermes_ll_token_destroy(&((*n)->rtoken));
  mid_hermes_ll_token_destroy(&((*n)->wtoken));
  free(*n);
  *n=NULL;
  return HM_SUCCESS;
}

mid_hermes_ll_rights_list_t* mid_hermes_ll_rights_list_create(){
  mid_hermes_ll_rights_list_t* l=calloc(1, sizeof(mid_hermes_ll_rights_list_t));
  assert(l);
  return l;
}

hermes_status_t mid_hermes_ll_rights_list_rpush(mid_hermes_ll_rights_list_t* l, mid_hermes_ll_rights_list_node_t* n){
  HERMES_CHECK_IN_PARAM(l);
  HERMES_CHECK_IN_PARAM(n);
  if(l->len){
    n->prev=l->tail;
    n->next=NULL;
    l->tail->next=n;
    l->tail=n;
  }else{
    l->head=l->tail=n;
    n->prev=n->next=NULL;
  }
  ++(l->len);
  return HM_SUCCESS;
}

hermes_status_t mid_hermes_ll_rights_list_rpush_val(mid_hermes_ll_rights_list_t* l,const mid_hermes_ll_user_t* user, mid_hermes_ll_token_t* rtoken, mid_hermes_ll_token_t* wtoken){
  return mid_hermes_ll_rights_list_rpush(l,  mid_hermes_ll_rights_list_node_create(user, rtoken, wtoken));
}


mid_hermes_ll_rights_list_node_t* mid_hermes_ll_rights_list_rpop(mid_hermes_ll_rights_list_t* l){
  HERMES_CHECK_IN_PARAM_RET_NULL(l);
  HERMES_CHECK_IN_PARAM_RET_NULL(l->len);
  mid_hermes_ll_rights_list_node_t* n=l->tail;
  if(--(l->len)){
    l->tail=n->prev;
    l->tail->next=NULL;
  } else {
    l->tail=n->prev=NULL;
  }
  n->next = n->prev = NULL;
  return n;
}

mid_hermes_ll_rights_list_node_t* mid_hermes_ll_rights_list_find(mid_hermes_ll_rights_list_t* l, mid_hermes_ll_buffer_t* user_id){
  HERMES_CHECK_IN_PARAM_RET_NULL(l);
  HERMES_CHECK_IN_PARAM_RET_NULL(user_id);
  HERMES_CHECK_IN_PARAM_RET_NULL(user_id->length);
  mid_hermes_ll_rights_list_iterator_t *i = mid_hermes_ll_rights_list_iterator_create(l);
  mid_hermes_ll_rights_list_node_t *node;
  while ((node = mid_hermes_ll_rights_list_iterator_next(i))) {
    if(node->user->id->length==user_id->length && 0==memcmp(node->user->id->data, user_id->data, user_id->length)){
      mid_hermes_ll_rights_list_iterator_destroy(&i);
      return node;
    }
  }
  mid_hermes_ll_rights_list_iterator_destroy(&i);
  return NULL;
}

hermes_status_t mid_hermes_ll_rights_list_remove(mid_hermes_ll_rights_list_t* l, mid_hermes_ll_rights_list_node_t* n){
  HERMES_CHECK_IN_PARAM(l);
  HERMES_CHECK_IN_PARAM(n);
  n->prev?(n->prev->next = n->next):(l->head = n->next);
  n->next?(n->next->prev = n->prev):(l->tail = n->prev);
  --(l->len);
  mid_hermes_ll_rights_list_node_destroy(&n);
  return HM_SUCCESS;
}

hermes_status_t mid_hermes_ll_rights_list_destroy(mid_hermes_ll_rights_list_t** l){
  HERMES_CHECK_IN_PARAM(l);
  HERMES_CHECK_IN_PARAM(*l);
  size_t len = (*l)->len;
  mid_hermes_ll_rights_list_node_t* next;
  mid_hermes_ll_rights_list_node_t* curr = (*l)->head;

  while (len--) {
    next = curr->next;
    mid_hermes_ll_rights_list_node_destroy(&curr);
    curr = next;
  }

  free(*l);
  *l=NULL;
  return HM_SUCCESS;
}

mid_hermes_ll_rights_list_iterator_t* mid_hermes_ll_rights_list_iterator_create(mid_hermes_ll_rights_list_t *l){
  HERMES_CHECK_IN_PARAM_RET_NULL(l);
  mid_hermes_ll_rights_list_iterator_t* i=calloc(1, sizeof(mid_hermes_ll_rights_list_iterator_t));
  assert(i);
  i->next=l->head;
  return i;
}

mid_hermes_ll_rights_list_node_t* mid_hermes_ll_rights_list_iterator_next(mid_hermes_ll_rights_list_iterator_t *i){
  HERMES_CHECK_IN_PARAM_RET_NULL(i);
  mid_hermes_ll_rights_list_node_t* curr = i->next;
  if(curr){
    i->next=curr->next;
  }
  return curr;
}

hermes_status_t mid_hermes_ll_rights_list_iterator_destroy(mid_hermes_ll_rights_list_iterator_t **i){
  HERMES_CHECK_IN_PARAM(i);
  HERMES_CHECK_IN_PARAM(*i);
  free(*i);
  *i=NULL;
  return HM_SUCCESS;
}
