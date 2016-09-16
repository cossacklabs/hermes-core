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

#ifndef HERMES_HERMES_STORAGES_H_
#define HERMES_HERMES_STORAGES_H_

#include <hermes/hermes_credential_store.h>
#include <hermes/hermes_record_set_store.h>
#include <hermes/hermes_access_key_store.h>

typedef struct hermes_storages_t_ hermes_storages_t;

extern hermes_storages_t* hermes_storages;

hermes_storages_t* hermes_storages_create();

void hermes_storages_destroy(hermes_storages_t** storages);

hermes_credential_store_t* hermes_storages_get_credential_store(hermes_storages_t* storages);
hermes_record_set_store_t* hermes_storages_get_record_set_store(hermes_storages_t* storages);
hermes_access_key_store_t* hermes_storages_get_access_key_store(hermes_storages_t* storages);

#endif /* HERMES_HERMES_STORAGES_H_ */
