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

// #include <string.h>
// #include <stdint.h>
import "C"
import (
	"errors"
	"reflect"
	"unsafe"
)

var ErrCAlloc = errors.New("can't allocate memory")

//CArrayToSlice return buffer as byte slice with len/cap equals to bufferLength
func CArrayToSlice(buffer unsafe.Pointer, bufferLength int) []byte {
	sliceHeader := reflect.SliceHeader{uintptr(buffer), int(bufferLength), bufferLength}
	data := *(*[]byte)(unsafe.Pointer(&sliceHeader))
	return data
}

// SetOutBufferPointerFromByteSlice allocate memory, set pointer of memory into outBuffer, copy value to outBuffer and set size
// of memory to outBufferLength
func SetOutBufferPointerFromByteSlice(value []byte, outBuffer, outBufferLength unsafe.Pointer) error {
	if value == nil {
		*(**C.uint8_t)(outBuffer) = nil
		*(*C.size_t)(outBufferLength) = 0
		return nil
	}
	*(**C.uint8_t)(outBuffer) = (*C.uint8_t)(C.malloc(C.size_t(len(value))))
	if *(**C.uint8_t)(outBuffer) == nil {
		return ErrCAlloc
	}
	*(*C.size_t)(outBufferLength) = C.size_t(len(value))
	C.memcpy(unsafe.Pointer(*(**C.uint8_t)(outBuffer)), unsafe.Pointer(&value[0]), (C.size_t)(len(value)))
	return nil
}
