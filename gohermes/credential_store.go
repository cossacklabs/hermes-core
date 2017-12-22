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

import "errors"

var ErrPublicKeyNotFound = errors.New("Public key not found")
var ErrUserCredentialsExists = errors.New("User's credentials already exists")

// CredentialStore interface of hermes-core credential store and should be implemented by go implementations
type CredentialStore interface {
	GetPublicKey(userId []byte) ([]byte, error)
	GetUsersList() ([][]byte, error)
	Close() error
}
