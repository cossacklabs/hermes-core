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

var ErrMacNotMatched = errors.New("Mac not matched")

// DataStore interface of hermes-core data store and should be implemented by go implementations
type DataStore interface {
	// AddBlock return block id or error
	AddBlock(data, meta, mac []byte) ([]byte, error)
	// AddBlockWithId return id or error
	AddBlockWithId(id, data, meta, mac []byte) ([]byte, error)
	// ReadBlock return data and meta data of block or error
	ReadBlock(id []byte) ([]byte, []byte, error)
	// ReadBlockMac return mac of block
	ReadBlockMac(id []byte) ([]byte, error)
	// UpdateBlock only if current block mac == oldMac
	UpdateBlock(id, data, meta, mac, oldMac []byte) error
	// DeleteBlock only if current block mac == oldMac
	DeleteBlock(id []byte, oldMac []byte) error
	// Close any implementation related resources
	Close() error
}
