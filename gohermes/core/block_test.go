// Copyright 2016, Cossack Labs Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
package core

import (
	"bytes"
	"github.com/cossacklabs/themis/gothemis/keys"
	"testing"
)

func createTestBlock(data, meta []byte, t *testing.T) *Block {
	keypair, err := keys.New(keys.KEYTYPE_EC)
	if err != nil {
		t.Fatal(err)
	}
	user, err := NewUser([]byte("clientid"), keypair.Private.Value, keypair.Public.Value)
	if err != nil {
		t.Fatal(err)
	}
	block, err := NewBlockFromData(user, []byte("blockId"), data, meta)
	if err != nil {
		t.Fatal(err)
	}
	if block == nil {
		t.Fatal("block didn't created")
	}
	return block
}

func TestNewBlock(t *testing.T) {
	testData := []byte("somedata")
	testMeta := []byte("somemeta")
	block := createTestBlock(testData, testMeta, t)

	if !bytes.Equal(block.GetData(), testData) {
		t.Fatal("data not equal")
	}
	if bytes.Equal(block.GetData(), block.GetEncryptedData()) {
		t.Fatal("data can't be equal to encrypted data")
	}
	if !bytes.Equal(block.GetMeta(), testMeta) {
		t.Fatal("meta not equal")
	}
}

func TestNewBlockFromEncryptedData(t *testing.T) {
	testData := []byte("somedata")
	testMeta := []byte("somemeta")
	// create block with encrypted data
	block := createTestBlock(testData, testMeta, t)
	user, err := block.GetUser()
	if err != nil {
		t.Fatal(err)
	}
	readToken, err := block.GetReadToken()
	if err != nil {
		t.Fatal(err)
	}
	writeToken, err := block.GetWriteToken()
	if err != nil {
		t.Fatal(err)
	}
	block2, err := NewBlockFromEncryptedData(
		user, block.GetId(), block.GetEncryptedData(), block.GetMeta(), readToken, writeToken)
	if err != nil {
		t.Fatal(err)
	}
	if !bytes.Equal(block2.GetData(), block.GetData()) {
		t.Fatal("data not equal")
	}

	_, err = NewBlockFromEncryptedData(
		user, block.GetId(), block.GetEncryptedData(), block.GetMeta(), nil, nil)
	if err != ErrHermesBlockCreation {
		t.Fatal("expected error")
	}

	block2, err = NewBlockFromEncryptedData(
		user, block.GetId(), block.GetEncryptedData(), block.GetMeta(), readToken, nil)
	if err != nil {
		t.Fatal("expected error")
	}
	if block2 == nil {
		t.Fatal("unexpected error")
	}
}

func TestNewBlockFromDataWithTokens2(t *testing.T) {
	testData := []byte("somedata")
	testMeta := []byte("somemeta")
	// create block with encrypted data
	block := createTestBlock(testData, testMeta, t)
	user, err := block.GetUser()
	if err != nil {
		t.Fatal(err)
	}
	readToken, err := block.GetReadToken()
	if err != nil {
		t.Fatal(err)
	}
	writeToken, err := block.GetWriteToken()
	if err != nil {
		t.Fatal(err)
	}
	newBlock, err := NewBlockFromDataWithTokens(
		user, block.GetId(), testData, testMeta, readToken, writeToken)
	if err != nil {
		t.Fatal(err)
	}
	if bytes.Equal(newBlock.GetEncryptedData(), block.GetEncryptedData()) {
		t.Fatal("encrypted data of new block with same keys should not be the same")
	}
	if !bytes.Equal(newBlock.GetData(), block.GetData()) {
		t.Fatal("encrypted data of new block with same keys are not the same")
	}
	if !bytes.Equal(newBlock.GetMeta(), block.GetMeta()) {
		t.Fatal("encrypted data of new block with same keys are not the same")
	}
	if !bytes.Equal(newBlock.GetMac(), block.GetMac()) {
		t.Fatal("encrypted data of new block with same keys are not the same")
	}
}

func TestBlock_HasWriteToken(t *testing.T) {
	block := createTestBlock([]byte("data"), []byte("meta"), t)
	if !block.HasWriteToken() {
		t.Fatal("new block should have generated write token")
	}
}

func TestBlock_Update(t *testing.T) {
	block := createTestBlock([]byte("data"), []byte("meta"), t)
	if !block.HasWriteToken() {
		t.Fatal("new block should have generated write token")
	}
	newData := []byte("new data")
	newMeta := []byte("new meta")
	err := block.Update(newData, newMeta)
	if err != nil {
		t.Fatal(err)
	}

	if !bytes.Equal(block.GetData(), newData) {
		t.Fatal("data not equal")
	}
	if bytes.Equal(block.GetData(), block.GetEncryptedData()) {
		t.Fatal("data can't be equal to encrypted data")
	}
	if !bytes.Equal(block.GetMeta(), newMeta) {
		t.Fatal("meta not equal")
	}

	// update with meta nil
	err = block.Update(newData, nil)
	if err != ErrHermesBlockUpdate {
		t.Fatal(err)
	}
}

func TestBlockPackUnpack(t *testing.T) {
	// pack and then unpack block, compare that unpacked data equals to original encrypted data
	block := createTestBlock([]byte("data"), []byte("meta"), t)
	oldMac := []byte("old mac")
	block.SetOldMac(oldMac)
	packedBlock, err := PackBlock(block)
	if err != nil {
		t.Fatal(err)
	}
	unpackedData, unpackedMac, unpackedOldMac, err := UnpackBlock(packedBlock)
	if err != nil {
		t.Fatal(err)
	}
	if !bytes.Equal(unpackedData, block.GetEncryptedData()) {
		t.Fatal("unpacked data not equal to encrypted data")
	}
	if !bytes.Equal(unpackedMac, block.GetMac()) {
		t.Fatal("unpacked mac not equal to encrypted data")
	}
	if !bytes.Equal(unpackedOldMac, block.GetOldMac()) {
		t.Fatal("unpacked mac not equal to encrypted data")
	}
}

func TestBlock_SetOldMac(t *testing.T) {
	block := createTestBlock([]byte("data"), []byte("meta"), t)
	testMac := []byte("test mac")
	block.SetOldMac(testMac)
	if !bytes.Equal(testMac, block.GetOldMac()) {
		t.Fatal("old mac not equal")
	}
}
