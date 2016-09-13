/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef HERMES_DOCUMENT_BLOCK_H_
#define HERMES_DOCUMENT_BLOCK_H_

typedef struct hermes_document_block_t_ hermes_document_block_t;

hermes_document_block_t* hermes_document_block_create(const char* owner_id, const char* doc_id, const char* name, const char* desc);
hermes_document_block_t* hermes_document_block_create_from_blob(const uint8_t* blob, const size_t blob_length);
void hermes_document_block_destroy(hermes_document_block_t** block);

int hermes_document_block_update(hermes_document_block_t* block);
int hermes_document_block_grant_access(hermes_document_block_t* block, const char* user_id, const uint32_t rights_mask);

#endif /* HERMES_DOCUMENT_BLOCK_H_ */
