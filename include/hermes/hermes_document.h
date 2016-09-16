/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef HERMES_DOCUMENT_H_
#define HERMES_DOCUMENT_H_

#include <hermes/hermes_user.h>
#include <hermes/hermes_document_block.h>

typedef struct hermes_document_t_ hermes_document_t;

hermes_document_t* hermes_document_create(const char* name);
void hermes_document_destroy(hermes_document_t** doc);

int hermes_document_add_block(hermes_document_t* doc, hermes_document_block_t* block);
int hermes_document_delete_block(hermes_document_t* doc, hermes_document_block_t* block);




#endif /* HERMES_DOCUMENT_H_ */
