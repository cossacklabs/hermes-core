//
// Copyright (c) 2017 Cossack Labs Limited
//
// This file is a part of Hermes-core.
//
// Hermes-core is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hermes-core is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with Hermes-core.  If not, see <http://www.gnu.org/licenses/>.
//
//

package gohermes

/*
#cgo LDFLAGS: -lhermes_mid_hermes -lhermes_mid_hermes_ll -lhermes_credential_store -lhermes_data_store -lhermes_key_store -lhermes_rpc -lhermes_common -lthemis -lsoter
#include <hermes/mid_hermes/mid_hermes.h>
#include <string.h>
*/
import "C"

import (
	"errors"
	"runtime"
	"unsafe"
)

type MidHermes struct {
	mid_hermes                 *C.mid_hermes_t
	credential_store_transport *HermesTransport
	key_store_transport        *HermesTransport
	data_store_transport       *HermesTransport
}

func MidHermes_finalize(mid_hermes *MidHermes) {
	mid_hermes.Close()
}

func NewMidHermes(
	id []byte, private_key []byte,
	credential_store_transport *HermesTransport,
	data_store_transport *HermesTransport,
	key_store_transport *HermesTransport) (*MidHermes, error) {

	mh := &MidHermes{
		credential_store_transport: credential_store_transport,
		data_store_transport:       data_store_transport,
		key_store_transport:        key_store_transport}
	mh.mid_hermes = C.mid_hermes_create(
		(*C.uint8_t)(unsafe.Pointer(&id[0])), C.size_t(len(id)),
		(*C.uint8_t)(unsafe.Pointer(&private_key[0])), C.size_t(len(private_key)),
		mh.key_store_transport.GetHermesTransport(),
		mh.data_store_transport.GetHermesTransport(),
		mh.credential_store_transport.GetHermesTransport())
	if nil == mh.mid_hermes {
		return nil, errors.New("MidHermes object creation error")
	}
	runtime.SetFinalizer(mh, MidHermes_finalize)
	return mh, nil
}

func (mh *MidHermes) Close() error {
	C.mid_hermes_destroy(&(mh.mid_hermes))
	return nil
}

func (mh *MidHermes) AddBlock(id []byte, data []byte, meta []byte) error {
	var id_ptr *C.uint8_t = nil
	id_length := C.size_t(len(id))
	if len(id) > 0 {
		id_ptr = (*C.uint8_t)(C.malloc(C.size_t(len(id))))
		C.memcpy(unsafe.Pointer(id_ptr), unsafe.Pointer(&id[0]), id_length)
	}
	if 0 != C.mid_hermes_create_block(
		mh.mid_hermes,
		&id_ptr, &id_length,
		(*C.uint8_t)(unsafe.Pointer(&data[0])), C.size_t(len(data)),
		(*C.uint8_t)(unsafe.Pointer(&meta[0])), C.size_t(len(meta))) {
		C.free(unsafe.Pointer(id_ptr))
		return errors.New("MidHermes create block error")
	}
	C.free(unsafe.Pointer(id_ptr))
	return nil
}

func (mh *MidHermes) ReadBlock(id []byte) ([]byte, []byte, error) {
	var block, meta *C.uint8_t
	var block_length, meta_length C.size_t
	if 0 != C.mid_hermes_read_block(
		mh.mid_hermes,
		(*C.uint8_t)(unsafe.Pointer(&id[0])), C.size_t(len(id)),
		&block, &block_length,
		&meta, &meta_length) {
		return nil, nil, errors.New("MidHermes read block error")
	}

	return (*[1 << 30]byte)(unsafe.Pointer(block))[0:block_length], (*[1 << 30]byte)(unsafe.Pointer(meta))[0:meta_length], nil
}

func (mh *MidHermes) UpdateBlock(id []byte, data []byte, meta []byte) error {
	if 0 != C.mid_hermes_update_block(
		mh.mid_hermes,
		(*C.uint8_t)(unsafe.Pointer(&id[0])), C.size_t(len(id)),
		(*C.uint8_t)(unsafe.Pointer(&data[0])), C.size_t(len(data)),
		(*C.uint8_t)(unsafe.Pointer(&meta[0])), C.size_t(len(meta))) {
		return errors.New("MidHermes update block error")
	}
	return nil
}

func (mh *MidHermes) DeleteBlock(id []byte) error {
	if 0 != C.mid_hermes_delete_block(
		mh.mid_hermes,
		(*C.uint8_t)(unsafe.Pointer(&id[0])), C.size_t(len(id))) {
		return errors.New("MidHermes delete block error")
	}
	return nil
}

func (mh *MidHermes) RotateBlock(id []byte) error {
	if 0 != C.mid_hermes_rotate_block(
		mh.mid_hermes,
		(*C.uint8_t)(unsafe.Pointer(&id[0])), C.size_t(len(id))) {
		return errors.New("MidHermes rotate block error")
	}
	return nil
}

func (mh *MidHermes) GrantReadAccess(id []byte, for_user []byte) error {
	if 0 != C.mid_hermes_grant_read_access(
		mh.mid_hermes,
		(*C.uint8_t)(unsafe.Pointer(&id[0])), C.size_t(len(id)),
		(*C.uint8_t)(unsafe.Pointer(&for_user[0])), C.size_t(len(for_user))) {
		return errors.New("MidHermes grant read access error")
	}
	return nil
}
func (mh *MidHermes) GrantUpdateAccess(id []byte, for_user []byte) error {
	if 0 != C.mid_hermes_grant_update_access(
		mh.mid_hermes,
		(*C.uint8_t)(unsafe.Pointer(&id[0])), C.size_t(len(id)),
		(*C.uint8_t)(unsafe.Pointer(&for_user[0])), C.size_t(len(for_user))) {
		return errors.New("MidHermes grant update access error")
	}
	return nil
}

func (mh *MidHermes) RevokeReadAccess(id []byte, for_user []byte) error {
	if 0 != C.mid_hermes_deny_read_access(
		mh.mid_hermes,
		(*C.uint8_t)(unsafe.Pointer(&id[0])), C.size_t(len(id)),
		(*C.uint8_t)(unsafe.Pointer(&for_user[0])), C.size_t(len(for_user))) {
		return errors.New("MidHermes revoke read access error")
	}
	return nil
}

func (mh *MidHermes) RevokeUpdateAccess(id []byte, for_user []byte) error {
	if 0 != C.mid_hermes_deny_update_access(
		mh.mid_hermes,
		(*C.uint8_t)(unsafe.Pointer(&id[0])), C.size_t(len(id)),
		(*C.uint8_t)(unsafe.Pointer(&for_user[0])), C.size_t(len(for_user))) {
		return errors.New("MidHermes revoke update access error")
	}
	return nil
}
