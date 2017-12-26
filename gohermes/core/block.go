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

/*
#cgo LDFLAGS: -lthemis -lsoter -lhermes_common -lhermes_mid_hermes -lhermes_rpc -lhermes_mid_hermes_ll -lhermes_data_store -lhermes_key_store -lhermes_credential_store -lhermes_rpc
#include "hc_core.h"
*/
import "C"
import (
	"bytes"
	"encoding/gob"
	"log"
	"runtime"
)

// Block wrap C struct of hermes block
type Block struct {
	hermesBlock *C.mid_hermes_ll_block_t
}

// finalizeBlock free memory allocated for C struct of hermes block
func finalizeBlock(block *Block) {
	result := C.mid_hermes_ll_block_destroy(&(block.hermesBlock))
	if result != C.HM_SUCCESS {
		log.Println("can't free memory for hermes user")
	}
}

// block_ used as temp struct to serializing data+mac with encoding/gob
type block_ struct {
	Data   []byte
	Mac    []byte
	OldMac []byte
}

// PackBlock return encrypted data + mac as []byte
func PackBlock(block *Block) ([]byte, error) {
	blockSize := block.GetEncryptedDataLength() + block.GetMacLength()
	buffer := bytes.NewBuffer(make([]byte, 0, blockSize))
	encoder := gob.NewEncoder(buffer)
	err := encoder.Encode(&block_{Data: block.GetEncryptedData(), Mac: block.GetMac(), OldMac: block.GetOldMac()})
	if err != nil {
		return nil, err
	}
	return buffer.Bytes(), nil
}

// UnpackBlock get blob of data and split to (data, mac, oldMac, error) of block
func UnpackBlock(data []byte) ([]byte, []byte, []byte, error) {
	tempBlock := &block_{}
	decoder := gob.NewDecoder(bytes.NewReader(data))
	err := decoder.Decode(tempBlock)
	return tempBlock.Data, tempBlock.Mac, tempBlock.OldMac, err
}

// NewBlockFromData return new Block that generate new tokens, encrypt rawData and contain rawData, encryptedData and
// new tokens
func NewBlockFromData(user *User, id, rawData, meta []byte) (*Block, error) {
	block := &Block{}
	hermesUser, err := user.GetHermesUser()
	if err != nil {
		return nil, err
	}
	block.hermesBlock = C.mid_hermes_ll_block_create(
		hermesUser, BytesToHermesBuffer(id), BytesToHermesBuffer(rawData), BytesToHermesBuffer(meta),
		nil, nil)
	if block.hermesBlock == nil {
		return nil, ErrHermesBlockCreation
	}
	runtime.SetFinalizer(block, finalizeBlock)
	return block, nil
}

// NewBlockFromDataWithTokens return block and encrypt data using readToken and writeToken
func NewBlockFromDataWithTokens(user *User, id, rawData, meta []byte, readToken, writeToken *Token) (*Block, error) {
	hermesUser, err := user.GetHermesUser()
	if err != nil {
		return nil, err
	}
	hermesReadToken, err := readToken.GetHermesToken()
	if err != nil {
		C.mid_hermes_ll_user_destroy(&(hermesUser))
		return nil, err
	}
	hermesWriteToken, err := writeToken.GetHermesToken()
	if err != nil {
		C.mid_hermes_ll_user_destroy(&(hermesUser))
		C.mid_hermes_ll_token_destroy(&(hermesReadToken))
		return nil, err
	}
	block := &Block{hermesBlock: C.mid_hermes_ll_block_create_new(
		hermesUser, BytesToHermesBuffer(id), BytesToHermesBuffer(rawData), BytesToHermesBuffer(meta),
		hermesReadToken, hermesWriteToken)}
	if block.hermesBlock == nil {
		return nil, ErrHermesBlockCreation
	}
	runtime.SetFinalizer(block, finalizeBlock)
	return block, nil
}

