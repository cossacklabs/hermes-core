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
#include "data_store.h"
*/
import "C"
import (
	"fmt"
	"github.com/cossacklabs/hermes-core/gohermes"
	"unsafe"
)

type MidHermesDataStore struct {
	hermesDataStore C.hermes_data_store_t
	dataStore       gohermes.DataStore
}

func NewDataStore(store gohermes.DataStore) (*MidHermesDataStore, error) {
	dataStore := &MidHermesDataStore{dataStore: store}
	return dataStore, nil
}

func (store *MidHermesDataStore) GetHermesDataStore() *C.hermes_data_store_t {
	return &store.hermesDataStore
}

func (store *MidHermesDataStore) Close() error {
	return store.dataStore.Close()
}

//export hermes_data_store_create_block
func hermes_data_store_create_block(datastorePtr unsafe.Pointer, blockPtr unsafe.Pointer, blockLen C.size_t, metaPtr unsafe.Pointer, metaLen C.size_t, macPtr unsafe.Pointer, macLen C.size_t, idPtr unsafe.Pointer, idLenPtr unsafe.Pointer) C.uint32_t {
	var db *MidHermesDataStore
	block := C.GoBytes(blockPtr, C.int(blockLen))
	meta := C.GoBytes(metaPtr, C.int(metaLen))
	mac := C.GoBytes(macPtr, C.int(macLen))
	db = (*MidHermesDataStore)(unsafe.Pointer(uintptr(datastorePtr) - unsafe.Offsetof(db.hermesDataStore)))
	id, err := db.dataStore.AddBlock(block, meta, mac)
	if err != nil {
		fmt.Printf("Call CreateBlock(block_len=%o, mac=%o) failed with error: %s\n", len(block), mac, err)
		return C.HM_FAIL
	}
	err = gohermes.SetOutBufferPointerFromByteSlice(id, idPtr, idLenPtr)
	if err != nil {
		return C.HM_FAIL
	}
	fmt.Printf("Call CreateBlock(block_len=%o, mac=%o) success\n", len(block), mac)
	return C.HM_SUCCESS
}

//export hermes_data_store_create_block_with_id
func hermes_data_store_create_block_with_id(datastorePtr unsafe.Pointer, idPtr unsafe.Pointer, idLen C.size_t, blockPtr unsafe.Pointer, blockLen C.size_t, metaPtr unsafe.Pointer, metaLen C.size_t, macPtr unsafe.Pointer, macLen C.size_t) C.uint32_t {
	var db *MidHermesDataStore
	blockId := C.GoBytes(idPtr, C.int(idLen))
	block := C.GoBytes(blockPtr, C.int(blockLen))
	meta := C.GoBytes(metaPtr, C.int(metaLen))
	mac := C.GoBytes(macPtr, C.int(macLen))
	db = (*MidHermesDataStore)(unsafe.Pointer(uintptr(datastorePtr) - unsafe.Offsetof(db.hermesDataStore)))
	_, err := db.dataStore.AddBlockWithId(blockId, block, meta, mac)
	if err != nil {
		fmt.Printf("Call CreateBlock(block_len=%o, mac=%o) failed with error: %s\n", len(block), mac, err)
		return C.HM_FAIL
	}
	fmt.Printf("Call CreateBlock(block_len=%o, mac=%o) success\n", len(block), mac)
	return C.HM_SUCCESS
}

