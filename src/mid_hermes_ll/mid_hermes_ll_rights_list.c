/*
* Copyright (c) 2017 Cossack Labs Limited
*
* This file is a part of Hermes-core.
*
* Hermes-core is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Hermes-core is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Hermes-core.  If not, see <http://www.gnu.org/licenses/>.
*
*/



#include <hermes/mid_hermes_ll/mid_hermes_ll_rights_list.h>

#include <assert.h>
#include <string.h>

mid_hermes_ll_rights_list_node_t *mid_hermes_ll_rights_list_node_create(
        mid_hermes_ll_user_t *user, mid_hermes_ll_token_t *read_token, mid_hermes_ll_token_t *write_token) {
    HERMES_CHECK_IN_PARAM_RET_NULL(user);
    HERMES_CHECK_IN_PARAM_RET_NULL(read_token);
    mid_hermes_ll_rights_list_node_t *n = calloc(1, sizeof(mid_hermes_ll_rights_list_node_t));
    assert(n);
    n->user = user;
    n->rtoken = read_token;
    n->wtoken = write_token;
    return n;
}

hermes_status_t mid_hermes_ll_rights_list_node_destroy(mid_hermes_ll_rights_list_node_t **node) {
    HERMES_CHECK_IN_PARAM(node);
    HERMES_CHECK_IN_PARAM(*node);
    mid_hermes_ll_user_destroy(&((*node)->user));
    mid_hermes_ll_token_destroy(&((*node)->rtoken));
    mid_hermes_ll_token_destroy(&((*node)->wtoken));
    free(*node);
    *node = NULL;
    return HM_SUCCESS;
}

mid_hermes_ll_rights_list_t *mid_hermes_ll_rights_list_create() {
    mid_hermes_ll_rights_list_t *list = calloc(1, sizeof(mid_hermes_ll_rights_list_t));
    assert(list);
    return list;
}

hermes_status_t mid_hermes_ll_rights_list_rpush(
        mid_hermes_ll_rights_list_t *rights_list, mid_hermes_ll_rights_list_node_t *rights_node) {
    HERMES_CHECK_IN_PARAM(rights_list);
    HERMES_CHECK_IN_PARAM(rights_node);
    if (rights_list->len) {
        rights_node->prev = rights_list->tail;
        rights_node->next = NULL;
        rights_list->tail->next = rights_node;
        rights_list->tail = rights_node;
    } else {
        rights_list->head = rights_list->tail = rights_node;
        rights_node->prev = rights_node->next = NULL;
    }
    ++(rights_list->len);
    return HM_SUCCESS;
}

hermes_status_t mid_hermes_ll_rights_list_rpush_val(
        mid_hermes_ll_rights_list_t *rights_list, mid_hermes_ll_user_t *user,
        mid_hermes_ll_token_t *read_token, mid_hermes_ll_token_t *write_token) {
    return mid_hermes_ll_rights_list_rpush(
            rights_list, mid_hermes_ll_rights_list_node_create(user, read_token, write_token));
}


mid_hermes_ll_rights_list_node_t *mid_hermes_ll_rights_list_rpop(mid_hermes_ll_rights_list_t *rights_list) {
    HERMES_CHECK_IN_PARAM_RET_NULL(rights_list);
    HERMES_CHECK_IN_PARAM_RET_NULL(rights_list->len);
    mid_hermes_ll_rights_list_node_t *node = rights_list->tail;
    if (--(rights_list->len)) {
        rights_list->tail = node->prev;
        rights_list->tail->next = NULL;
    } else {
        rights_list->tail = node->prev = NULL;
    }
    node->next = node->prev = NULL;
    return node;
}

mid_hermes_ll_rights_list_node_t *mid_hermes_ll_rights_list_find(
        mid_hermes_ll_rights_list_t *rights_list, mid_hermes_ll_buffer_t *user_id) {
    HERMES_CHECK_IN_PARAM_RET_NULL(rights_list);
    HERMES_CHECK_IN_PARAM_RET_NULL(user_id);
    HERMES_CHECK_IN_PARAM_RET_NULL(user_id->length);
    mid_hermes_ll_rights_list_iterator_t *iterator = mid_hermes_ll_rights_list_iterator_create(rights_list);
    mid_hermes_ll_rights_list_node_t *node;
    while ((node = mid_hermes_ll_rights_list_iterator_next(iterator))) {
        if (node->user->id->length == user_id->length &&
            0 == memcmp(node->user->id->data, user_id->data, user_id->length)) {
            mid_hermes_ll_rights_list_iterator_destroy(&iterator);
            return node;
        }
    }
    mid_hermes_ll_rights_list_iterator_destroy(&iterator);
    return NULL;
}

hermes_status_t mid_hermes_ll_rights_list_remove(
        mid_hermes_ll_rights_list_t *rights_list, mid_hermes_ll_rights_list_node_t *rights_node) {
    HERMES_CHECK_IN_PARAM(rights_list);
    HERMES_CHECK_IN_PARAM(rights_node);
    rights_node->prev ? (rights_node->prev->next = rights_node->next) : (rights_list->head = rights_node->next);
    rights_node->next ? (rights_node->next->prev = rights_node->prev) : (rights_list->tail = rights_node->prev);
    --(rights_list->len);
    mid_hermes_ll_rights_list_node_destroy(&rights_node);
    return HM_SUCCESS;
}

hermes_status_t mid_hermes_ll_rights_list_destroy(mid_hermes_ll_rights_list_t **rights_list) {
    HERMES_CHECK_IN_PARAM(rights_list);
    HERMES_CHECK_IN_PARAM(*rights_list);
    size_t len = (*rights_list)->len;
    mid_hermes_ll_rights_list_node_t *next;
    mid_hermes_ll_rights_list_node_t *current = (*rights_list)->head;

    while (len--) {
        next = current->next;
        mid_hermes_ll_rights_list_node_destroy(&current);
        current = next;
    }

    free(*rights_list);
    *rights_list = NULL;
    return HM_SUCCESS;
}

mid_hermes_ll_rights_list_iterator_t *mid_hermes_ll_rights_list_iterator_create(
        mid_hermes_ll_rights_list_t *rights_list) {
    HERMES_CHECK_IN_PARAM_RET_NULL(rights_list);
    mid_hermes_ll_rights_list_iterator_t *iterator = calloc(1, sizeof(mid_hermes_ll_rights_list_iterator_t));
    assert(iterator);
    iterator->next = rights_list->head;
    return iterator;
}

mid_hermes_ll_rights_list_node_t *mid_hermes_ll_rights_list_iterator_next(
        mid_hermes_ll_rights_list_iterator_t *iterator) {
    HERMES_CHECK_IN_PARAM_RET_NULL(iterator);
    mid_hermes_ll_rights_list_node_t *curr = iterator->next;
    if (curr) {
        iterator->next = curr->next;
    }
    return curr;
}

hermes_status_t mid_hermes_ll_rights_list_iterator_destroy(mid_hermes_ll_rights_list_iterator_t **iterator) {
    HERMES_CHECK_IN_PARAM(iterator);
    HERMES_CHECK_IN_PARAM(*iterator);
    free(*iterator);
    *iterator = NULL;
    return HM_SUCCESS;
}
