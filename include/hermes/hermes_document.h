/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef HERMES_DOCUMENT_H_
#define HERMES_DOCUMENT_H_

#include "hermes_document_block.h"

typedef struct hermes_document_t_ hermes_document_t;

hermes_document_t* hermes_document_create(const char* owner_id, const char* name, const char* desc);
hermes_document_t* hermes_document_create_from_blob(const uint8_t* blob, const size_t blob_length);
void hermes_document_destroy(hermes_document_t** doc);

int hermes_document_add_block(hermes_document_t** doc, hermes_document_block_t* block);
int hermes_document_update_block(hermes_document_t** doc, hermes_document_block_t* block);
int hermes_document_delete_block(hermes_document_t** doc, hermes_document_block_t* block);
int hermes_document_delete_block_by_id(hermes_document_t** doc, const char* block_id);

#endif /* HERMES_DOCUMENT_H_ */
