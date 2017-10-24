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

int mid_hermes_ll_block_buffer_equality_tests ();
int mid_hermes_ll_block_user_equality_tests ();
int mid_hermes_ll_block_cst_time_memcmp_tests ();


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

    // test equality
    int equalityTestResult = mid_hermes_ll_block_buffer_equality_tests();
    testsuite_fail_if(equalityTestResult != TEST_SUCCESS, "mid_hermes_ll_block_buffer_equality_tests check");

    equalityTestResult = mid_hermes_ll_block_user_equality_tests();
    testsuite_fail_if(equalityTestResult != TEST_SUCCESS, "mid_hermes_ll_block_user_equality_tests check");

    equalityTestResult = mid_hermes_ll_block_cst_time_memcmp_tests();
    testsuite_fail_if(equalityTestResult != TEST_SUCCESS, "mid_hermes_ll_block_cst_time_memcmp_tests check");

    return TEST_SUCCESS;
}

int mid_hermes_ll_block_buffer_equality_tests () {
    uint8_t *userId1 = (uint8_t*)"user id1";
    mid_hermes_ll_buffer_t* userBuffer1 = mid_hermes_ll_buffer_create(userId1, 8);

    uint8_t *userId1Twin = (uint8_t*)"user id1";
    mid_hermes_ll_buffer_t* userBuffer1Twin = mid_hermes_ll_buffer_create(userId1, 8);

    uint8_t *userId2 = (uint8_t*)"user id2";
    mid_hermes_ll_buffer_t* userBuffer2 = mid_hermes_ll_buffer_create(userId2, 8);

    uint8_t *userId3 = (uint8_t*)"user id3";
    mid_hermes_ll_buffer_t* userBufferWrongLength = mid_hermes_ll_buffer_create(userId3, 1);

    mid_hermes_ll_buffer_t* userBufferWrongData = mid_hermes_ll_buffer_create(NULL, 1);
    mid_hermes_ll_buffer_t* userBufferWrongData2 = mid_hermes_ll_buffer_create(NULL, 56);

    bool isEqual = mid_hermes_ll_buffer_is_equal(userBuffer1, userBuffer1);
    testsuite_fail_if(!isEqual, "buffer_is_equal should be equal if block is the same");

    isEqual = mid_hermes_ll_buffer_is_equal(userBuffer1, userBuffer1Twin);
    testsuite_fail_if(!isEqual, "buffer_is_equal should be equal if blocks have same content");

    isEqual = mid_hermes_ll_buffer_is_equal(userBuffer1, userBuffer2);
    testsuite_fail_if(isEqual, "buffer_is_equal should NOT be equal if blocks are different");

    isEqual = mid_hermes_ll_buffer_is_equal(userBuffer2, userBufferWrongLength);
    testsuite_fail_if(isEqual, "buffer_is_equal should NOT be equal if blocks have different lenght");

    isEqual = mid_hermes_ll_buffer_is_equal(userBuffer1, userBufferWrongData);
    testsuite_fail_if(isEqual, "buffer_is_equal should NOT be equal if block has NULL data");

    isEqual = mid_hermes_ll_buffer_is_equal(userBufferWrongData2, userBufferWrongData);
    testsuite_fail_if(isEqual, "buffer_is_equal should NOT be equal if both blocks have NULL data");

    return TEST_SUCCESS; 
}

int mid_hermes_ll_block_user_equality_tests () {

    int KEY_LENGTH = 256;

    size_t private_key_length1=KEY_LENGTH;
    size_t public_key_length1=KEY_LENGTH;

    uint8_t* private_key1 = malloc(private_key_length1);
    uint8_t* public_key1 = malloc(public_key_length1);
    if(themis_gen_ec_key_pair(private_key1, &private_key_length1, public_key1, &public_key_length1)!=THEMIS_SUCCESS){
        testsuite_fail_if(true, "generate key pair");
        return TEST_FAIL;
    }

    size_t private_key_length2=KEY_LENGTH;
    size_t public_key_length2=KEY_LENGTH;

    uint8_t* private_key2 = malloc(private_key_length2);
    uint8_t* public_key2 = malloc(public_key_length2);
    if(themis_gen_ec_key_pair(private_key2, &private_key_length2, public_key2, &public_key_length2)!=THEMIS_SUCCESS){
        testsuite_fail_if(true, "generate key pair");
        return TEST_FAIL;
    }

    uint8_t *userId1 = (uint8_t*)"user id1";
    mid_hermes_ll_buffer_t* userBuffer1 = mid_hermes_ll_buffer_create(userId1, 8);
    mid_hermes_ll_buffer_t* privateKeyBuffer1 = mid_hermes_ll_buffer_create(private_key1, private_key_length1);
    mid_hermes_ll_buffer_t* publicKeyBuffer1 = mid_hermes_ll_buffer_create(public_key1, public_key_length1);
    mid_hermes_ll_user_t* user1 = mid_hermes_ll_local_user_create(userBuffer1, privateKeyBuffer1, publicKeyBuffer1);


    uint8_t *userId2 = (uint8_t*)"user id2";
    mid_hermes_ll_buffer_t* userBuffer2 = mid_hermes_ll_buffer_create(userId2, 8);
    mid_hermes_ll_buffer_t* privateKeyBuffer2 = mid_hermes_ll_buffer_create(private_key2, private_key_length2);
    mid_hermes_ll_buffer_t* publicKeyBuffer2 = mid_hermes_ll_buffer_create(public_key2, public_key_length2);
    mid_hermes_ll_user_t* user2 = mid_hermes_ll_local_user_create(userBuffer2, privateKeyBuffer2, publicKeyBuffer2);

    bool isEqual = mid_hermes_ll_user_is_equal(user1, user1);
    testsuite_fail_if(!isEqual, "buffer_is_equal should be equal if users are the same");

    isEqual = mid_hermes_ll_user_is_equal(user1, user2);
    testsuite_fail_if(isEqual, "buffer_is_equal should NOT be equal if users are different");

    return TEST_SUCCESS; 
}

int mid_hermes_ll_block_cst_time_memcmp_tests () {

    uint8_t data1[3] = {0xAB, 0xAC, 0x12};
    uint8_t data2[3] = {0xAB, 0xAC, 0x12};
    uint8_t data3[3] = {0xAA, 0xAA, 0x22};
    uint8_t data4[4] = {0xAB, 0xAC, 0x12, 0x12};

    int cmp_result = cst_time_memcmp(data1, data1, sizeof(data1) / sizeof(uint8_t));
    testsuite_fail_if(cmp_result != 0, "cst_time_memcmp should be equal if data is the same");

    cmp_result = cst_time_memcmp(data1, data2, sizeof(data1) / sizeof(uint8_t));
    testsuite_fail_if(cmp_result != 0, "cst_time_memcmp should be equal if data has same content");

    cmp_result = cst_time_memcmp(data1, data3, sizeof(data1) / sizeof(uint8_t));
    testsuite_fail_if(cmp_result == 0, "cst_time_memcmp should NOT be equal if data is different");

    cmp_result = cst_time_memcmp(data1, data4, sizeof(data1) / sizeof(uint8_t));
    testsuite_fail_if(cmp_result != 0, "cst_time_memcmp might be equal if data is different, size is different, comparasion is using shorted size");

    cmp_result = cst_time_memcmp(data1, data4, sizeof(data4) / sizeof(uint8_t));
    testsuite_fail_if(cmp_result == 0, "cst_time_memcmp should NOT be equal if data is different, size is different, comparasion is using longer size");

    return TEST_SUCCESS; 
}