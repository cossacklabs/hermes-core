/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef SERVICES_KEY_SERVICE_H_
#define SERVICES_KEY_SERVICE_H_

#include "utils.h"
#include "config.h"

#include "service.h"

typedef struct key_service_service_t_ key_service_service_t;

key_service_service_t* key_service_service_create(const config_t* config);
service_status_t key_service_service_destroy(key_service_service_t* service);
service_status_t key_service_service_run(key_service_service_t* service);
service_status_t key_service_service_stop(key_service_service_t* service);


#endif /* SERVICES_KEY_SERVICE_H_ */
