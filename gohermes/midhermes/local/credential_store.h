#ifndef GOHERMES_CREDENTIAL_STORE_H
#define GOHERMES_CREDENTIAL_STORE_H

#include <hermes/mid_hermes_ll/interfaces/credential_store.h>
#include <hermes/common/errors.h>
#include <string.h>

// define empty struct that expect mid_hermes_ll and which pointer will be stored on go side
typedef struct hermes_credential_store_type {
} hermes_credential_store_t;

#endif // GOHERMES_CREDENTIAL_STORE_H