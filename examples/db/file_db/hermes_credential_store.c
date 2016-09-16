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

#include <hermes/utils.h>
#include <hermes/hermes_credential_store.h>
#include <stdlib.h>
#include <stdio.h>

#define CREDENTIAL_STORE_FOLDER "./db/users/"

struct hermes_credential_store_t_{ 
  int a; //unused
};

hermes_credential_store_t* hermes_credential_store_create(){
  hermes_credential_store_t* store = calloc(sizeof(hermes_credential_store_t),1);
  return store;
}

void hermes_credential_store_destroy(hermes_credential_store_t** store){
  free(*store);
}

int hermes_credential_store_get_pub_key(hermes_credential_store_t* store, const char* id, uint8_t** pub_key, size_t* pub_key_length){
  HERMES_CHECK(store, return -1);
  char key_file_name[1024];
  sprintf(key_file_name, "%s%s", CREDENTIAL_STORE_FOLDER, id);
  FILE* key_file = fopen(key_file_name, "rb");
  HERMES_CHECK(key_file, return -1);

  //determine publik key file size
  fseek(key_file, 0L, SEEK_END);
  *pub_key_length = ftell(key_file);
  fseek(key_file, 0L, SEEK_SET);

  *pub_key = malloc(*pub_key_length);
  HERMES_CHECK(*pub_key, fclose(key_file); return -1);
  HERMES_CHECK((*pub_key_length) == fread(*pub_key, 1, *pub_key_length, key_file), free(*pub_key); fclose(key_file); return -1);
  fclose(key_file);
  return 0;
}