//export hermes_data_store_read_block
func hermes_data_store_read_block(datastorePtr unsafe.Pointer, idPtr unsafe.Pointer, idLen C.size_t, blockPtr unsafe.Pointer, blockLenPtr unsafe.Pointer, metaPtr unsafe.Pointer, metaLenPtr unsafe.Pointer) C.uint32_t {
	var db *MidHermesDataStore
	id := C.GoBytes(idPtr, C.int(idLen))
	db = (*MidHermesDataStore)(unsafe.Pointer(uintptr(datastorePtr) - unsafe.Offsetof(db.hermesDataStore)))
	block, meta, err := db.dataStore.ReadBlock(id)
	if err != nil {
		fmt.Printf("Call ReadBlock(id=%o) failed with error: %s\n", id, err)
		return C.HM_FAIL
	}
	if err = gohermes.SetOutBufferPointerFromByteSlice(block, blockPtr, blockLenPtr); err != nil {
		return C.HM_FAIL
	}
	if err = gohermes.SetOutBufferPointerFromByteSlice(meta, metaPtr, metaLenPtr); err != nil {
		C.free(blockPtr)
		return C.HM_FAIL
	}

	fmt.Printf("Call ReadBlock(id=%o) success\n", id)
	return C.HM_SUCCESS
}

//export hermes_data_store_read_block_mac
func hermes_data_store_read_block_mac(datastorePtr unsafe.Pointer, idPtr unsafe.Pointer, idLen C.size_t, macPtr unsafe.Pointer, macLenPtr unsafe.Pointer) C.uint32_t {
	var db *MidHermesDataStore
	id := C.GoBytes(idPtr, C.int(idLen))
	db = (*MidHermesDataStore)(unsafe.Pointer(uintptr(datastorePtr) - unsafe.Offsetof(db.hermesDataStore)))
	mac, err := db.dataStore.ReadBlockMac(id)
	if err != nil {
		fmt.Printf("Call ReadBlockMac(id=%o) failed with error: %s\n", id, err)
		return C.HM_FAIL
	}
	err = gohermes.SetOutBufferPointerFromByteSlice(mac, macPtr, macLenPtr)
	if err != nil {
		return C.HM_FAIL
	}
	fmt.Printf("Call ReadBlockMac(id=%o) success\n", id)
	return C.HM_SUCCESS
}

//export hermes_data_store_update_block
func hermes_data_store_update_block(datastorePtr unsafe.Pointer, idPtr unsafe.Pointer, idLen C.size_t, blockPtr unsafe.Pointer, blockLen C.size_t, metaPtr unsafe.Pointer, metaLen C.size_t, macPtr unsafe.Pointer, macLen C.size_t, oldMacPtr unsafe.Pointer, oldMacLen C.size_t) C.uint32_t {
	var db *MidHermesDataStore
	id := C.GoBytes(idPtr, C.int(idLen))
	block := C.GoBytes(blockPtr, C.int(blockLen))
	meta := C.GoBytes(metaPtr, C.int(metaLen))
	mac := C.GoBytes(macPtr, C.int(macLen))
	oldMac := C.GoBytes(oldMacPtr, C.int(oldMacLen))
	db = (*MidHermesDataStore)(unsafe.Pointer(uintptr(datastorePtr) - unsafe.Offsetof(db.hermesDataStore)))
	err := db.dataStore.UpdateBlock(id, block, meta, mac, oldMac)
	if err != nil {
		fmt.Printf("Call UpdateBlock(id=%o) failed with error: %s\n", id, err)
		return C.HM_FAIL
	}
	fmt.Printf("Call UpdateBlock(id=%o) success\n", id)
	return C.HM_SUCCESS
}

//export hermes_data_store_delete_block
func hermes_data_store_delete_block(datastorePtr unsafe.Pointer, idPtr unsafe.Pointer, idLen C.size_t, oldMacPtr unsafe.Pointer, oldMacLen C.size_t) C.uint32_t {
	var db *MidHermesDataStore
	id := C.GoBytes(idPtr, C.int(idLen))
	oldMac := C.GoBytes(oldMacPtr, C.int(oldMacLen))
	db = (*MidHermesDataStore)(unsafe.Pointer(uintptr(datastorePtr) - unsafe.Offsetof(db.hermesDataStore)))
	err := db.dataStore.DeleteBlock(id, oldMac)
	if err != nil {
		fmt.Printf("Call DeleteBlock(id=%o) failed with error: %s\n", id, err)
		return C.HM_FAIL
	}
	fmt.Printf("Call DeleteBlock(id=%o) success\n", id)
	return C.HM_SUCCESS
}
