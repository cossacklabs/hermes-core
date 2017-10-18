//
// Created by lagovas on 10/11/17.
//

#ifndef HERMES_CORE_TRANSPORT_H
#define HERMES_CORE_TRANSPORT_H

#include "hermes/rpc/transport.h"

typedef struct secure_transport_type secure_transport_t;

uint32_t destroy_secure_transport(secure_transport_t** transport_);
uint32_t destroy_rpc_secure_transport(hm_rpc_transport_t** transport_);
hm_rpc_transport_t* create_secure_transport(
        const uint8_t *user_id, size_t user_id_length,
        const uint8_t *private_key, size_t private_key_length,
        const uint8_t *public_key, size_t public_key_length,
        const uint8_t *public_key_id, size_t public_key_id_length,
        hm_rpc_transport_t* user_transport);
#endif //HERMES_CORE_TRANSPORT_H
