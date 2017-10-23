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
#include "transport.h"
#include <string.h>
*/
import "C"

import (
	"crypto/rand"
	"encoding/binary"
	"errors"
	"runtime"
	"unsafe"
)

func pseudo_uuid() (uint64, error) {

	b := make([]byte, 8)
	_, err := rand.Read(b)
	if err != nil {
		return 0, err
	}
	return binary.BigEndian.Uint64(b), nil
}

type HermesTransport interface {
	Send(buf []byte) error
	Recv(buf []byte) error
}

var transport_map = make(map[uint64]HermesTransport)

//export go_transport_send
func go_transport_send(transport C.uint64_t, buf unsafe.Pointer, buf_length C.size_t) C.int {
	p := transport_map[uint64(transport)]
	b := (*[1 << 30]byte)(buf)[0:buf_length] //convert {buf, buf_length} to []byte
	err := p.Send(b)
	if nil != err {
		return 1
	}
	return 0
}

//export go_transport_recv
func go_transport_recv(transport C.uint64_t, buf unsafe.Pointer, buf_length C.size_t) C.int {
	p := transport_map[uint64(transport)]
	b := (*[1 << 30]byte)(buf)[0:buf_length]
	err := p.Recv(b)
	if nil != err {
		return 1
	}
	return 0
}

type MidHermes struct {
	credential_store_transport_ref, data_store_transport_ref, key_store_transport_ref uint64
	credential_store_transport, data_store_transport, key_store_transport             *C.hm_rpc_transport_t
	mid_hermes                                                                        *C.mid_hermes_t
}

func MidHermes_finalize(mid_hermes *MidHermes) {
	mid_hermes.Close()
}

func NewMidHermes(
	id []byte,
	private_key []byte,
	credential_store_transport HermesTransport,
	data_store_transport HermesTransport,
	key_store_transport HermesTransport) (*MidHermes, error) {
	credential_store_transport_ref, err := pseudo_uuid()
	if nil != err {
		return nil, err
	}
	data_store_transport_ref, err := pseudo_uuid()
	if nil != err {
		return nil, err
	}
	key_store_transport_ref, err := pseudo_uuid()
	if nil != err {
		return nil, err
	}
	transport_map[credential_store_transport_ref] = credential_store_transport
	transport_map[data_store_transport_ref] = data_store_transport
	transport_map[key_store_transport_ref] = key_store_transport
	mh := &MidHermes{
		credential_store_transport_ref: credential_store_transport_ref,
		data_store_transport_ref:       data_store_transport_ref,
		key_store_transport_ref:        key_store_transport_ref,
		credential_store_transport:     C.transport_create(C.uint64_t(credential_store_transport_ref)),
		data_store_transport:           C.transport_create(C.uint64_t(data_store_transport_ref)),
		key_store_transport:            C.transport_create(C.uint64_t(key_store_transport_ref))}
	mh.mid_hermes = C.mid_hermes_create(
		(*C.uint8_t)(unsafe.Pointer(&id[0])), C.size_t(len(id)),
		(*C.uint8_t)(unsafe.Pointer(&private_key[0])), C.size_t(len(private_key)),
		mh.credential_store_transport,
		mh.data_store_transport,
		mh.key_store_transport)
	if nil == mh.mid_hermes {
		return nil, errors.New("MidHermes object creation error")
	}
	runtime.SetFinalizer(mh, MidHermes_finalize)
	return mh, nil
}

func (mh *MidHermes) Close() error {
	C.mid_hermes_destroy(&(mh.mid_hermes))
	C.transport_destroy(&(mh.credential_store_transport))
	C.transport_destroy(&(mh.data_store_transport))
	C.transport_destroy(&(mh.key_store_transport))
	return nil
}

func (mh *MidHermes) AddBlock(id []byte, data []byte, meta []byte) error {
	id_ptr := (*C.uint8_t)(C.malloc(C.size_t(len(id))))
	id_length := C.size_t(len(id))
	C.memcpy(unsafe.Pointer(id_ptr), unsafe.Pointer(&id[0]), id_length)
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
