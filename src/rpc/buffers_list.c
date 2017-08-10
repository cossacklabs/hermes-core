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



#include <hermes/rpc/buffers_list.h>

#include <stdarg.h>
#include <assert.h>
#include <string.h>

hm_buffers_list_t *hm_buffers_list_create() {
    hm_buffers_list_t *list = calloc(1, sizeof(hm_buffers_list_t));
    assert(list);
    return list;
}

hm_buffers_list_t *hm_buffers_list_extract(const uint8_t *data, const size_t length) {
    if (!data || !length) {
        return NULL;
    }
    hm_buffers_list_t *buffers_list = hm_buffers_list_create();
    const uint8_t *end = (data + length);
    while (data != end) {
        if ((data + sizeof(uint32_t)) > end) {
            hm_buffers_list_destroy(&buffers_list);
            return NULL;
        }
        uint32_t len = (*((uint32_t *) data));
        if ((data + sizeof(uint32_t) + len) > end
            || (HM_SUCCESS != hm_buffers_list_add_c(buffers_list, data + sizeof(uint32_t), len))) {
            hm_buffers_list_destroy(&buffers_list);
            return NULL;
        }
        data += sizeof(uint32_t) + len;
    }
    return buffers_list;
}

size_t hm_buffres_list_get_needed_buffer_size(hm_buffers_list_t *buffers_list) {
    if (!buffers_list || !(buffers_list->first)) {
        return 0;
    }
    hm_buffers_list_node_t *current = buffers_list->first;
    size_t result = 0;
    while (current) {
        result += sizeof(uint32_t);
        result += current->length;
    }
    return result;
}

size_t hm_buffers_list_to_buf(hm_buffers_list_t *buffers_list, uint8_t **data) {
    size_t result = 0;
    if (!buffers_list || !data || !(result = hm_buffres_list_get_needed_buffer_size(buffers_list))) {
        return 0;
    }
    *data = malloc(result);
    assert(*data);
    uint8_t *current_position = *data;
    hm_buffers_list_node_t *current = buffers_list->first;
    while (current) {
        *((uint32_t *) current_position) = (uint32_t) (current->length);
        memcpy(current_position + sizeof(uint32_t), current->data, current->length);
        current_position += sizeof(uint32_t) + current->length;
    }
    return result;
}


hm_buffers_list_node_t *hm_buffers_list_get_last(hm_buffers_list_t *buffers_list) {
    if (!buffers_list || !(buffers_list->first)) {
        return NULL;
    }
    hm_buffers_list_node_t *last = buffers_list->first;
    while (last->next) {
        last = last->next;
    }
    return last;
}

hermes_status_t hm_buffers_list_add(hm_buffers_list_t *buffers_list, uint8_t *data, size_t length) {
    if (!buffers_list || !data || !length) {
        return HM_INVALID_PARAMETER;
    }
    hm_buffers_list_node_t *new = calloc(1, sizeof(hm_buffers_list_node_t));
    assert(new);
    new->data = data;
    new->length = length;
    hm_buffers_list_node_t *last = hm_buffers_list_get_last(buffers_list);
    if (!last) {
        buffers_list->first = new;
    } else {
        last->next = new;
    }
    return HM_SUCCESS;
}

hermes_status_t hm_buffers_list_add_c(hm_buffers_list_t *buffers_list, const uint8_t *data, const size_t length) {
    if (!buffers_list || !data || !length) {
        return HM_INVALID_PARAMETER;
    }
    hm_buffers_list_node_t *new = calloc(1, sizeof(hm_buffers_list_node_t));
    assert(new);
    new->data = malloc(length);
    assert(new->data);
    memcpy(new->data, data, length);
    new->length = length;
    hm_buffers_list_node_t *last = hm_buffers_list_get_last(buffers_list);
    if (!last) {
        buffers_list->first = new;
    } else {
        last->next = new;
    }
    return HM_SUCCESS;
}

hermes_status_t hm_buffers_list_destroy(hm_buffers_list_t **buffers_list) {
    if (!buffers_list || !(*buffers_list)) {
        return HM_INVALID_PARAMETER;
    }
    hm_buffers_list_node_t *current = (*buffers_list)->first;
    while (current) {
        free(current->data);
        hm_buffers_list_node_t *tmp = current->next;
        free(current);
        current = tmp;
    }
    free(*buffers_list);
    *buffers_list = NULL;
    return HM_SUCCESS;
}

const uint8_t *hm_buffers_list_iterator_get_data(hm_buffers_list_iterator_t *iterator) {
    if (!iterator || !(iterator->curr)) {
        return NULL;
    }
    return iterator->curr->data;
}

const size_t hm_buffers_list_iterator_get_size(hm_buffers_list_iterator_t *iterator) {
    if (!iterator || !(iterator->curr)) {
        return 0;
    }
    return iterator->curr->length;
}

bool hm_buffers_list_iterator_next(hm_buffers_list_iterator_t *iterator) {
    if (!iterator || !(iterator->curr)) {
        return false;
    }
    iterator->curr = iterator->curr->next;
    return true;
}

hm_buffers_list_iterator_t *hm_buffers_list_iterator_create(hm_buffers_list_t *buffers_list) {
    if (!buffers_list || !(buffers_list->first)) {
        return NULL;
    }
    hm_buffers_list_iterator_t *iterator = calloc(1, sizeof(hm_buffers_list_iterator_t));
    assert(iterator);
    iterator->curr = buffers_list->first;
    iterator->data = hm_buffers_list_iterator_get_data;
    iterator->size = hm_buffers_list_iterator_get_size;
    iterator->next = hm_buffers_list_iterator_next;
    return iterator;
}

hermes_status_t hm_buffers_list_iterator_destroy(hm_buffers_list_iterator_t **iterator) {
    if (!iterator || !(*iterator)) {
        return HM_INVALID_PARAMETER;
    }
    free(*iterator);
    *iterator = NULL;
    return HM_SUCCESS;
}
