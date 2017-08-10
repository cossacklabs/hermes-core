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



#ifndef HERMES_CREDENTIAL_STORE_DB_H
#define HERMES_CREDENTIAL_STORE_DB_H


#include <stdint.h>
#include <stdlib.h>

typedef uint32_t(*hm_cs_db_get_pub_by_id_t)(void *db, const uint8_t *id, const size_t id_length, uint8_t **key, size_t *key_length);

typedef struct hm_cs_db_type {
    void *user_data;
    hm_cs_db_get_pub_by_id_t get_pub;
} hm_cs_db_t;


#endif //HERMES_CREDENTIAL_STORE_DB_H
