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


#include <hermes/hermes_storages.h>
#include <hermes/utils.h>

struct hermes_storages_t_{
  hermes_credential_store_t* credential_store;
  hermes_record_set_store_t* record_set_store;
  hermes_access_key_store_t* access_key_store;
};

hermes_storages_t* hermes_storages_create(){
  hermes_storages_t* storages = calloc(sizeof(hermes_storages_t),1);
  HERMES_CHECK(storages, return NULL);
  storages->credential_store = hermes_credential_store_create();
  HERMES_CHECK(storages->credential_store, hermes_storages_destroy(&storages); return NULL);
  storages->record_set_store = hermes_record_set_store_create();
  HERMES_CHECK(storages->record_set_store, hermes_storages_destroy(&storages); return NULL);
  storages->access_key_store = hermes_access_key_store_create();
  HERMES_CHECK(storages->access_key_store, hermes_storages_destroy(&storages); return NULL);
  return storages;
}

void hermes_storages_destroy(hermes_storages_t** storages){
  HERMES_CHECK(*storages, return);
  hermes_credential_store_destroy(&((*storages)->credential_store));
  hermes_record_set_store_destroy(&((*storages)->record_set_store));
  hermes_access_key_store_destroy(&((*storages)->access_key_store));
}

hermes_credential_store_t* hermes_storages_get_credential_store(hermes_storages_t* storages){
  HERMES_CHECK(storages, return);
  return storages->credential_store;
}
hermes_record_set_store_t* hermes_storages_get_record_set_store(hermes_storages_t* storages){
  HERMES_CHECK(storages, return);
  return storages->record_set_store;
}

hermes_access_key_store_t* hermes_storages_get_access_key_store(hermes_storages_t* storages){
  HERMES_CHECK(storages, return);
  return storages->access_key_store;
}

