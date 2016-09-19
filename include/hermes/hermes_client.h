/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef HERMES_CLIENT_H_
#define HERMES_CLIENT_H_

#include <hermes/hermes_user.h>

typedef struct hermes_client_t_ hermes_client_t;

hermes_client_t* hermes_client_create(const char* user_id, const uint8_t* private_key, const size_t private_key_length);
void hermes_client_destroy(hermes_client_t** ctx);

//Document CRUD
int hermes_client_create_document(hermes_client_t* ctx, const uint8_t* data, const size_t data_length, char** new_id);
int hermes_client_read_document(hermes_client_t* ctx, const char* doc_id, uint8_t** data, size_t* data_length);
int hermes_client_update_document(hermes_client_t* ctx, const char* doc_id, const uint8_t* data, const size_t data_length);
int hermes_client_delete_document(hermes_client_t* ctx, const char* doc_id);

//block CRUD
int hermes_client_create_document_block(hermes_client_t* ctx, const char* doc_id, bool is_private, const uint8_t* data, const size_t data_length, char** new_block_id);
int hermes_client_read_document_block(hermes_client_t* ctx, const char* doc_id, const char* block_id, uint8_t** data, size_t* data_length);
int hermes_client_update_document_block(hermes_client_t* ctx, const char* doc_id, const char* block_id, const uint8_t* data, const size_t data_length);
int hermes_client_delete_document_block(hermes_client_t* ctx, const char* doc_id, const char* block_id);

//manipulate user rights
int hermes_client_grant_access_to_document(hermes_client_t* ctx, const char* doc_id, const char* user_id_granting_to, int rights_mask);
int hermes_client_deny_access_to_document(hermes_client_t* ctx, const char* doc_id, const char* user_id_denying_to);
int hermes_client_grant_access_to_document_block(hermes_client_t* ctx, const char* doc_id, const char* block_id, const char* user_id_granting_to, int rights_mask);
int hermes_client_deny_access_to_document_block(hermes_client_t* ctx, const char* doc_id, const char* block_id, const char* user_id_denying_to);


#endif /* MIDHERMES_HERMES_H_ */
