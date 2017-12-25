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
#cgo LDFLAGS: -lhermes_mid_hermes -lhermes_mid_hermes_ll -lhermes_credential_store -lhermes_data_store -lhermes_key_store -lhermes_rpc -lhermes_common -lthemis -lsoter -lhermes_secure_transport
#include "transport.h"
*/
import "C"
import (
	"errors"
	"log"
	"runtime"
	"unsafe"

	"github.com/cossacklabs/hermes-core/gohermes"
)

type Transport interface {
	Write([]byte) error
	Read([]byte) error
	Close() error
}

type HermesTransport struct {
	hermesTransport         *C.hm_rpc_transport_t
	secureHermesTransport   *C.hm_rpc_transport_t
	transportImplementation Transport
}

func finalizeTransport(transport *HermesTransport) {
	if transport.secureHermesTransport != nil {
		C.destroy_rpc_secure_transport(&(transport.secureHermesTransport))
		transport.secureHermesTransport = nil
	}
	if transport.hermesTransport != nil {
		C.free(unsafe.Pointer(transport.hermesTransport))
		transport.hermesTransport = nil
	}
}

// NewTransport return new HermesTransport that wraps transport into hm_rpc_transport_t
func NewTransport(transport Transport) (*HermesTransport, error) {
	testTransport := &HermesTransport{transportImplementation: transport}
	testTransport.hermesTransport = C.create_transport(&testTransport.hermesTransport)
	if testTransport.hermesTransport == nil {
		return nil, errors.New("can't allocate memory for hm_rpc_transport_t struct")
	}

	runtime.SetFinalizer(testTransport, finalizeTransport)
	return testTransport, nil
}

//NewSecureTransport return *HermesTransport that wrap transport into hm_rpc_transport_t and secure session using hermes-core
func NewSecureTransport(userId, privateKey, publicKeyId, publicKey []byte, transport Transport, isServer bool) (*HermesTransport, error) {
	testTransport := &HermesTransport{transportImplementation: transport}
	testTransport.hermesTransport = C.create_transport(&testTransport.hermesTransport)
	if testTransport.hermesTransport == nil {
		return nil, errors.New("can't allocate memory for hm_rpc_transport_t struct")
	}
	testTransport.secureHermesTransport = C.create_secure_transport(
		(*C.uint8_t)(unsafe.Pointer(&userId[0])), C.size_t(len(userId)),
		(*C.uint8_t)(unsafe.Pointer(&privateKey[0])), C.size_t(len(privateKey)),
		(*C.uint8_t)(unsafe.Pointer(&publicKey[0])), C.size_t(len(publicKey)),
		(*C.uint8_t)(unsafe.Pointer(&publicKeyId[0])), C.size_t(len(publicKeyId)),
		testTransport.hermesTransport, (C.bool)(isServer))
	if testTransport.secureHermesTransport == nil {
		finalizeTransport(testTransport)
		return nil, errors.New("can't create secure transport")
	}
	runtime.SetFinalizer(testTransport, finalizeTransport)
	return testTransport, nil
}

// GetHermesTransport return C hermes struct hm_rpc_transport_t that wrapped
func (transport *HermesTransport) GetHermesTransport() *C.hm_rpc_transport_t {
	if transport.secureHermesTransport == nil {
		return transport.hermesTransport
	}
	return transport.secureHermesTransport
}

func (transport *HermesTransport) Close() error {
	return transport.transportImplementation.Close()
}

//export transport_send
func transport_send(transportPtr, buf unsafe.Pointer, bufLength C.size_t) C.uint32_t {
	var transport *HermesTransport
	transport = (*HermesTransport)(unsafe.Pointer(uintptr(transportPtr) - unsafe.Offsetof(transport.hermesTransport)))
	data := gohermes.CArrayToSlice(buf, int(bufLength))
	err := transport.transportImplementation.Write(data)
	if nil != err {
		log.Printf("error - %v\n", err)
		return 1
	}
	return 0
}

//export transport_recv
func transport_recv(transportPtr, buf unsafe.Pointer, bufLength C.size_t) C.uint32_t {
	var transport *HermesTransport
	transport = (*HermesTransport)(unsafe.Pointer(uintptr(transportPtr) - unsafe.Offsetof(transport.hermesTransport)))
	data := gohermes.CArrayToSlice(buf, int(bufLength))
	err := transport.transportImplementation.Read(data)
	if nil != err {
		log.Printf("error - %v\n", err)
		return 1
	}
	return 0
}
