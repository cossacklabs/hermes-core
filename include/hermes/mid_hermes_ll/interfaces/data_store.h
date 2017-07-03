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


#ifndef HERMES_DATA_STORE_H
#define HERMES_DATA_STORE_H

#include <hermes/common/errors.h>

#include <stdlib.h>
#include <stdint.h>

typedef struct hermes_data_store_type hermes_data_store_t;

hermes_status_t hermes_data_store_set_block(hermes_data_store_t* ds,
                             uint8_t** id,
                             size_t* id_length,
                             const uint8_t* data,
                             const size_t data_length,
                             const uint8_t* meta,
                             const size_t meta_length,
                             const uint8_t* mac,
                             const size_t mac_length,
                             const uint8_t* old_mac,
                             const size_t old_mac_length);

hermes_status_t hermes_data_store_get_block(hermes_data_store_t* ds,
                             const uint8_t* id,
                             const size_t id_length,
                             uint8_t** data,
                             size_t* data_length,
                             uint8_t** meta,
                             size_t* meta_length);

hermes_status_t hermes_data_store_rem_block(hermes_data_store_t* ds,
                             const uint8_t* id,
                             const size_t id_length,
                             const uint8_t* old_mac,
                             const size_t old_mac_length);

hermes_status_t hermes_data_store_destroy(hermes_data_store_t** ds);
#endif //HERMES_DATA_STORE_H
