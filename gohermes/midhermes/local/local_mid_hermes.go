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
#cgo LDFLAGS: -lhermes_mid_hermes
#include <hermes/mid_hermes/mid_hermes.h>
#include "credential_store.h"
#include "data_store.h"
#include "key_store.h"
*/
import "C"
import (
	"errors"
	"runtime"
	"unsafe"
	"github.com/cossacklabs/hermes-core/gohermes"
)

type LocalMidHermes struct {
	mid_hermes *C.mid_hermes_t
	// we need pointers to stores to avoid free their memory by gc but mid_hermes already has pointers on them on C side
	credentialStore *MidHermesCredentialStore
	keyStore        *MidHermesKeyStore
	dataStore       *MidHermesDataStore
}

func finalizeLocalMidHermes(mid_hermes *LocalMidHermes) {
	mid_hermes.Close()
}

func NewLocalMidHermes(id []byte, private_key []byte, credentialStore gohermes.CredentialStore, keyStore gohermes.KeyStore, dataStore gohermes.DataStore) (*LocalMidHermes, error) {
	credentialWrapper, err := NewCredentialStore(credentialStore)
	if err != nil{
		return nil, err
	}
	keystoreWrapper, err := NewKeyStore(keyStore)
	if err != nil{
		return nil, err
	}
	datastoreWrapper, err := NewDataStore(dataStore)
	if err != nil{
		return nil, err
	}
	mh := &LocalMidHermes{credentialStore: credentialWrapper, keyStore: keystoreWrapper, dataStore: datastoreWrapper}
	mh.mid_hermes = C.mid_hermes_create_with_services(
		(*C.uint8_t)(unsafe.Pointer(&id[0])), C.size_t(len(id)),
		(*C.uint8_t)(unsafe.Pointer(&private_key[0])), C.size_t(len(private_key)),
		(*C.hermes_key_store_t)(unsafe.Pointer(keystoreWrapper.GetHermesKeyStore())),
		(*C.hermes_data_store_t)(unsafe.Pointer(datastoreWrapper.GetHermesDataStore())),
		(*C.hermes_credential_store_t)(unsafe.Pointer(credentialWrapper.GetHermesCredentialStore())))
	if nil == mh.mid_hermes {
		return mh, errors.New("LocalMidHermes object creation error")
	}
	runtime.SetFinalizer(mh, finalizeLocalMidHermes)
	return mh, nil
}

func (mh *LocalMidHermes) Close() error {
	// TODO: use multierror like https://github.com/hashicorp/go-multierror
	C.mid_hermes_destroy(&(mh.mid_hermes))
	var err error = nil
	err = mh.credentialStore.Close()
	if kerr := mh.keyStore.Close(); kerr != nil && err == nil {
		err = kerr
	}
	if derr := mh.dataStore.Close(); derr != nil && err == nil {
		err = derr
	}
	return err
}

func (mh *LocalMidHermes) GetMidHermes() unsafe.Pointer {
	return unsafe.Pointer(mh.mid_hermes)
}
