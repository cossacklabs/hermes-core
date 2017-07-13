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


#ifndef CREDENTIAL_STORE_IMPL_H
#define CREDENTIAL_STORE_IMPL_H

#include <hermes/mid_hermes_ll/interfaces/credential_store.h>
#include <hermes/rpc/transport.h>

hermes_credential_store_t *hermes_credential_store_create(hm_rpc_transport_t *transport);


#endif //CREDENTIAL_STORE_IMPL_H
