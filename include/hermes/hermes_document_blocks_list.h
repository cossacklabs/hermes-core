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

#ifndef HERMES_HERMES_DOCUMENT_BLOCKS_LIST_H_
#define HERMES_HERMES_DOCUMENT_BLOCKS_LIST_H_

#include <hermes/hermes_document_block.h>

typedef struct hermes_document_blocks_list_t_ hermes_document_blocks_list_t;

hermes_document_blocks_list_t* hermes_documant_blocks_list_create();
void hermes_document_blocks_list_destroy(hermes_document_blocks_list_t** list);

int hermes_document_blocks_list_first(hermes_document_blocks_list_t* list);
int hermes_document_blocks_list_next(hermes_document_blocks_list_t* list);
hermes_document_block_t* hermes_document_blocks_list_get_curr(hermes_document_blocks_list_t* list);
hermes_document_block_t* hermes_document_blocks_list_remove_curr(hermes_document_blocks_list_t* list);

int hermes_document_blocks_list_push(hermes_document_blocks_list_t *list, hermes_document_block_t* new_block);
hermes_document_block_t* hermes_document_blocks_list_pop(hermes_document_blocks_list_t *list);

#endif /* HERMES_HERMES_DOCUMENT_BLOCKS_LIST_H_ */
