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


#ifndef MID_HERMES_MID_HERMES_LL_H
#define MID_HERMES_MID_HERMES_LL_H


#include <hermes/common/errors.h>
#include <stdint.h>
#include <stdlib.h>

#include <hermes/mid_hermes/mid_hermes_ll_block.h>

typedef struct mid_hermes_ll_type mid_hermes_ll_t;
typedef struct mid_hermes_ll_token_type mid_hermes_ll_token_t;


mid_hermes_ll_t* mid_mermes_ll_create(const uint8_t* id, const size_t id_length, const uint8_t* sk, const size_t sk_length);
hermes_status_t mid_hermes_ll_destroy(mid_hermes_ll_t** ctx);

hermes_status_t mid_hermes_ll_create_block(mid_hermes_ll_t* ctx,
                                           const mid_hermes_ll_block_t* block,
                                           uint8_t** encrypted_block,
                                           size_t* encrypted_block_length);

#endif //MID_HERMES_MID_HERMES_LL_H
