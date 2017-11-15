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
#include <netinet/in.h>
#include "utils.h"

// send_data send 4 byte of data_length with htonl and then send data itself
uint32_t send_data(const uint8_t* data, size_t data_length, hm_rpc_transport_t* transport){
    uint32_t output_data_size = htonl((uint32_t)data_length);
    if (transport->send(transport->user_data, (uint8_t*)&output_data_size, sizeof(output_data_size)) == HM_FAIL){
        return HM_FAIL;
    };
    return transport->send(transport->user_data, data, data_length);
}

// read_data_size read 4 byte and return after ntohl or HM_FAIL on transport HM_FAIL return
uint32_t read_data_size(hm_rpc_transport_t* transport){
    uint32_t data_size;
    uint32_t read_count = transport->recv(transport->user_data, (uint8_t*) &data_size, sizeof(data_size));
    if (read_count == HM_FAIL){
        return HM_FAIL;
    }
    data_size = ntohl(data_size);
    return data_size;
}
// read_data read data_length from transport and return HM_FAIL on transport HM_FAIL response or if read count
// of bytes not equal to data_length
uint32_t read_data(uint8_t* data, size_t data_length, hm_rpc_transport_t* transport){
    return transport->recv(transport->user_data, data, data_length);
}