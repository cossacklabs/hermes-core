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


#ifndef BUFFERS_LIST_H
#define BUFFERS_LIST_H

#include <hermes/common/errors.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct hm_buffers_list_node_type hm_buffers_list_node_t;

struct hm_buffers_list_node_type {
    uint8_t *data;
    size_t length;
    hm_buffers_list_node_t *next;
};

typedef struct hm_buffers_list_type {
    hm_buffers_list_node_t *first;
} hm_buffers_list_t;

hm_buffers_list_t *hm_buffers_list_create();

hm_buffers_list_t *hm_buffers_list_extract(const uint8_t *data, const size_t length);

size_t hm_buffers_list_to_buf(hm_buffers_list_t *list, uint8_t **data);

hermes_status_t hm_buffers_list_add(hm_buffers_list_t *list, uint8_t *data, size_t length);

hermes_status_t hm_buffers_list_add_c(hm_buffers_list_t *list, const uint8_t *data, const size_t length);

hermes_status_t hm_buffers_list_destroy(hm_buffers_list_t **list);

typedef struct hm_buffers_list_iterator_type hm_buffers_list_iterator_t;

struct hm_buffers_list_iterator_type {
    hm_buffers_list_node_t *curr;

    const uint8_t *(*data)(hm_buffers_list_iterator_t *iterator);

    const size_t (*size)(hm_buffers_list_iterator_t *iterator);

    bool (*next)(hm_buffers_list_iterator_t *iterator);
};

hm_buffers_list_iterator_t *hm_buffers_list_iterator_create(hm_buffers_list_t *list);

hermes_status_t hm_buffers_list_iterator_destroy(hm_buffers_list_iterator_t **iterator);

#endif //BUFFERS_LIST_H
