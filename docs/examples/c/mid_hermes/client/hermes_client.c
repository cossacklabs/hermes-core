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
#include "hermes/secure_transport/transport.h"
#include "config.h"

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

#define CREDENTIAL_STORE_IP   "127.0.0.1"
#define DATA_STORE_IP         "127.0.0.1"
#define KEY_STORE_IP          "127.0.0.1"

#define CREDENTIAL_STORE_PORT 8888
#define DATA_STORE_PORT       8890
#define KEY_STORE_PORT        8889

const char const* HELP = "usage: client <command> <user id> <base64 encoded user private key>  <name of file for proceed> <meta> <for user>.\n"
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

hm_rpc_transport_t *server_connect(const char *ip, int port) {
    int64_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        fprintf(stderr, "connection error (1) (%s:%i)\n", ip, port);
        return NULL;
    }
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        fprintf(stderr, "connection error (2) (%s:%i)\n", ip, port);
        return NULL;
    }

    hm_rpc_transport_t *transport = transport_create(sock);
    if (!transport) {
        close(sock);
        fprintf(stderr, "connection error (3) (%s:%i)\n", ip, port);
        return NULL;
    }
    return transport;
}

int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0){
        fprintf(stdout, HELP);
        return SUCCESS;
    }
    if (argc < 5) {
        fprintf(stderr, "error: params error\n\n%s\n", HELP);
        return 1;
    }
    hm_rpc_transport_t *tcp_cs_transport = server_connect(CREDENTIAL_STORE_IP, CREDENTIAL_STORE_PORT);
    hm_rpc_transport_t *tcp_ks_transport = server_connect(KEY_STORE_IP, KEY_STORE_PORT);
    hm_rpc_transport_t *tcp_ds_transport = server_connect(DATA_STORE_IP, DATA_STORE_PORT);
    if (!(tcp_cs_transport && tcp_ks_transport && tcp_ds_transport)){
        return 1;
    }
    mid_hermes_t *mh = NULL;
    uint8_t sk[1024];
    size_t sk_length = sizeof(sk);
    if (!(sk_length = base64_decode(sk, argv[3], sk_length))){
        return FAIL;
    }
    fprintf(stderr, "cs transport\n");
    hm_rpc_transport_t *cs_transport = create_secure_transport(argv[2], strlen(argv[2]), sk, sk_length, credential_store_pk, sizeof(credential_store_pk), credential_store_id, strlen(credential_store_id), tcp_cs_transport);
    if(!cs_transport){return 1;}
    fprintf(stderr, "ks transport\n");
    hm_rpc_transport_t *ks_transport = create_secure_transport(argv[2], strlen(argv[2]), sk, sk_length, key_store_pk, sizeof(key_store_pk), key_store_id, strlen(key_store_id), tcp_ks_transport);
    if(!ks_transport){return 1;}
    fprintf(stderr, "ds transport\n");
    hm_rpc_transport_t *ds_transport = create_secure_transport(argv[2], strlen(argv[2]), sk, sk_length, data_store_pk, sizeof(data_store_pk), data_store_id, strlen(data_store_id), tcp_ds_transport);
    if(!ds_transport){return 1;}
    if (!cs_transport
        || !ds_transport
        || !ks_transport
        || !(mh = mid_hermes_create(argv[2], strlen(argv[2]), sk, sk_length, ks_transport, ds_transport, cs_transport))) {
        transport_destroy(&cs_transport);
        transport_destroy(&ds_transport);
        transport_destroy(&ks_transport);
        fprintf(stderr, "mid_hermes creation error ...\n");
        return FAIL;
    }

    if (strcmp(argv[1], "add_block") == 0) {
        size_t id_length = strlen(argv[4]);
        uint8_t *block = NULL;
        uint8_t* idBuffer = NULL;
        size_t idBufferLength;
        size_t block_length = 0;
        if (argc != 6
            || (SUCCESS != read_whole_file(argv[4], &block, &block_length))
            || (0 != mid_hermes_create_block(mh, &idBuffer, &idBufferLength, block, block_length, argv[5], strlen(argv[5])))) {
            free(block);
            fprintf(stderr, "error: block adding error\n");
            return FAIL;
        }
        fprintf(stdout, "block created with id: ");
        for(int i=0; i<idBufferLength; i++){
            fprintf(stdout, "%c", idBuffer[i]);
        }
        fprintf(stdout, "\n");
        free(block);
        return SUCCESS;
    } else if (strcmp(argv[1], "read_block") == 0) {
        uint8_t *data = NULL, *meta = NULL;
        size_t data_length = 0, meta_length = 0;
        if (argc != 5 ||
            0 != mid_hermes_read_block(mh, argv[4], strlen(argv[4]), &data, &data_length, &meta, &meta_length)) {
            fprintf(stderr, "error: block getting error\n");
            return FAIL;
        }
        fwrite(data, 1, data_length, stdout);
        fwrite(meta, 1, meta_length, stderr);
        free(data);
        free(meta);
        return SUCCESS;
    } else if (strcmp(argv[1], "delete_block") == 0) {
        if (argc != 5
            || 0 != mid_hermes_delete_block(mh, argv[4], strlen(argv[4]))) {
            fprintf(stderr, "error: block deleting error\n");
            return FAIL;
        }
        return SUCCESS;
    } else if (strcmp(argv[1], "update_block") == 0) {
        uint8_t *block = NULL;
        size_t block_length = 0;
        if (argc != 6
            || (SUCCESS != read_whole_file(argv[4], &block, &block_length))
            || (0 != mid_hermes_update_block(mh, argv[4], strlen(argv[4]), block, block_length, argv[5], strlen(argv[5])))) {
            free(block);
            fprintf(stderr, "error: block adding error\n");
            return FAIL;
        }
        free(block);
        return SUCCESS;
    } else if (strcmp(argv[1], "grant_read") == 0) {
        if (argc != 6
            || 0 != mid_hermes_grant_read_access(mh, argv[4], strlen(argv[4]), argv[5], strlen(argv[5]))) {
            fprintf(stderr, "error: block read access granting error\n");
            return FAIL;
        }
        return SUCCESS;
    } else if (strcmp(argv[1], "grant_update") == 0) {
        if (argc != 6
            || 0 != mid_hermes_grant_update_access(mh, argv[4], strlen(argv[4]), argv[5], strlen(argv[5]))) {
            fprintf(stderr, "error: block update access granting error\n");
            return FAIL;
        }
        return SUCCESS;
    } else if (strcmp(argv[1], "revoke_read") == 0) {
        if (argc != 6
            || 0 != mid_hermes_deny_read_access(mh, argv[4], strlen(argv[4]), argv[5], strlen(argv[5]))) {
            fprintf(stderr, "error: block read access denying error\n");
            return FAIL;
        }
        return SUCCESS;
    } else if (strcmp(argv[1], "revoke_update") == 0) {
        if (argc != 6
            || 0 != mid_hermes_deny_update_access(mh, argv[4], strlen(argv[4]), argv[5], strlen(argv[5]))) {
            fprintf(stderr, "error: block update access denying error\n");
            return FAIL;
        }
        return SUCCESS;
    } else if (strcmp(argv[1], "rotate") == 0) {
        if (argc != 5
            || 0 != mid_hermes_rotate_block(mh, argv[4], strlen(argv[4]))) {
            fprintf(stderr, "error: block rotate error\n");
            return FAIL;
        }
        return SUCCESS;
    } else {
        fprintf(stderr, "error: undefined command %s\n\n%s\n", argv[1], HELP);
        return FAIL;
    }
    mid_hermes_destroy(&mh);
    transport_destroy(&cs_transport);
    transport_destroy(&ds_transport);
    transport_destroy(&ks_transport);
    return SUCCESS;
}
