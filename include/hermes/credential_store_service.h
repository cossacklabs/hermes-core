/*
 * Copyright (c) 2016 Cossack Labs Limited
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

#ifndef SERVICES_CREDENTIAL_STORE_SERVICE_H_
#define SERVICES_CREDENTIAL_STORE_SERVICE_H_

#include "utils.h"
#include "config.h"

#include "service.h"

typedef struct credential_store_service_t_ credential_store_service_t;

credential_store_service_t* credential_store_service_create(const config_t* config);
service_status_t credential_store_service_destroy(credential_store_service_t* service);
service_status_t credential_store_service_run(credential_store_service_t* service);
service_status_t credential_store_service_stop(credential_store_service_t* service);


#endif /* SERVICES_CREDENTIAL_STORE_SERVICE_H_ */
