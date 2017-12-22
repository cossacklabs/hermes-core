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
package local

/*
#include "key_store.h"
*/
import "C"
import (
	"fmt"
	"github.com/cossacklabs/hermes-core/gohermes"
	"unsafe"
)

type MidHermesKeyStore struct {
	hermesKeyStore C.hermes_key_store_t
	keyStore       gohermes.KeyStore
}

func NewKeyStore(store gohermes.KeyStore) (*MidHermesKeyStore, error) {
	keyStore := &MidHermesKeyStore{keyStore: store}
	return keyStore, nil
}

func (store *MidHermesKeyStore) GetHermesCredentialStore() *C.hermes_key_store_t {
	return &store.hermesKeyStore
}

func (store *MidHermesKeyStore) Close() error {
	return nil
}

//export hermes_key_store_get_read_token_callback
func hermes_key_store_get_read_token_callback(keyStorePtr unsafe.Pointer, userIdPtr unsafe.Pointer, userIdLen C.size_t, blockIdPtr unsafe.Pointer, blockIdLen C.size_t, tokenPtr unsafe.Pointer, tokenLenPtr unsafe.Pointer, ownerIdPtr unsafe.Pointer, ownerIdLenPtr unsafe.Pointer) C.uint32_t {
	var db *MidHermesKeyStore
	blockId := C.GoBytes(blockIdPtr, C.int(blockIdLen))
	userId := C.GoBytes(userIdPtr, C.int(userIdLen))
	db = (*MidHermesKeyStore)(unsafe.Pointer(uintptr(keyStorePtr) - unsafe.Offsetof(db.hermesKeyStore)))
	token, ownerId, err := db.keyStore.Get(blockId, userId, 0)
	if err != nil {
		fmt.Printf("Call getRtoken(blockId=%o, userId=%o) failed with error: %s\n", blockId, userId, err)
		return C.HM_FAIL
	}
	err = gohermes.SetOutBufferPointerFromByteSlice(token, tokenPtr, tokenLenPtr)
	if err != nil {
		return C.HM_FAIL
	}
	err = gohermes.SetOutBufferPointerFromByteSlice(ownerId, ownerIdPtr, ownerIdLenPtr)
	if err != nil {
		C.free(tokenPtr)
		return C.HM_FAIL
	}
	fmt.Printf("Call getRtoken(blockId=%o, userId=%o) success\n", blockId, userId)
	return C.HM_SUCCESS
}

//export hermes_key_store_get_write_token_callback
func hermes_key_store_get_write_token_callback(keyStorePtr unsafe.Pointer, userIdPtr unsafe.Pointer, userIdLen C.size_t, blockIdPtr unsafe.Pointer, blockIdLen C.size_t, tokenPtr unsafe.Pointer, tokenLenPtr unsafe.Pointer, ownerIdPtr unsafe.Pointer, ownerIdLenPtr unsafe.Pointer) C.uint32_t {
	var db *MidHermesKeyStore
	blockId := C.GoBytes(blockIdPtr, C.int(blockIdLen))
	userId := C.GoBytes(userIdPtr, C.int(userIdLen))
	db = (*MidHermesKeyStore)(unsafe.Pointer(uintptr(keyStorePtr) - unsafe.Offsetof(db.hermesKeyStore)))
	token, ownerId, err := db.keyStore.Get(blockId, userId, 1)
	if err != nil {
		fmt.Printf("Call getRtoken(blockId=%o, userId=%o) failed with error: %s\n", blockId, userId, err)
		return C.HM_FAIL
	}
	err = gohermes.SetOutBufferPointerFromByteSlice(token, tokenPtr, tokenLenPtr)
	if err != nil {
		return C.HM_FAIL
	}
	err = gohermes.SetOutBufferPointerFromByteSlice(ownerId, ownerIdPtr, ownerIdLenPtr)
	if err != nil {
		C.free(tokenPtr)
		return C.HM_FAIL
	}
	fmt.Printf("Call getRtoken(blockId=%o, userId=%o) success\n", blockId, userId)
	return C.HM_SUCCESS
}

