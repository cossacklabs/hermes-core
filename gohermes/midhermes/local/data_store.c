#include "data_store.h"

// hermes_credential_store_destroy implement hermes/mid_hermes_ll_interfaces/data_store.h interface
hermes_status_t hermes_data_store_destroy(hermes_data_store_t **data_store){
    return HM_SUCCESS;
}

// hermes_data_store_create_block implemented by go function
extern hermes_status_t hermes_data_store_create_block(
     hermes_data_store_t *client,
     const uint8_t *block, const size_t block_length,
     const uint8_t *meta, const size_t meta_length,
     const uint8_t *mac, const size_t mac_length,
     uint8_t **id, size_t *id_length);

// hermes_data_store_create_block_with_id implemented by go function
extern uint32_t hermes_data_store_create_block_with_id(
       hermes_data_store_t *client,
       const uint8_t *id, const size_t id_length,
       const uint8_t *block, const size_t block_length,
       const uint8_t *meta, const size_t meta_length,
       const uint8_t *mac, const size_t mac_length);

// hermes_data_store_update_block implemented by go function
extern uint32_t hermes_data_store_update_block(
       hermes_data_store_t *client,
       const uint8_t *id, const size_t id_length,
       const uint8_t *block, const size_t block_length,
       const uint8_t *meta, const size_t meta_length,
       const uint8_t *mac, const size_t mac_length,
       const uint8_t *old_mac, const size_t old_mac_length);

// hermes_data_store_set_block implement hermes/mid_hermes_ll/interfaces/data_store.h interface and pass
// args to golang func
hermes_status_t hermes_data_store_set_block(
        hermes_data_store_t *data_store,
        uint8_t **id, size_t *id_length,
        const uint8_t *data, const size_t data_length,
        const uint8_t *meta, const size_t meta_length,
        const uint8_t *mac, const size_t mac_length,
        const uint8_t *old_mac, const size_t old_mac_length){

    if (!data_store || !data || !data_length || !meta || !meta_length || !mac || !mac_length || !id || !id_length) {
            return HM_INVALID_PARAMETER;
        }
    if (!old_mac) {
        if (!(*id)) {
            return hermes_data_store_create_block(
                    data_store,
                    data, data_length,
                    meta, meta_length,
                    mac, mac_length,
                    id, id_length);
        }
        return hermes_data_store_create_block_with_id(
                data_store,
                *id, *id_length,
                data, data_length,
                meta, meta_length,
                mac, mac_length);
    }
    return hermes_data_store_update_block(
            data_store,
            *id, *id_length,
            data, data_length,
            meta, meta_length,
            mac, mac_length,
            old_mac, old_mac_length);
};

// hermes_data_store_read_block implemented by go function
extern hermes_status_t hermes_data_store_read_block(
               hermes_data_store_t *data_store,
               const uint8_t *id, const size_t id_length,
               uint8_t **data, size_t *data_length,
               uint8_t **meta, size_t *meta_length);

// hermes_data_store_set_block implement hermes/mid_hermes_ll/interfaces/data_store.h interface and pass
// args to golang func
hermes_status_t hermes_data_store_get_block(
        hermes_data_store_t *data_store,
        const uint8_t *id, const size_t id_length,
        uint8_t **data, size_t *data_length,
        uint8_t **meta, size_t *meta_length){

    return hermes_data_store_read_block(
        data_store,
        id, id_length,
        data, data_length,
        meta, meta_length);

};

// hermes_data_store_delete_block implemented by go function
extern hermes_status_t hermes_data_store_delete_block(
               hermes_data_store_t *data_store,
               const uint8_t *id, const size_t id_length,
               const uint8_t *old_mac, const size_t old_mac_length);

// hermes_data_store_set_block implement hermes/mid_hermes_ll/interfaces/data_store.h interface and pass
// args to golang func
hermes_status_t hermes_data_store_rem_block(
        hermes_data_store_t *data_store,
        const uint8_t *id, const size_t id_length,
        const uint8_t *old_mac, const size_t old_mac_length){
    return hermes_data_store_delete_block(data_store, id, id_length, old_mac, old_mac_length);
};