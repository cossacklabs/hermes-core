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

package rpc

/*
#cgo LDFLAGS: -lhermes_mid_hermes -lhermes_mid_hermes_ll -lhermes_credential_store -lhermes_data_store -lhermes_key_store -lhermes_rpc -lhermes_common -lthemis -lsoter
#include <hermes/mid_hermes/mid_hermes.h>
#include <hermes/rpc/transport.h>
#include <string.h>
*/
import "C"

import (
	"errors"
	"runtime"
	"unsafe"
)

type RPCMidHermes struct {
	mid_hermes                 *C.mid_hermes_t
	credential_store_transport *HermesTransport
	key_store_transport        *HermesTransport
	data_store_transport       *HermesTransport
}

func finalizeRPCMidHermes(mid_hermes *RPCMidHermes) {
	mid_hermes.Close()
}

func NewRPCMidHermes(
	id []byte, private_key []byte,
	credential_store_transport *HermesTransport,
	data_store_transport *HermesTransport,
	key_store_transport *HermesTransport) (*RPCMidHermes, error) {

	mh := &RPCMidHermes{
		credential_store_transport: credential_store_transport,
		data_store_transport:       data_store_transport,
		key_store_transport:        key_store_transport}
	mh.mid_hermes = C.mid_hermes_create(
		(*C.uint8_t)(unsafe.Pointer(&id[0])), C.size_t(len(id)),
		(*C.uint8_t)(unsafe.Pointer(&private_key[0])), C.size_t(len(private_key)),
		(*C.hm_rpc_transport_t)(mh.key_store_transport.GetHermesTransport()),
		(*C.hm_rpc_transport_t)(mh.data_store_transport.GetHermesTransport()),
		(*C.hm_rpc_transport_t)(mh.credential_store_transport.GetHermesTransport()))
	if nil == mh.mid_hermes {
		return nil, errors.New("RPCMidHermes object creation error")
	}
	runtime.SetFinalizer(mh, finalizeRPCMidHermes)
	return mh, nil
}

// Close destroy midhermes and close all transports, return first error occurred
func (mh *RPCMidHermes) Close() error {
	// TODO: use multierror like https://github.com/hashicorp/go-multierror
	var err error = nil
	C.mid_hermes_destroy(&(mh.mid_hermes))
	err = mh.credential_store_transport.Close()
	if kerr := mh.key_store_transport.Close(); kerr != nil && err == nil {
		err = kerr
	}
	if derr := mh.data_store_transport.Close(); derr != nil && err == nil {
		err = derr
	}
	return err
}

func (mh *RPCMidHermes) GetMidHermes() unsafe.Pointer {
	return unsafe.Pointer(mh.mid_hermes)
}
