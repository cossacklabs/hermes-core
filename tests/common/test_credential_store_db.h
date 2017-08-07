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



#ifndef HERMES_TESTS_TEST_CREDENTIAL_STORE_DB_H
#define HERMES_TESTS_TEST_CREDENTIAL_STORE_DB_H

#include <hermes/credential_store/db.h>

#include <stdint.h>

hm_cs_db_t* hm_test_cs_db_create();
uint32_t hm_test_cs_db_destroy(hm_cs_db_t** db);


#endif //HERMES_TESTS_TEST_CREDENTIAL_STORE_DB_H
