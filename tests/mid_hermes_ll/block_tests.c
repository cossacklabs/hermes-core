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

#include <string.h>
#include <hermes/mid_hermes_ll/mid_hermes_ll.h>
#include <themis/secure_message.h>
#include <common/test_utils.h>
#include <common/common.h>
#include "block_tests.h"

int mid_hermes_ll_block_tests(){
    // generate test keypair
    uint8_t* private_key=NULL;
    size_t private_key_length=0;
    uint8_t* public_key=NULL;
    size_t public_key_length=0;
    if(themis_gen_ec_key_pair(private_key, &private_key_length, public_key, &public_key_length)!=THEMIS_BUFFER_TOO_SMALL){
        testsuite_fail_if(true, "get length for private or public keys");
        TEST_FAIL;
    }
    private_key = malloc(private_key_length);
    public_key = malloc(public_key_length);
    if(!private_key || !public_key){
        testsuite_fail_if(true, "malloc memory for private|public key");
        return TEST_FAIL;
    }
    if(themis_gen_ec_key_pair(private_key, &private_key_length, public_key, &public_key_length)!=THEMIS_SUCCESS){
        testsuite_fail_if(true, "generate key pair");
        return TEST_FAIL;
    }

    // initialize test values as hermes_buffer
    uint8_t *userId = (uint8_t*)"user id";
    mid_hermes_ll_buffer_t* userBuffer = mid_hermes_ll_buffer_create(userId, 7);
    mid_hermes_ll_buffer_t* privateKeyBuffer = mid_hermes_ll_buffer_create(private_key, private_key_length);
    mid_hermes_ll_buffer_t* publicKeyBuffer = mid_hermes_ll_buffer_create(public_key, public_key_length);

    mid_hermes_ll_user_t* user = mid_hermes_ll_local_user_create(userBuffer, privateKeyBuffer, publicKeyBuffer);

    uint8_t *blockId = (uint8_t*) "block id";
    mid_hermes_ll_buffer_t* blockIdBuffer = mid_hermes_ll_buffer_create(blockId, 8);

    uint8_t *blockData = (uint8_t*) "block data";
    size_t dataLength = 10;
    mid_hermes_ll_buffer_t* blockDataBuffer = mid_hermes_ll_buffer_create(blockData, dataLength);

    uint8_t *blockMeta = (uint8_t*) "block meta";
    mid_hermes_ll_buffer_t* blockMetaBuffer = mid_hermes_ll_buffer_create(blockMeta, 10);

    mid_hermes_ll_token_t* readToken = mid_hermes_ll_token_generate(mid_hermes_ll_user_copy(user));
    mid_hermes_ll_token_t* writeToken = mid_hermes_ll_token_generate(mid_hermes_ll_user_copy(user));


    // test that block not created without write token
    mid_hermes_ll_block_t *block = mid_hermes_ll_block_create_new(user, blockIdBuffer, blockDataBuffer, blockMetaBuffer, readToken, NULL);
    testsuite_fail_if(block != NULL, "block_create_new not return block without write token");
    if (block){
        return TEST_FAIL;
    }

    // check that block not created with incorrect read token
    mid_hermes_ll_token_t* invalidToken = mid_hermes_ll_token_create(
            mid_hermes_ll_user_copy(user), mid_hermes_ll_user_copy(user),
            mid_hermes_ll_buffer_create((uint8_t*)"invalid token", 13));
    block = mid_hermes_ll_block_create_new(user, blockIdBuffer, blockDataBuffer, blockMetaBuffer, invalidToken, writeToken);
    testsuite_fail_if(block != NULL, "block_create_new not return block with invalid read token");
    if (block){
        return TEST_FAIL;
    }
    // check that block not created with incorrect write token
    block = mid_hermes_ll_block_create_new(user, blockIdBuffer, blockDataBuffer, blockMetaBuffer, readToken, invalidToken);
    testsuite_fail_if(block != NULL, "block_create_new not return block with invalid write token");
    if (block){
        return TEST_FAIL;
    }

    block = mid_hermes_ll_block_create_new(user, blockIdBuffer, blockDataBuffer, blockMetaBuffer, readToken, writeToken);
    testsuite_fail_if(block == NULL, "block_create_new return block != NULL");
    if (!block){
        return TEST_FAIL;
    }
    // check that passed data is equal to data in block
    if (memcmp(blockData, block->data->data, dataLength) != 0){
        testsuite_fail_if(true, "block->block == blockData, not encrypted data equals to passed data");
        return TEST_FAIL;
    }
    // check that passed data is not equal to encrypted data in block
    if (memcmp(blockData, block->block->data, dataLength) == 0){
        testsuite_fail_if(true, "block->data != blockData, encrypted data not equals to passed data");
        return TEST_FAIL;
    }

    testsuite_fail_if(!block->wtoken, "block->wtoken != NULL");
    testsuite_fail_if(!block->rtoken, "block->rtoken != NULL");
    testsuite_fail_if(!block->id, "block->id != NULL");
    testsuite_fail_if(!block->meta, "block->meta != NULL");
    testsuite_fail_if(!block->mac, "block->mac != NULL");
    testsuite_fail_if(!block->block, "block->block != NULL");
    testsuite_fail_if(!block->user, "block->user != NULL");

    return TEST_SUCCESS;
}