/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef SERVICES_RECORD_SET_SERVICE_H_
#define SERVICES_RECORD_SET_SERVICE_H_

#include "utils.h"
#include "config.h"

#include "service.h"

typedef struct record_set_service_t_ record_set_service_t;

record_set_service_t* record_set_service_create(const config_t* config);
service_status_t record_set_service_destroy(record_set_service_t* service);
service_status_t record_set_service_run(record_set_service_t* service);
service_status_t record_set_service_stop(record_set_service_t* service);


#endif /* SERVICES_RECORD_SET_SERVICE_H_ */
