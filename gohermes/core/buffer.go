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
package core

/*
#cgo LDFLAGS: -lthemis -lsoter -lhermes_common -lhermes_mid_hermes -lhermes_rpc -lhermes_mid_hermes_ll -lhermes_data_store -lhermes_key_store -lhermes_credential_store -lhermes_rpc
#include "hc_core.h"
*/
import "C"
import (
	"log"
	"runtime"
	"unsafe"
)

// Buffer wrap mid_hermes_ll_buffer_t C struct
type Buffer struct {
	hermesBuffer *C.mid_hermes_ll_buffer_t
}

// finalizeHermesBuffer free memory allocated for hermesBuffer on C side
func finalizeHermesBuffer(hermesBuffer *C.mid_hermes_ll_buffer_t) {
	result := C.mid_hermes_ll_buffer_destroy(&(hermesBuffer))
	if result != C.HM_SUCCESS {
		log.Println("can't free memory for hermes buffer")
	}
}

// finalizeBuffer free memory allocated for wrapped mid_hermes_ll_buffer_t C struct
func finalizeBuffer(buffer *Buffer) {
	finalizeHermesBuffer(buffer.hermesBuffer)
}

// HermesBufferToBytes return copy of hermes buffer as byte slice
func HermesBufferToBytes(hermesBuffer *C.mid_hermes_ll_buffer_t) []byte {
	if hermesBuffer == nil {
		return nil
	}
	return C.GoBytes(unsafe.Pointer(hermesBuffer.data), C.int(hermesBuffer.length))
}

// NewFromHermesBuffer return new *Block with hermesBuffer copy
func NewFromHermesBuffer(hermesBuffer *C.mid_hermes_ll_buffer_t) (*Buffer, error) {
	hermesBufferCopy := C.mid_hermes_ll_buffer_create(hermesBuffer.data, hermesBuffer.length)
	if hermesBufferCopy == nil{
		return nil, ErrHermesBufferCreationError
	}
	buffer := &Buffer{hermesBuffer: hermesBufferCopy}
	runtime.SetFinalizer(buffer, finalizeBuffer)
	return buffer, nil
}

// BytesToHermesBuffer return copy of data wrapped into *C.mid_hermes_ll_buffer_t
func BytesToHermesBuffer(data []byte) *C.mid_hermes_ll_buffer_t {
	if data == nil {
		return C.mid_hermes_ll_buffer_create(nil, C.size_t(0))
	}
	return C.mid_hermes_ll_buffer_create((*C.uint8_t)(unsafe.Pointer(&data[0])), C.size_t(len(data)))
}

// NewHermesBuffer return new *Buffer that wraps data
func NewHermesBuffer(data []byte) (*Buffer, error) {
	if data == nil {
		return nil, ErrHermesBufferCreationError
	}
	buffer := &Buffer{
		hermesBuffer: C.mid_hermes_ll_buffer_create((*C.uint8_t)(unsafe.Pointer(&data[0])), C.size_t(len(data)))}
	if buffer.hermesBuffer == nil {
		return nil, ErrHermesBufferCreationError
	}
	runtime.SetFinalizer(buffer, finalizeBuffer)
	return buffer, nil
}

// GetData return data of buffer as byte slice
func (buffer *Buffer) GetData() []byte {
	return C.GoBytes(unsafe.Pointer(buffer.hermesBuffer.data), C.int(buffer.hermesBuffer.length))
}