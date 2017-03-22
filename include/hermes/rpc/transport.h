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


#ifndef HERMES_RPC_TRANSPORT_H
#define HERMES_RPC_TRANSPORT_H

typedef struct hm_rpc_transport_type hm_rpc_transport_t;

uint32_t hm_rpc_transport_send(const uint8_t* buffer, const size_t buffer_length);
uint32_t hm_rpc_transport_recv(uint8_t* buffer, size_t buffer_length);

#endif //HERMES_RPC_TRANSPORT_H
