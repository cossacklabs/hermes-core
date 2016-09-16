/*
 * Copyright (c) 2016 Cossack Labs Limited
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

#include <hermes/hermes_document_blocks_list.h>

typedef struct hermes_document_blocks_list_node_t_ hermes_document_blocks_list_node_t;

struct hermes_document_blocks_list_node_t_{
  hermes_document_block_t* data;
  hermes_document_blocks_list_node_t* next_node;
  hermes_document_blocks_list_node_t* prev_node;
}; 

struct hermes_document_blocks_list_t_{
  hermes_document_blocks_list_node_t* first;
  hermes_document_blocks_list_node_t* curr;
};

hermes_document_blocks_list_t* hermes_documant_blocks_list_create(){
  hermes_document_blocks_list_t* list = calloc(sizeof(hermes_document_blocks_list_node_t),1);
  HERMES_CHECK(list, return NULL);
  return list;
}

void hermes_document_blocks_list_destroy(hermes_document_blocks_list_t** list){
  HERMES_CHECK(*list, return);
  (*list)->curr = (*list)->first;
  while((*list)->curr){
    if(!((*list)->curr->deleted)){
      hermes_document_block_destroy((*list)->curr->data);
    }
    hermes_document_blocks_list_node_t* tmp=(*list)->curr;
    (*list)->curr = (*list)->curr->next;
    free(tmp);
  }
  free(*list);
}

int hermes_document_blocks_list_set_to_first(hermes_document_blocks_list_t* list){
  HERMES_CHECK(list, return -2);
  list->curr = list->first;
  return 0;
}

int hermes_document_blocks_list_next(hermes_document_blocks_list_t* list){
  HERMES_CHECK(list, return -2);
  HERMES_CHECK(list->curr, return -1);
  list->curr = list->curr->next;
  return 0;  
}
hermes_document_block_t* hermes_document_blocks_list_get_curr(hermes_document_blocks_list_t* list){
  HERMES_CHECK(list && list->curr && !(list->curr->deleted), return NULL);
  return list->curr->data;
}

hermes_document_block_t* hermes_document_blocks_list_remove_curr(hermes_document_blocks_list_t* list){
  HERMES_CHECK(list && list->curr, return NULL);
  hermes_document_block_t* tmp = list->curr;
  if(tmp->prev)
    tmp->prev->next = tmp->next;
  if(tmp->next)
    tmp->next->prev = tmp->prev;
  hermes_document_block_t* tmp_data = tmp->data;
  free(tmp);
  return tmp_data;
}

int hermes_document_blocks_list_push(hermes_document_blocks_list_t *list, hermes_document_block_t* new_block){
  HERMES_CHECK(list, return -2);
  hermes_document_blocks_list_node_t* new_node=calloc(sizeof(hermes_document_blocks_list_node_t),1);
  HERMES_CHECK(new_node, return -1);
  new_node->data = new_block;
  hermes_document_blocks_list_node_t* tmp = list->first;
  if(!(list->first)){
    list->first = new_node;
    return 0;
  }
  while(tmp->next)
    tmp = tmp->next;
  tmp->next = new_node;
  new_node->prev = tmp;
  return 0; 
}

hermes_document_block_t* hermes_document_blocks_list_pop(hermes_document_blocks_list_t *list){
  HERMES_CHECK(list && list->first, return NULL);
  hermes_document_blocks_list_node_t* tmp = list->first;
  
}

