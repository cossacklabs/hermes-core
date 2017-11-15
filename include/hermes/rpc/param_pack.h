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


#ifndef HERMES_RPC_PARAM_PACK_H
#define HERMES_RPC_PARAM_PACK_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <hermes/rpc/transport.h>

typedef struct hm_param_pack_type hm_param_pack_t;

typedef uint32_t (*send_rouitine_t)(void* user_data, const uint8_t* buffer_to_send, const size_t buffer_to_send_length);
// if really readed less than buffer_to_receive_length bytes need return error!!!
typedef uint32_t (*recv_rouitine_t)(void* user_data, uint8_t* buffer_to_receive, const size_t buffer_to_receive_length);

hm_param_pack_t* hm_param_pack_create(void);

hm_param_pack_t* hm_param_pack_create_(void* unused, ...);
uint32_t hm_param_pack_extract_(hm_param_pack_t* pack, ...);
uint32_t hm_param_pack_destroy(hm_param_pack_t** pack);

uint32_t hm_param_pack_write(hm_param_pack_t* pack, uint8_t* buffer, size_t *buffer_length);
hm_param_pack_t* hm_param_pack_read(uint8_t* buffer, size_t buffer_length);

uint32_t hm_param_pack_send(const hm_param_pack_t* pack, hm_rpc_transport_t* transport);
hm_param_pack_t* hm_param_pack_receive(hm_rpc_transport_t* transport);


/* constant definitions
 * all constant values is random? but need to be unique
 */
#define HM_PARAM_PACK_MAGIC 0x26048026
#define HM_PARAM_TYPE_INT32 0x26048027      //int32
#define HM_PARAM_TYPE_BUFFER 0x26048028     //buffer with putting ownership to param_pack
                                            //buffer will be freed with hm_param_pack_destroy() calling

#define HM_PARAM_TYPE_BUFFER_C 0x26048029   //buffer without putting ownership to param_pack
                                            //buffer must be destroyed separately with param_pack

#define HM_PARAM_TYPE_BUFFERS_LIST 0x26048030

/* end of constatnt definitions*/

#define HM_PARAM_INT32(p) (uint32_t)HM_PARAM_PACK_MAGIC, (uint32_t)HM_PARAM_TYPE_INT32, p
#define HM_PARAM_BUFFER(p, p_len) (uint32_t)HM_PARAM_PACK_MAGIC, (uint32_t)HM_PARAM_TYPE_BUFFER, p, p_len
#define HM_PARAM_BUFFER_C(p, p_len) (uint32_t)HM_PARAM_PACK_MAGIC, (uint32_t)HM_PARAM_TYPE_BUFFER_C, p, p_len
#define HM_PARAM_BUFFERS_LIST(p) (uint32_t)HM_PARAM_PACK_MAGIC, (uint32_t)HM_PARAM_TYPE_BUFFERS_LIST, p

#define HM_PARAM_PACK(...) hm_param_pack_create_((void*)NULL, __VA_ARGS__, (uint32_t)0)
#define HM_PARAM_EXTRACT(p, ...) hm_param_pack_extract_(p, __VA_ARGS__, (uint32_t)0)

#endif //HERMES_RPC_PARAM_PACK_H
