#ifndef GOHERMES_DATA_STORE_H
#define GOHERMES_DATA_STORE_H

#include <hermes/mid_hermes_ll/interfaces/data_store.h>
#include <hermes/common/errors.h>
#include <string.h>

// define empty struct that expect mid_hermes_ll and which pointer will be stored on go side
typedef struct hermes_data_store_type {
} hermes_data_store_t;

#endif // GOHERMES_DATA_STORE_H