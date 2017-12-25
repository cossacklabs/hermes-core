//
// Copyright (c) 2017 Cossack Labs Limited
//
// This file is a part of Hermes-core.
//
// Hermes-core is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hermes-core is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with Hermes-core.  If not, see <http://www.gnu.org/licenses/>.
//
//
#include "transport.h"

extern uint32_t transport_recv(void *transport, uint8_t *buffer, size_t buffer_length);
extern uint32_t transport_send(void *transport, const uint8_t *buffer, const size_t buffer_length);


hm_rpc_transport_t* create_transport(hm_rpc_transport_t** user_data){
  hm_rpc_transport_t* transport = calloc(1, sizeof(hm_rpc_transport_t));
  if(!transport){
    return NULL;
  }
  transport->send = transport_send;
  transport->recv = transport_recv;
  transport->user_data = user_data;
  return transport;
}