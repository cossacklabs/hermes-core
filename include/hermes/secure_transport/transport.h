//
// Created by lagovas on 10/11/17.
//

#ifndef HERMES_CORE_TRANSPORT_H
#define HERMES_CORE_TRANSPORT_H

#include "hermes/rpc/transport.h"
uint32_t destroy_secure_transport(hm_rpc_transport_t** transport_);
hm_rpc_transport_t* create_secure_transport(
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *private_key, const size_t private_key_length,
        const uint8_t *public_key, const size_t public_key_length,
        const uint8_t *public_key_id, const size_t public_key_id_length,
        hm_rpc_transport_t* user_transport);
#endif //HERMES_CORE_TRANSPORT_H
