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
#include "credential_store.h"
*/
import "C"
import (
	"fmt"
	"github.com/cossacklabs/hermes-core/gohermes"
	"unsafe"
)

type MidHermesCredentialStore struct {
	hermesCredentialStore C.hermes_credential_store_t
	credentialStore       gohermes.CredentialStore
}

func NewCredentialStore(store gohermes.CredentialStore) (*MidHermesCredentialStore, error) {
	credentialStore := &MidHermesCredentialStore{credentialStore: store}
	return credentialStore, nil
}

//export hermes_credential_store_get_public_key_callback
func hermes_credential_store_get_public_key_callback(dbPtr unsafe.Pointer, idPtr unsafe.Pointer, idLength C.size_t, keyPtr unsafe.Pointer, keyLengthPtr unsafe.Pointer) C.uint32_t {
	var db *MidHermesCredentialStore
	id := C.GoBytes(idPtr, C.int(idLength))
	fmt.Printf("get public key for id %s\n", string(id))
	db = (*MidHermesCredentialStore)(unsafe.Pointer(uintptr(dbPtr) - unsafe.Offsetof(db.hermesCredentialStore)))
	pub, err := db.credentialStore.GetPublicKey(id)
	if err != nil {
		return C.HM_FAIL
	}
	err = gohermes.SetOutBufferPointerFromByteSlice(pub, keyPtr, keyLengthPtr)
	if err != nil {
		return C.HM_FAIL
	}
	return C.HM_SUCCESS
}

func (store *MidHermesCredentialStore) GetHermesCredentialStore() *C.hermes_credential_store_t {
	return &store.hermesCredentialStore
}

func (store *MidHermesCredentialStore) Close() error {
	return store.credentialStore.Close()
}
