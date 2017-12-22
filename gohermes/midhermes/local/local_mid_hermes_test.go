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

import (
	"bytes"
	"github.com/cossacklabs/hermes-core/gohermes"
	"testing"
)

var userId = []byte("user id")
var userPublicKey = []byte{0x55, 0x45, 0x43, 0x32, 0x0, 0x0, 0x0, 0x2d, 0x15, 0x9, 0xf, 0xdf, 0x2, 0xdf, 0x3c, 0x77, 0x63, 0x7c, 0xbc, 0x31, 0x55, 0x0, 0xa6, 0xc3, 0x24, 0x7b, 0x17, 0x7b, 0xf, 0x7, 0x80, 0x1e, 0x79, 0x3c, 0x8a, 0xd5, 0x7e, 0x11, 0xb, 0x5e, 0xb1, 0x1f, 0xd4, 0xdf, 0x3d}
var userPrivateKey = []byte{0x52, 0x45, 0x43, 0x32, 0x0, 0x0, 0x0, 0x2d, 0x2d, 0x2e, 0x99, 0x8e, 0x0, 0x42, 0x42, 0xe, 0x41, 0x8f, 0x4e, 0xbf, 0xa5, 0xda, 0x3a, 0x3e, 0x2d, 0x65, 0x1b, 0x7, 0xbb, 0x92, 0x5f, 0x9e, 0xbb, 0x9f, 0x38, 0x26, 0x31, 0xf3, 0x83, 0x97, 0x8c, 0x16, 0x4c, 0x1, 0xf8}

type SimpleCredentialStore struct {
}

func (store *SimpleCredentialStore) GetPublicKey(userId []byte) ([]byte, error) {
	return userPublicKey, nil
}

func (store *SimpleCredentialStore) GetUsersList() ([][]byte, error) {
	return [][]byte{userId}, nil
}

func (store *SimpleCredentialStore) Close() error {
	return nil
}

type SimpleKeyStore struct {
	writeKey []byte
	readKey  []byte
}

func (store *SimpleKeyStore) Get(docId, userId []byte, isUpdate int) ([]byte, []byte, error) { // key, ownerId, error
	if isUpdate == 1 {
		return store.writeKey, userId, nil
	} else {
		return store.readKey, userId, nil
	}
}
func (store *SimpleKeyStore) Add(docId, userId, key, ownerId []byte, isUpdate int) error {
	if isUpdate == 1 {
		store.writeKey = key
	} else {
		store.readKey = key
	}
	return nil
}
func (store *SimpleKeyStore) Delete(docId, userId []byte, isUpdate int) error {
	return nil
}
func (store *SimpleKeyStore) GetIndexedRights(blockId []byte, index int) ([]byte, int, error) {
	return nil, 0, nil
}

type SimpleDataStore struct {
	data []byte
	meta []byte
	mac  []byte
}

func (store *SimpleDataStore) AddBlock(data, meta, mac []byte) ([]byte, error) {
	store.data = data
	store.meta = meta
	store.mac = mac
	return mac, nil
}
func (store *SimpleDataStore) AddBlockWithId(id, data, meta, mac []byte) ([]byte, error) {
	store.data = data
	store.meta = meta
	store.mac = mac
	return id, nil
}
func (store *SimpleDataStore) ReadBlock(id []byte) ([]byte, []byte, error) {
	return store.data, store.meta, nil
}
func (store *SimpleDataStore) ReadBlockMac(id []byte) ([]byte, error) {
	return store.mac, nil
}
func (store *SimpleDataStore) UpdateBlock(id, data, meta, mac, oldMac []byte) error {
	if bytes.Equal(store.mac, oldMac) {
		store.data = data
		store.meta = meta
		store.mac = mac
		return nil
	}
	return gohermes.ErrMacNotMatched
}
func (store *SimpleDataStore) DeleteBlock(id []byte, oldMac []byte) error { return nil }

func (store *SimpleDataStore) Close() error { return nil }

func TestNewLocalMidHermes(t *testing.T) {
	credentialStore := &SimpleCredentialStore{}
	midhermesCredentialStore, err := NewCredentialStore(credentialStore)
	if err != nil {
		t.Fatal(err)
	}

	midhermesKeyStore, err := NewKeyStore(&SimpleKeyStore{})
	if err != nil {
		t.Fatal(err)
	}

	midhermesDataStore, err := NewDataStore(&SimpleDataStore{})
	if err != nil {
		t.Fatal(err)
	}

	hermesWrapper, err := NewLocalMidHermes(userId, userPrivateKey, midhermesCredentialStore, midhermesKeyStore, midhermesDataStore)
	if err != nil {
		t.Fatal(err)
	}
	midhermes, err := gohermes.NewMidHermes(hermesWrapper)
	if err != nil {
		t.Fatal(err)
	}
	blockId := []byte("some block id")
	err = midhermes.AddBlock(blockId, []byte("data"), []byte("yolo meta"))
	if err != nil {
		t.Fatal(err)
	}
	_, _, err = midhermes.ReadBlock([]byte("someif"))
	if err != nil {
		t.Fatal(err)
	}
	err = midhermes.GrantReadAccess(userId, []byte("userid"))
	if err != nil {
		t.Fatal(err)
	}
	err = midhermes.GrantWriteAccess([]byte("id"), []byte("userid"))
	if err != nil {
		t.Fatal(err)
	}
	err = midhermes.RevokeReadAccess([]byte("id"), []byte("userid"))
	if err != nil {
		t.Fatal(err)
	}
	err = midhermes.RevokeWriteAccess([]byte("id"), []byte("userid"))
	if err != nil {
		t.Fatal(err)
	}
	err = midhermes.UpdateBlock([]byte("id"), []byte("data"), []byte("meta"))
	if err != nil {
		t.Fatal(err)
	}
	err = midhermes.DeleteBlock([]byte("id"))
	if err != nil {
		t.Fatal(err)
	}
}
