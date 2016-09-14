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

#ifndef HERMES_HERMES_USER_H_
#define HERMES_HERMES_USER_H_

#include <hermes/crypt.h>

typedef struct hermes_user_t_ hermes_user_t;

hermes_user_t* hermes_user_create(const char* user_id, const uint8_t* private_key, const size_t private_key_length);

void hermes_user_destroy(hermes_user_t** user);

hermes_crypter_t* hermes_user_get_crypter(hermes_user_t* user);

#endif /* HERMES_HERMES_USER_H_ */
  
