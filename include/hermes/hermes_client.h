/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef HERMES_CLIENT_H_
#define HERMES_CLIENT_H_

#include "hermes_document.h"

typedef struct hermes_c_t_ hermes_c_t;

hermes_c_t* hermes_c_create(const char* user_id, const uint8_t* private_key, const size_t private_key_length);
void hermes_c_destroy(hermes_c_t** ctx);

int hermes_c_get_documents_id_list(hermes_c_t_* ctx, char** docs_ids);
int hermes_c_get_document(hermes_c_t_* ctx, const char* doc_id, hermes_document_t** doc);
int hermes_c_update_document(hermes_c_t_* ctx, hermes_document_t* doc);
int hermes_c_create_document(hermes_c_t_* ctx, hermes_document_t* doc);
int hermes_c_delete_document(hermes_c_t_* ctx, hermes_document_t* doc);

#endif /* MIDHERMES_HERMES_H_ */
