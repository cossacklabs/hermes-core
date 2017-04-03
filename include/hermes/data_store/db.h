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


#ifndef HERMES_DATA_STORE_DB_H
#define HERMES_DATA_STORE_DB_H


#include <stdint.h>
#include <stdlib.h>

typedef struct hm_ds_db_type hm_ds_db_t;

uint32_t hm_ds_db_get_pub_by_id(hm_ds_db_t* db, const uint8_t* id, const size_t id_length, uint8_t** key, size_t* key_length);

#endif //HERMES_DATA_STORE_DB_H
