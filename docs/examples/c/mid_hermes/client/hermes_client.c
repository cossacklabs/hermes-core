/*
* Copyright (c) 2017 Cossack Labs Limited
*
* This file is a part of Hermes-core.
*
* Hermes-core is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Hermes-core is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Hermes-core.  If not, see <http://www.gnu.org/licenses/>.
*
*/




#include <hermes/mid_hermes/mid_hermes.h>
#include "../common/transport.h"
#include "../../utils/base64.h"
#include "../../utils/utils.h"
#include <hermes/secure_transport/transport.h>
#include "../common/config.h"

#include <argp.h>
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


#define SUCCESS 0
#define FAIL 1

const char* const HELP = "usage: client <command> <user id> <base64 encoded user private key>  <name of file for proceed> <meta> <for user>.\n"
        "           <command>                         - executes the command to be performed by the client, see below;\n"
        "           <user id>                         - user identifier (user needs to be registered in Credential store);\n"
        "           <base64 encoded user private key> - base64 encoded private key of the user;\n"
        "           <name of file to be processed>    - filename of the file to be processed (file name is used as block ID in Hermes);\n"
        "           <meta>                            - some data associated with a file that is stored in the database in plaintext;\n"
        "           <for user>                        - identifier of the user for which permissions are provided/revoked from (this information is needed by some commands).\n"
        "\n"
        "commands:\n"
        "           add_block -  add <name of file to be processed> block with <meta> to Hermes system\n"
        "           read_block -  read <name of file to be processed> block with <meta> from Hermes system\n"
        "           update_block -  update <name of file to be processed> block with <meta> in Hermes system\n"
        "           delete_block -  delete <name of file to be processed> block from Hermes system\n"
        "           rotate -   rotate <name of file to be processed> block from Hermes system\n"
        "           grant_read - grant read access for <for user> to <name of file to be processed> block in Hermes system\n"
        "           grant_update - grant update access for <for user> to <name of file to be processed> block in Hermes system\n"
        "           revoke_read - deny read access for <for user> to <name of file to be processed> block in Hermes system\n"
        "           revoke_update - deny update access for <for user> to <name of file to be processed> block in Hermes system\n";



typedef struct transports_container_type{
    hm_rpc_transport_t* credential_store_transport;
    hm_rpc_transport_t* key_store_transport;
    hm_rpc_transport_t* data_store_transport;
    hm_rpc_transport_t* raw_credential_store_transport;
    hm_rpc_transport_t* raw_key_store_transport;
    hm_rpc_transport_t* raw_data_store_transport;
} transports_container_t;

void destroy_transports_container(transports_container_t* container){
    if(!container){
        return;
    }
    if(container->credential_store_transport){
        destroy_rpc_secure_transport(&(container->credential_store_transport));
    }
    if(container->key_store_transport){
        destroy_rpc_secure_transport(&(container->key_store_transport));
    }
    if(container->data_store_transport){
        destroy_rpc_secure_transport(&(container->data_store_transport));
    }
    if (container->raw_credential_store_transport){
        transport_destroy(&(container->raw_credential_store_transport));
    }
    if (container->raw_key_store_transport){
        transport_destroy(&(container->raw_key_store_transport));
    }
    if (container->raw_data_store_transport){
        transport_destroy(&(container->raw_data_store_transport));
    }
}

