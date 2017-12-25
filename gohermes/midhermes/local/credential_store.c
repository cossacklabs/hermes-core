#include "credential_store.h"

// hermes_credential_store_get_public_key_callback implemented by go function
extern hermes_status_t hermes_credential_store_get_public_key_callback(
               hermes_credential_store_t *credential_store,
               const uint8_t *user_id, const size_t user_id_length,
               uint8_t **public_key, size_t *public_key_length);

// hermes_credential_store_get_public_key implement hermes/mid_hermes_ll/interfaces/credential_store.h interface and pass
// args to golang func
hermes_status_t hermes_credential_store_get_public_key(
               hermes_credential_store_t *credential_store,
               const uint8_t *user_id, const size_t user_id_length,
               uint8_t **public_key, size_t *public_key_length){
    return hermes_credential_store_get_public_key_callback(credential_store, user_id, user_id_length, public_key, public_key_length);
}

// hermes_credential_store_destroy implement hermes/mid_hermes_ll_interfaces/credential_store.h interface
hermes_status_t hermes_credential_store_destroy(hermes_credential_store_t **credential_store){
    return HM_SUCCESS;
}