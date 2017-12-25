#include "key_store.h"

hermes_status_t hermes_key_store_destroy(hermes_key_store_t **key_store){
    return HM_SUCCESS;
}

extern hermes_status_t hermes_key_store_get_read_token_callback(
        hermes_key_store_t *key_store,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        uint8_t **token, size_t *token_length,
        uint8_t **owner_id, size_t *owner_id_length);

hermes_status_t hermes_key_store_get_rtoken(
        hermes_key_store_t *key_store,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        uint8_t **token, size_t *token_length,
        uint8_t **owner_id, size_t *owner_id_length){

    return hermes_key_store_get_read_token_callback(
        key_store,
        user_id, user_id_length,
        block_id, block_id_length,
        token, token_length,
        owner_id, owner_id_length);
}


extern hermes_status_t hermes_key_store_get_write_token_callback(
               hermes_key_store_t *key_store,
               const uint8_t *user_id, const size_t user_id_length,
               const uint8_t *block_id, const size_t block_id_length,
               uint8_t **token, size_t *token_length,
               uint8_t **owner_id, size_t *owner_id_length);

hermes_status_t hermes_key_store_get_wtoken(
        hermes_key_store_t *key_store,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        uint8_t **token, size_t *token_length,
        uint8_t **owner_id, size_t *owner_id_length){

    return hermes_key_store_get_write_token_callback(
            key_store,
            user_id,user_id_length,
            block_id, block_id_length,
            token, token_length,
            owner_id, owner_id_length);
}

extern hermes_status_t hermes_key_store_set_read_token_callback(
               hermes_key_store_t *key_store,
               const uint8_t *user_id, const size_t user_id_length,
               const uint8_t *block_id, const size_t block_id_length,
               const uint8_t *token, const size_t token_length,
               const uint8_t *owner_id, const size_t owner_id_length);
hermes_status_t hermes_key_store_set_rtoken(
        hermes_key_store_t *key_store,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *token, const size_t token_length,
        const uint8_t *owner_id, const size_t owner_id_length){

    return hermes_key_store_set_read_token_callback(
        key_store,
        user_id, user_id_length,
        block_id, block_id_length,
        token, token_length,
        owner_id, owner_id_length);
}

extern hermes_status_t hermes_key_store_set_write_token_callback(
        hermes_key_store_t *key_store,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *token, const size_t token_length,
        const uint8_t *owner_id, const size_t owner_id_length);

hermes_status_t hermes_key_store_set_wtoken(
        hermes_key_store_t *key_store,
        const uint8_t *user_id, const size_t user_id_length,
        const uint8_t *block_id, const size_t block_id_length,
        const uint8_t *token, const size_t token_length,
        const uint8_t *owner_id, const size_t owner_id_length){

    return hermes_key_store_set_write_token_callback(
        key_store,
        user_id, user_id_length,
        block_id, block_id_length,
        token, token_length,
        owner_id, owner_id_length);
}

struct hermes_key_store_iterator_type {
    hermes_key_store_t *ks;
    uint8_t *block_id;
    size_t block_id_length;
    uint8_t *current_user_id;
    size_t current_user_id_length;
    uint32_t current_rights_mask;
    size_t current_index;
};

extern hermes_status_t hermes_key_store_get_indexed_rights_callback(
    void* db, const uint8_t* block_id, const size_t block_id_length,
    const size_t index, uint8_t** user_id,
    size_t* user_id_length, uint32_t* rights_mask);

hermes_key_store_iterator_t *hermes_key_store_iterator_create(
        hermes_key_store_t *key_store, const uint8_t *block_id, const size_t block_id_length) {
    if (!key_store || !block_id || !block_id_length) {
        return NULL;
    }
    hermes_key_store_iterator_t *iterator = calloc(1, sizeof(hermes_key_store_iterator_t));
    assert(iterator);
    iterator->block_id = malloc(block_id_length);
    assert(iterator->block_id);
    memcpy(iterator->block_id, block_id, block_id_length);
    iterator->block_id_length = block_id_length;
    iterator->ks = key_store;
    if (HM_SUCCESS != hermes_key_store_get_indexed_rights_callback(
            key_store, block_id, block_id_length, iterator->current_index,
            &(iterator->current_user_id), &(iterator->current_user_id_length), &(iterator->current_rights_mask))) {
        hermes_key_store_iterator_destroy(&iterator);
        return NULL;
    }
    return iterator;
}

hermes_status_t hermes_key_store_iterator_next(hermes_key_store_iterator_t *iterator) {
    if (!iterator) {
        return HM_INVALID_PARAMETER;
    }
    free(iterator->current_user_id);
    iterator->current_user_id=NULL;
    if (HM_SUCCESS != hermes_key_store_get_indexed_rights_callback(
            iterator->ks, iterator->block_id, iterator->block_id_length, ++(iterator->current_index),
            &(iterator->current_user_id), &(iterator->current_user_id_length), &(iterator->current_rights_mask))) {
        return HM_FAIL;
    }
    return HM_SUCCESS;
}

uint8_t *hermes_key_store_iterator_get_user_id(hermes_key_store_iterator_t *iterator) {
    if (!iterator) {
        return NULL;
    }
    return iterator->current_user_id;
}

size_t hermes_key_store_iterator_get_user_id_length(hermes_key_store_iterator_t *iterator) {
    if (!iterator) {
        return 0;
    }
    return iterator->current_user_id_length;
}

hermes_status_t hermes_key_store_iterator_destroy(hermes_key_store_iterator_t **iterator) {
    if (!iterator || !(*iterator)) {
        return HM_INVALID_PARAMETER;
    }
    free((*iterator)->block_id);
    free((*iterator)->current_user_id);
    free(*iterator);
    *iterator = NULL;
    return HM_SUCCESS;
}