//export hermes_key_store_set_write_token_callback
func hermes_key_store_set_write_token_callback(keyStorePtr unsafe.Pointer, userIdPtr unsafe.Pointer, userIdLen C.size_t, blockIdPtr unsafe.Pointer, blockIdLen C.size_t, tokenPtr unsafe.Pointer, tokenLen C.size_t, ownerIdPtr unsafe.Pointer, ownerIdLen C.size_t) C.uint32_t {
	var db *MidHermesKeyStore
	blockId := C.GoBytes(blockIdPtr, C.int(blockIdLen))
	userId := C.GoBytes(userIdPtr, C.int(userIdLen))
	ownerId := C.GoBytes(ownerIdPtr, C.int(ownerIdLen))
	token := C.GoBytes(tokenPtr, C.int(tokenLen))
	db = (*MidHermesKeyStore)(unsafe.Pointer(uintptr(keyStorePtr) - unsafe.Offsetof(db.hermesKeyStore)))
	var err error
	if len(token) == 0 {
		err = db.keyStore.Delete(blockId, userId, 1)
	} else {
		err = db.keyStore.Add(blockId, userId, token, ownerId, 1)
	}
	if err != nil {
		fmt.Printf("Call addWtoken(blockId=%o, userId=%o, ownerId=%o, token=%o) failed with error: %s\n", blockId, userId, ownerId, token, err)
		return C.HM_FAIL
	}
	fmt.Printf("Call addWtoken(blockId=%o, userId=%o, ownerId=%o, token=%o) success\n", blockId, userId, ownerId, token)
	return C.HM_SUCCESS
}

//export hermes_key_store_set_read_token_callback
func hermes_key_store_set_read_token_callback(keyStorePtr unsafe.Pointer, userIdPtr unsafe.Pointer, userIdLen C.size_t, blockIdPtr unsafe.Pointer, blockIdLen C.size_t, tokenPtr unsafe.Pointer, tokenLen C.size_t, ownerIdPtr unsafe.Pointer, ownerIdLen C.size_t) C.uint32_t {
	var db *MidHermesKeyStore
	blockId := C.GoBytes(blockIdPtr, C.int(blockIdLen))
	userId := C.GoBytes(userIdPtr, C.int(userIdLen))
	ownerId := C.GoBytes(ownerIdPtr, C.int(ownerIdLen))
	token := C.GoBytes(tokenPtr, C.int(tokenLen))
	db = (*MidHermesKeyStore)(unsafe.Pointer(uintptr(keyStorePtr) - unsafe.Offsetof(db.hermesKeyStore)))
	var err error
	if len(token) == 0 {
		err = db.keyStore.Delete(blockId, userId, 0)
	} else {
		err = db.keyStore.Add(blockId, userId, token, ownerId, 0)
	}
	if err != nil {
		fmt.Printf("Call addWtoken(blockId=%o, userId=%o, ownerId=%o, token=%o) failed with error: %s\n", blockId, userId, ownerId, token, err)
		return C.HM_FAIL
	}
	fmt.Printf("Call addWtoken(blockId=%o, userId=%o, ownerId=%o, token=%o) success\n", blockId, userId, ownerId, token)
	return C.HM_SUCCESS
}

//export hermes_key_store_get_indexed_rights_callback
func hermes_key_store_get_indexed_rights_callback(ksPtr unsafe.Pointer, blockIdPtr unsafe.Pointer, blockIdLen C.size_t, rightsIndex C.size_t, outUserIdPtr unsafe.Pointer, outUserIdLen unsafe.Pointer, outRightsMask unsafe.Pointer) C.uint32_t {
	var db *MidHermesKeyStore
	blockId := C.GoBytes(blockIdPtr, C.int(blockIdLen))
	index := int(rightsIndex)
	db = (*MidHermesKeyStore)(unsafe.Pointer(uintptr(ksPtr) - unsafe.Offsetof(db.hermesKeyStore)))
	userId, rightsMask, err := db.keyStore.GetIndexedRights(blockId, index)
	if err != nil {
		fmt.Printf("Call get_indexed_rights(blockId=%o, index=%v) failed with error: %s\n", blockId, index, err)
		return C.HM_FAIL
	}
	// allocate memory to out param outUserIdPtr and copy userId data from byte slice
	gohermes.SetOutBufferPointerFromByteSlice(userId, outUserIdPtr, outUserIdLen)

	// allocate memory to out param outUserIdPtr and copy userId data from byte slice
	*(*C.size_t)(outRightsMask) = C.size_t(rightsMask)
	fmt.Printf("Call get_indexed_rights(blockId=%o, index=%v) success\n", blockId, index)
	return C.HM_SUCCESS
}
