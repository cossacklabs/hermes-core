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


#include <hermes/hermes_user.h>
#include <hermes/utils.h>


struct hermes_user_t_{
  char* id;
  uint8_t* public_key;
  size_t public_key_length;
  hermes_crypter_t* crypter;
};

hermes_user_t* hermes_user_create(const char* user_id, const uint8_t* private_key, const size_t private_key_length){
  HERMES_CHECK(user_id && private_key, return NULL);
  hermes_user_t* user = calloc(sizeof(hermes_user_t), 1);
  HERMES_CHECK(user, return NULL);
  user->crypter = hermes_crypter_create(private_key, private_key_length);
  HERMES_CHECK(user->crypter, hermes_user_destroy(&user); return NULL);
  return user;
}

void hermes_user_destroy(hermes_user_t** user){
  HERMES_CHECK(*user, return);
  if((*user)->crypter)
    hermes_crypter_destroy(&((*user)->crypter));
  free((*user)->public_key);
  free(*user);
  *user=NULL;
}

hermes_crypter_t* hermes_user_get_crypter(hermes_user_t* user){
  HERMES_CHECK(user && user->crypter, return NULL);
  return user->crypter;
}

