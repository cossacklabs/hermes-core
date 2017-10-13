#ifndef HERMES_CORE_UTILS_H
#define HERMES_CORE_UTILS_H
#include <string.h>
#include <stdint.h>
#include <hermes/rpc/transport.h>
#include <hermes/common/errors.h>
uint32_t read_data_size(hm_rpc_transport_t* transport);
uint32_t read_data(uint8_t* data, size_t data_length, hm_rpc_transport_t* transport);
uint32_t send_data(const uint8_t* data, size_t data_length, hm_rpc_transport_t* transport);
#endif //HERMES_CORE_UTILS_H
