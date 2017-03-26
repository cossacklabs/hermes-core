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


#ifndef HERMES_TEST_TRANSPORT_H
#define HERMES_TEST_TRANSPORT_H

#include <hermes/rpc/transport.h>

#include <stdbool.h>

#define CS_PIPE_NAME "/tmp/hermes_core_test_cs_pipe" 
#define SC_PIPE_NAME "/tmp/hermes_core_test_sc_pipe"

hm_rpc_transport_t* hm_test_transport_create(bool is_server);
uint32_t hm_test_transport_destroy(hm_rpc_transport_t* t);

#endif //HERMES_TEST_TRANSPORT_H
