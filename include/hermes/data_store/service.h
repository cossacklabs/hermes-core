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


#ifndef HM_DATA_STORE_SERVICE
#define HM_DATA_STORE_SERVICE

#include <hermes/rpc/transport.h>
#include <hermes/data_store/db.h>

typedef struct hm_credential_server_service_type hm_data_store_service_t;

hm_data_store_service_t* hm_data_store_service_create(hm_rpc_transport_t* transport, hm_ds_db_t* db);
uint32_t hm_data_store_service_destroy(hm_data_store_service_t** service);
uint32_t hm_data_store_service_start(hm_data_store_service_t* service);
uint32_t hm_data_store_service_stop(hm_data_store_service_t* service);

#endif //HM_DATA_STORE_SERVICE