int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0){
        fprintf(stdout, HELP);
        return SUCCESS;
    }
    if (argc < 5) {
        fprintf(stderr, "error: params error\n\n%s\n", HELP);
        return FAIL;
    }
    transports_container_t container = {0};

    mid_hermes_t *mh = NULL;
    uint8_t sk[1024];
    size_t sk_length = sizeof(sk);
    if (!(sk_length = (size_t)base64_decode(sk, argv[3], (int)sk_length))){
        destroy_transports_container(&container);
        return FAIL;
    }

    container.raw_credential_store_transport = server_connect(CREDENTIAL_STORE_IP, CREDENTIAL_STORE_PORT);
    if (!container.raw_credential_store_transport){
        fprintf(stderr, "can't connect to credential store\n");
        return FAIL;
    }
    container.credential_store_transport = create_secure_transport(
            (uint8_t*)argv[2], strlen(argv[2]), sk, sk_length, credential_store_pk, sizeof(credential_store_pk),
            credential_store_id, strlen((char*)credential_store_id), container.raw_credential_store_transport, false);
    if(!container.credential_store_transport){
        fprintf(stderr, "can't initialize secure transport to credential store\n");
        destroy_transports_container(&container);
        return FAIL;
    }

    container.raw_key_store_transport = server_connect(KEY_STORE_IP, KEY_STORE_PORT);
    if (!container.raw_key_store_transport){
        fprintf(stderr, "can't connect to key store\n");
        destroy_transports_container(&container);
        return FAIL;
    }
    container.key_store_transport = create_secure_transport(
            (uint8_t*)argv[2], strlen(argv[2]), sk, sk_length, key_store_pk, sizeof(key_store_pk),
            key_store_id, strlen((char*)key_store_id), container.raw_key_store_transport, false);
    if(!container.key_store_transport){
        fprintf(stderr, "can't initialize secure transport to key store\n");
        destroy_transports_container(&container);
        return FAIL;
    }

    container.raw_data_store_transport = server_connect(DATA_STORE_IP, DATA_STORE_PORT);
    if (!container.raw_data_store_transport){
        fprintf(stderr, "can't connect to data store\n");
        destroy_transports_container(&container);
        return FAIL;
    }
    container.data_store_transport = create_secure_transport(
            (uint8_t*)argv[2], strlen(argv[2]), sk, sk_length, data_store_pk, sizeof(data_store_pk),
            data_store_id, strlen((char*)data_store_id), container.raw_data_store_transport, false);
    if(!container.data_store_transport){
        fprintf(stderr, "can't initialize secure transport to data store\n");
        destroy_transports_container(&container);
        return FAIL;
    }
    if (!(mh = mid_hermes_create(
            (uint8_t*)argv[2], strlen(argv[2]), sk, sk_length, container.key_store_transport, container.data_store_transport,
            container.credential_store_transport))) {
        fprintf(stderr, "mid_hermes creation error ...\n");
        destroy_transports_container(&container);
        return FAIL;
    }

    if (strcmp(argv[1], "add_block") == 0) {
        size_t idLength = strlen(argv[4]);
        uint8_t* idBuffer = (uint8_t*)argv[4];
        uint8_t *block = NULL;
        size_t block_length = 0;
        if (argc != 6
            || (SUCCESS != read_whole_file(argv[4], &block, &block_length))
            || (0 != mid_hermes_create_block(mh, &idBuffer, &idLength, block, block_length, (uint8_t*)argv[5], strlen(argv[5])))) {
            free(block);
            fprintf(stderr, "error: block adding error\n");
            destroy_transports_container(&container);
            return FAIL;
        }
        fprintf(stdout, "block created with id: ");
        for(size_t i=0; i<idLength; i++){
            fprintf(stdout, "%c", idBuffer[i]);
        }
        fprintf(stdout, "\n");
        free(block);
    } else if (strcmp(argv[1], "read_block") == 0) {
        uint8_t *data = NULL, *meta = NULL;
        size_t data_length = 0, meta_length = 0;
        if (argc != 5 ||
            0 != mid_hermes_read_block(mh, (uint8_t*)argv[4], strlen(argv[4]), &data, &data_length, &meta, &meta_length)) {
            fprintf(stderr, "error: block getting error\n");
            destroy_transports_container(&container);
            return FAIL;
        }
        fprintf(stdout, "file data: ");
        fwrite(data, 1, data_length, stdout);
        fprintf(stdout, "\nmetadata: ");
        fwrite(meta, 1, meta_length, stdout);
        fprintf(stdout, "\n");
        free(data);
        free(meta);
    } else if (strcmp(argv[1], "delete_block") == 0) {
        if (argc != 5
            || 0 != mid_hermes_delete_block(mh, (uint8_t*)argv[4], strlen(argv[4]))) {
            fprintf(stderr, "error: block deleting error\n");
            destroy_transports_container(&container);
            return FAIL;
        }
    } else if (strcmp(argv[1], "update_block") == 0) {
        uint8_t *block = NULL;
        size_t block_length = 0;
        if (argc != 6
            || (SUCCESS != read_whole_file(argv[4], &block, &block_length))
            || (0 != mid_hermes_update_block(mh, (uint8_t*)argv[4], strlen(argv[4]), block, block_length, (uint8_t*)argv[5], strlen(argv[5])))) {
            free(block);
            fprintf(stderr, "error: block adding error\n");
            destroy_transports_container(&container);
            return FAIL;
        }
        free(block);
    } else if (strcmp(argv[1], "grant_read") == 0) {
        if (argc != 6
            || 0 != mid_hermes_grant_read_access(mh, (uint8_t*)argv[4], strlen(argv[4]), (uint8_t*)argv[5], strlen(argv[5]))) {
            fprintf(stderr, "error: block read access granting error\n");
            destroy_transports_container(&container);
            return FAIL;
        }
    } else if (strcmp(argv[1], "grant_update") == 0) {
        if (argc != 6
            || 0 != mid_hermes_grant_update_access(mh, (uint8_t*)argv[4], strlen(argv[4]), (uint8_t*)argv[5], strlen(argv[5]))) {
            fprintf(stderr, "error: block update access granting error\n");
            destroy_transports_container(&container);
            return FAIL;
        }
    } else if (strcmp(argv[1], "revoke_read") == 0) {
        if (argc != 6
            || 0 != mid_hermes_deny_read_access(mh, (uint8_t*)argv[4], strlen(argv[4]), (uint8_t*)argv[5], strlen(argv[5]))) {
            fprintf(stderr, "error: block read access denying error\n");
            destroy_transports_container(&container);
            return FAIL;
        }
    } else if (strcmp(argv[1], "revoke_update") == 0) {
        if (argc != 6
            || 0 != mid_hermes_deny_update_access(mh, (uint8_t*)argv[4], strlen(argv[4]), (uint8_t*)argv[5], strlen(argv[5]))) {
            fprintf(stderr, "error: block update access denying error\n");
            mid_hermes_destroy(&mh);
            destroy_transports_container(&container);
            return FAIL;
        }
    } else if (strcmp(argv[1], "rotate") == 0) {
        if (argc != 5
            || 0 != mid_hermes_rotate_block(mh, (uint8_t*)argv[4], strlen(argv[4]))) {
            fprintf(stderr, "error: block rotate error\n");
            mid_hermes_destroy(&mh);
            destroy_transports_container(&container);
            return FAIL;
        }
    } else {
        fprintf(stderr, "error: undefined command %s\n\n%s\n", argv[1], HELP);
        mid_hermes_destroy(&mh);
        destroy_transports_container(&container);
        return FAIL;
    }
    mid_hermes_destroy(&mh);
    destroy_transports_container(&container);
    return SUCCESS;
}
