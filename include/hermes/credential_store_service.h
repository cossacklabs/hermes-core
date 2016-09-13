/*
 * Copyright (c) 2016 Cossack Labs Limited
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
