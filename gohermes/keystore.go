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
	"errors"
)

const (
	READ_ACCESS  = 0
	WRITE_ACCESS = 1
)

var ErrInvalidUserId = errors.New("invalid user id")
var ErrInvalidDocumentId = errors.New("invalid document id")
var ErrIncorrectRightsValue = errors.New("used incorrect rights value")
var ErrKeyStoreGet = errors.New("get command - Failed")
var ErrKeyStoreDelete = errors.New("delete command - Failed")
var ErrKeyStoreAdd = errors.New("add command - Failed")
var ErrKeyStoreGetIndexedRights = errors.New("getIndexedRights command - Failed")

// KeyStore interface of hermes-core key store and should be implemented by go implementations
type KeyStore interface {
	Get(docId, userId []byte, isUpdate int) ([]byte, []byte, error) // key, ownerId, error
	Add(docId, userId, key, ownerId []byte, isUpdate int) error
	Delete(docId, userId []byte, isUpdate int) error
	GetIndexedRights(blockId []byte, index int) ([]byte, int, error)
	Close() error
}
