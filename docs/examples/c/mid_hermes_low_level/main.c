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



#include <hermes/mid_hermes_ll/interfaces/key_store.h>
#include <hermes/mid_hermes_ll/interfaces/credential_store.h>
#include <hermes/mid_hermes_ll/interfaces/data_store.h>

#include "key_store_impl.h"
#include "credential_store_impl.h"
#include "data_store_impl.h"

#include <themis/themis.h>

#include <string.h>
#include "commands.h"

#define SUCCESS 0
#define FAIL 1

const char* const HELP = "usage: hermes_client_ll <command> <user id> <base64 encoded user private key>  <name of file for proceed> <meta> <for user>.\n"
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
        //"           rotate -   rotate <name of file to be processed> block from Hermes system\n"
        "           grant_read - grant read access for <for user> to <name of file to be processed> block in Hermes system\n"
        "           grant_update - grant update access for <for user> to <name of file to be processed> block in Hermes system\n"
        "           revoke_read - deny read access for <for user> to <name of file to be processed> block in Hermes system\n"
        "           revoke_update - deny update access for <for user> to <name of file to be processed> block in Hermes system\n";

hermes_key_store_t *ks = NULL;
hermes_credential_store_t *cs = NULL;
hermes_data_store_t *ds = NULL;

int finalize(void) {
    hermes_key_store_destroy(&ks);
    hermes_data_store_destroy(&ds);
    hermes_credential_store_destroy(&cs);
    return 0;
}

int init(void) {
    ks = hermes_key_store_create();
    ds = hermes_data_store_create();
    cs = hermes_credential_store_create();
    if (!ks || !cs || !ds) {
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "error: params error\n\n%s", HELP);
        return FAIL;
    }
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        fprintf(stdout, "%s", HELP);
        return SUCCESS;
    }
    if (0 != init()) {
        fprintf(stderr, "error: initialisation\n");
        finalize();
        return FAIL;
    }
    if (strcmp(argv[1], "read_block") == 0) {
        if (argc != 5 || 0 != get_block(argv[2], argv[3], argv[4])) {
            fprintf(stderr, "error: block getting error\n");
            finalize();
            return FAIL;
        }
    } else if (strcmp(argv[1], "add_block") == 0) {
        if (argc != 6 || 0 != add_block(argv[2], argv[3], argv[4], argv[5])) {
            fprintf(stderr, "error: block adding error\n");
            finalize();
            return FAIL;
        }
    } else if (strcmp(argv[1], "update_block") == 0) {
        if (argc != 6 || 0 != upd_block(argv[2], argv[3], argv[4], argv[5])) {
            fprintf(stderr, "error: block update error\n");
            finalize();
            return FAIL;
        }
    } else if (strcmp(argv[1], "delete_block") == 0) {
        if (argc != 5 || 0 != del_block(argv[2], argv[3], argv[4])) {
            fprintf(stderr, "error: block deleting error\n");
            finalize();
            return FAIL;
        }
    } else if (strcmp(argv[1], "grant_read") == 0) {
        if (argc != 6 || 0 != grant_read(argv[2], argv[3], argv[4], argv[5])) {
            fprintf(stderr, "error: block read access granting error\n");
            finalize();
            return FAIL;
        }
    } else if (strcmp(argv[1], "grant_update") == 0) {
        if (argc != 6 || 0 != grant_update(argv[2], argv[3], argv[4], argv[5])) {
            fprintf(stderr, "error: block update access granting error\n");
            finalize();
            return FAIL;
        }
    } else if (strcmp(argv[1], "revoke_read") == 0) {
        if (argc != 6 || 0 != deny_read(argv[2], argv[3], argv[4], argv[5])) {
            fprintf(stderr, "error: block read access denying error\n");
            finalize();
            return FAIL;
        }
    } else if (strcmp(argv[1], "revoke_update") == 0) {
        if (argc != 6 || 0 != deny_update(argv[2], argv[3], argv[4], argv[5])) {
            fprintf(stderr, "error: block update access denying error\n");
            finalize();
            return FAIL;
        }
//    } else if (strcmp(argv[1], "rotate") == 0) {
//        if (argc != 5 || 0 != rotate_block(argv[2], argv[3], argv[4])) {
//            fprintf(stderr, "error: block rotate error\n");
//            finalize();
//            return FAIL;
//        }
    } else {
        fprintf(stderr, "error: undefined command %s\n\n%s", argv[1], HELP);
        finalize();
        return FAIL;
    }
    finalize();
    return SUCCESS;
}
