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
package gohermes

import (
	"testing"
	"bytes"
)

// GeneralKeystoreTest test keystore implementation on standard behaviour and can be used by custom implementations
// for testing
func GeneralKeystoreTest(store KeyStore, t *testing.T){
	key, ownerId, err := store.Get([]byte("some doc"), []byte("some user"), 1)
	if key != nil || ownerId != nil || err != ErrKeyStoreGet {
		t.Fatal("Unexpected keystore get")
	}

	err = store.Delete([]byte("some doc"), []byte("some user"), 1)
	if err != ErrKeyStoreDelete {
		t.Fatal("Unexpected keystore delete")
	}

	testReadKey := []byte("test read key")
	testWriteKey := []byte("test write key")
	testOwner := []byte("test owner key")
	testDocId := []byte("doc id")
	testReadWriteUser := []byte("user id")

	err = store.Add(testDocId, testReadWriteUser, testReadKey, testOwner, READ_ACCESS)
	if err != nil {
		t.Fatal(err)
	}

	key, ownerId, err = store.Get(testDocId, testReadWriteUser, WRITE_ACCESS)
	if key != nil || ownerId != nil || err != ErrKeyStoreGet {
		t.Fatal("Unexpected keystore get")
	}
	err = store.Add(testDocId, testReadWriteUser, testWriteKey, testOwner, WRITE_ACCESS)
	if err != nil {
		t.Fatal(err)
	}

	key, ownerId, err = store.Get(testDocId, testReadWriteUser, READ_ACCESS)
	if key == nil || ownerId == nil || err != nil {
		t.Fatal("Unexpected keystore get error")
	}
	if !bytes.Equal(key, testReadKey) {
		t.Fatal("Keys is not equal")
	}
	if !bytes.Equal(ownerId, testOwner) {
		t.Fatal("Keys is not equal")
	}

	key, ownerId, err = store.Get(testDocId, testReadWriteUser, WRITE_ACCESS)
	if key == nil || ownerId == nil || err != nil {
		t.Fatal("Unexpected keystore get")
	}
	if !bytes.Equal(key, testWriteKey) {
		t.Fatal("Keys is not equal")
	}
	if !bytes.Equal(ownerId, testOwner) {
		t.Fatal("Keys is not equal")
	}

	testReadOnlyUser := []byte("user id2")
	err = store.Add(testDocId, testReadOnlyUser, testReadKey, testOwner, READ_ACCESS)
	if err != nil {
		t.Fatal(err)
	}

	userId, rights, err := store.GetIndexedRights(testDocId, 0)
	if err != nil{
		t.Fatal(err)
	}
	if !bytes.Equal(testReadWriteUser, userId){
		t.Fatal("incorrect user")
	}
	if rights != WRITE_ACCESS{
		t.Fatal("incorrect user's rights")
	}

	userId, rights, err = store.GetIndexedRights(testDocId, 1)
	if err != nil{
		t.Fatal(err)
	}
	if !bytes.Equal(testReadOnlyUser, userId){
		t.Fatal("incorrect user")
	}
	if rights == WRITE_ACCESS{
		t.Fatal("incorrect user's rights")
	}

	err = store.Delete(testDocId, testReadWriteUser, WRITE_ACCESS)
	if err != nil {
		t.Fatal("Unexpected keystore delete")
	}

	err = store.Delete(testDocId, testReadWriteUser, READ_ACCESS)
	if err != nil {
		t.Fatal("Unexpected keystore delete")
	}

	key, ownerId, err = store.Get(testDocId, testReadWriteUser, READ_ACCESS)
	if key != nil || ownerId != nil || err != ErrKeyStoreGet {
		t.Fatal("Unexpected keystore get")
	}
}