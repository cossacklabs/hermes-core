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

import (
	"bytes"
	"testing"
)

func TestBuffer(t *testing.T) {
	testData := []byte("aloha")
	buffer, err := NewHermesBuffer(testData)
	if err != nil {
		t.Fatal(err)
	}
	buffer2, err := NewFromHermesBuffer(buffer.hermesBuffer)
	if err != nil{
		t.Fatal(err)
	}
	if !bytes.Equal(buffer.GetData(), buffer2.GetData()) {
		t.Fatal("Buffers not equal")
	}
	data := HermesBufferToBytes(buffer.hermesBuffer)
	if !bytes.Equal(buffer.GetData(), data) {
		t.Fatal("Buffers not equal")
	}

	if HermesBufferToBytes(nil) != nil {
		t.Fatal("Expected nil")
	}

	result := HermesBufferToBytes(BytesToHermesBuffer(nil))
	if !bytes.Equal(result, []byte{}) {
		t.Fatal("Expected []byte{}")
	}

	buffer, err = NewHermesBuffer(nil)
	if err != ErrHermesBufferCreationError {
		t.Fatal("Expected error")
	}
}
