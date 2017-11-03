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

import "C"
import (
	"bytes"
	"testing"
	"unsafe"
)

func testCArrayToSlice(t *testing.T) {
	testData := []byte("some data")
	cTestData := unsafe.Pointer(&testData[0])

	resultData := CArrayToSlice(cTestData, len(testData))
	if !bytes.Equal(resultData, testData) {
		t.Fatal("result not equal to ")
	}

	// test that both slices points to the same memory
	resultData[1] = 'b'
	if testData[1] != 'b' && !bytes.Equal(resultData, testData) {
		t.Fatal("result not equal to ")
	}
}
