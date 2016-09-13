/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef MIDHERMES_RECORD_SET_H_
#define MIDHERMES_RECORD_SET_H_

#include "config.h"
#include "record_set_protocol.h"

typedef struct record_set_client_t_ record_set_client_t;

int record_set_client_get_docs_list(record_set_client_t* rsc, char** res);
int record_set_client_read_doc(record_set_client_t* rsc, const char* user_id, const char* id, char** res);
int record_set_client_add_doc(record_set_client_t* rsc, const char* user_id, const char* doc, const uint8_t* mac, const size_t mac_length, const uint8_t* rt, const size_t rt_length, const uint8_t* ut, const size_t ut_length, const uint8_t* pub_key, const size_t pub_key_length);
int record_set_client_update_doc(record_set_client_t* rsc, const char* user_id, const char* doc, const uint8_t* mac, const size_t mac_length, const char* doc_id, const uint8_t* old_mac, const size_t old_mac_length);
int record_set_client_add_public_block(record_set_client_t* rsc, const char* user_id, const char* doc_id, const uint8_t* mac, const size_t mac_length, const char* name, const uint8_t* block, const size_t block_length);
int record_set_client_add_private_block(record_set_client_t* rsc, const char* user_id, const char* doc_id, const uint8_t* mac, const size_t mac_length, const char* name, const uint8_t* block, const size_t block_length);
int record_set_client_add_access(record_set_client_t* rsc, const char* user_id, const char* doc_id, const uint8_t* mac, const size_t mac_length, const char* new_user_name, const uint8_t* ut, const size_t ut_length, const uint8_t* rt, const size_t rt_length, const uint8_t* pubk, const size_t pubk_length);
int record_set_client_rm_access(record_set_client_t* rsc, const char* user_id, const char* doc_id, const uint8_t* mac, const size_t mac_length, const char* new_user_name);
int record_set_client_get_public_block(record_set_client_t* rsc, const char* user_id, const char* block_id, const char* doc_id, uint8_t** block, size_t* block_length);
int record_set_client_get_private_block(record_set_client_t* rsc, const char* user_id, const char* block_id, const char* doc_id, uint8_t** block, size_t* block_length);
int record_set_client_rm_public_block(record_set_client_t* rsc, const char* user_id, const char* block_id, const char* doc_id, const uint8_t* mac, const size_t mac_length);
int record_set_client_rm_private_block(record_set_client_t* rsc, const char* user_id, const char* block_id, const char* doc_id, const uint8_t* mac, const size_t mac_length);

/*int record_set_client_add_doc(record_set_client_t* rsc, const char* data, record_set_client_get_id_handler_t handler, void* ctx);
int record_set_client_add_public_block(record_set_client_t* rsc, const char* doc_id, const uint8_t* data, const size_t data_length, record_set_client_get_id_handler_t handler, void* ctx);
int record_set_client_add_private_block(record_set_client_t* rsc, const char* doc_id, const uint8_t* data, const size_t data_length, record_set_client_get_id_handler_t handler, void* ctx);

int record_set_client_get_doc(record_set_client_t* rsc, const char* id, record_set_client_get_doc_handler_t handler, void* ctx);
int record_set_client_get_doc_data(record_set_client_t* rsc, const char* id, record_set_client_get_doc_handler_t handler, void* ctx);
int record_set_client_get_doc_breaf(record_set_client_t* rsc, const char* id, record_set_client_get_doc_handler_t handler, void* ctx);
int record_set_client_get_public_block(record_set_client_t* rsc, const char* id, record_set_client_get_block_handler_t handler, void* ctx);
int record_set_client_get_private_block(record_set_client_t* rsc, const char* id, record_set_client_get_block_handler_t handler, void* ctx);

int record_set_client_update_doc(record_set_client_t* rsc, const char* id, const uint8_t* ut, const char* old_data, const char* data, record_set_client_get_res_handler_t handler, void* ctx);
int record_set_client_update_public_block(record_set_client_t* rsc, const char* id, const uint8_t* ut, const uint8_t* old_data, const size_t old_data_len, const uint8_t* data, const size_t data_len, record_set_client_get_res_handler_t handler, void* ctx);
int record_set_client_update_private_block(record_set_client_t* rsc, const char* id, const uint8_t* ut, const uint8_t* old_data, const size_t old_data_len, const uint8_t* data, const size_t data_len, record_set_client_get_res_handler_t handler, void* ctx);

int record_set_client_delete_doc(record_set_client_t* rsc, const char* id, const uint8_t* ut, const size_t ut_len, record_set_client_get_res_handler_t handler, void* ctx);
int record_set_client_delete_public_block(record_set_client_t* rsc, const char* id, const uint8_t* ut, const size_t ut_len, record_set_client_get_res_handler_t handler, void* ctx);
int record_set_client_delete_private_block(record_set_client_t* rsc, const char* id, const uint8_t* ut, const size_t ut_len, record_set_client_get_res_handler_t handler, void* ctx);
*/
record_set_client_t* record_set_client_create(const config_t* config);
int record_set_client_destroy(record_set_client_t* ctx);


#endif /* MIDHERMES_RECORD_SET_H_ */