// NewBlockFromEncryptedData return block and decrypt encrypted data using readToken and writeToken
func NewBlockFromEncryptedData(user *User, id, encryptedData, meta []byte, readToken, writeToken *Token) (*Block, error) {
	if user == nil || readToken == nil {
		return nil, ErrHermesBlockCreation
	}
	hermesUser, err := user.GetHermesUser()
	if err != nil {
		return nil, err
	}
	hermesReadToken, err := readToken.GetHermesToken()
	if err != nil {
		C.mid_hermes_ll_user_destroy(&(hermesUser))
		return nil, err
	}
	var block *Block
	if writeToken == nil || writeToken.hermesToken == nil {
		block = &Block{hermesBlock: C.mid_hermes_ll_block_create(
			hermesUser, BytesToHermesBuffer(id), BytesToHermesBuffer(encryptedData), BytesToHermesBuffer(meta),
			hermesReadToken, nil)}
	} else {
		hermesWriteToken, err := writeToken.GetHermesToken()
		if err != nil {
			C.mid_hermes_ll_user_destroy(&(hermesUser))
			C.mid_hermes_ll_token_destroy(&(hermesReadToken))
			return nil, err
		}
		block = &Block{hermesBlock: C.mid_hermes_ll_block_create(
			hermesUser, BytesToHermesBuffer(id), BytesToHermesBuffer(encryptedData), BytesToHermesBuffer(meta),
			hermesReadToken, hermesWriteToken)}
	}
	if block.hermesBlock == nil {
		return nil, ErrHermesBlockCreation
	}
	runtime.SetFinalizer(block, finalizeBlock)
	return block, nil
}

// SetOldMac set oldMac to block
func (block *Block) SetOldMac(oldMac []byte) {
	block.hermesBlock.old_mac = BytesToHermesBuffer(oldMac)
}

// GetOldMac return block's oldMac or nil
func (block *Block) GetOldMac() []byte {
	return HermesBufferToBytes(block.hermesBlock.old_mac)
}

// GetId return id of block
func (block *Block) GetId() []byte {
	return HermesBufferToBytes(block.hermesBlock.id)
}

// GetData return decrypted data of block
func (block *Block) GetData() []byte {
	return HermesBufferToBytes(block.hermesBlock.data)
}

// GetEncryptedDataLength return length of decrypted data
func (block *Block) GetEncryptedDataLength() int {
	return int(block.hermesBlock.block.length)
}

// GetEncryptedData return encrypted data of block
func (block *Block) GetEncryptedData() []byte {
	return HermesBufferToBytes(block.hermesBlock.block)
}

// GetMacLength return mac length
func (block *Block) GetMacLength() int {
	return int(block.hermesBlock.mac.length)
}

// GetMeta return meta of block
func (block *Block) GetMeta() []byte {
	return HermesBufferToBytes(block.hermesBlock.meta)
}

// GetMac return mac of block
func (block *Block) GetMac() []byte {
	return HermesBufferToBytes(block.hermesBlock.mac)
}

// GetUser return user of block
func (block *Block) GetUser() (*User, error) {
	return NewUserFromHermesUser(block.hermesBlock.user)
}

// GetReadToken return read token that was generated after creation new block or after initialization with read token
func (block *Block) GetReadToken() (*Token, error) {
	return NewTokenFromHermesToken(block.hermesBlock.rtoken)
}

// GetWriteToken return write token that was generated after creation new block or after initialization with write token
func (block *Block) GetWriteToken() (*Token, error) {
	return NewTokenFromHermesToken(block.hermesBlock.wtoken)
}

// HasWriteToken return true if block has related write token
func (block *Block) HasWriteToken() bool {
	return block.hermesBlock.wtoken != nil
}

// Update update data and meta in block
func (block *Block) Update(data, meta []byte) error {
	if data == nil || meta == nil {
		return ErrHermesBlockUpdate
	}
	result := C.mid_hermes_ll_block_update(block.hermesBlock, BytesToHermesBuffer(data), BytesToHermesBuffer(meta))
	if result == nil {
		return ErrHermesBlockUpdate
	}
	block.hermesBlock = result
	return nil
}
