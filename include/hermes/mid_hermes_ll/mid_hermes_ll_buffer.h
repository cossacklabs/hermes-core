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



#ifndef MID_HERMES_LL_BUFFER_H
#define MID_HERMES_LL_BUFFER_H

#include <hermes/common/errors.h>
#include <hermes/common/mem_cmp.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct mid_hermes_ll_buffer_type {
    uint8_t *data;
    size_t length;
} mid_hermes_ll_buffer_t;

mid_hermes_ll_buffer_t *mid_hermes_ll_buffer_create(const uint8_t *data, const size_t length);

mid_hermes_ll_buffer_t *mid_hermes_ll_buffer_copy(mid_hermes_ll_buffer_t *src, mid_hermes_ll_buffer_t *dst);

bool mid_hermes_ll_buffer_is_empty(mid_hermes_ll_buffer_t *buffer);

bool mid_hermes_ll_buffer_is_equal(mid_hermes_ll_buffer_t *buffer1, mid_hermes_ll_buffer_t *buffer2);

hermes_status_t mid_hermes_ll_buffer_reset(mid_hermes_ll_buffer_t *buffer, const uint8_t *data, const size_t length);

hermes_status_t mid_hermes_ll_buffer_destroy(mid_hermes_ll_buffer_t **buffer);

// same as `mid_hermes_ll_buffer_destroy`, but also set 0 into buffer-> data and length
hermes_status_t mid_hermes_ll_buffer_destroy_secure(mid_hermes_ll_buffer_t** buffer);

#endif //MID_HERMES_LL_BUFFER_H
