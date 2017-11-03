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
package gohermes

import (
	"testing"
	"time"

	"github.com/cossacklabs/themis/gothemis/keys"
)

type EmptyTransport struct {
	bufferIn  chan []byte
	bufferOut chan []byte
}

func (transport *EmptyTransport) Write(data []byte) error {
	transport.bufferOut <- data
	return nil
}
func (transport *EmptyTransport) Read(dataOut []byte) error {
	copy(dataOut, <-transport.bufferIn)
	return nil
}
func (transport *EmptyTransport) ReadFull(dataOut []byte) error {
	return transport.Read(dataOut)
}
func (transport *EmptyTransport) Close() error { return nil }

func TestNewTransport(t *testing.T) {
	_, err := NewTransport(&EmptyTransport{})
	if err != nil {
		t.Fatal(err)
	}
}

//TestNewSecureTransport create server and client transports that should connect to each other
func TestNewSecureTransport(t *testing.T) {
	bufferAChannel := make(chan []byte, 3)
	bufferBChannel := make(chan []byte, 3)
	finishAChannel := make(chan bool)
	finishBChannel := make(chan bool)

	keypairA, err := keys.New(keys.KEYTYPE_EC)
	if err != nil {
		t.Fatal(err)
	}
	keypairB, err := keys.New(keys.KEYTYPE_EC)
	if err != nil {
		t.Fatal(err)
	}
	testUserIdA := []byte("some user id A")
	testUserIdB := []byte("some user id B")
	var errA, errB error
	go func() {
		_, errA = NewSecureTransport(
			testUserIdA, keypairA.Private.Value, testUserIdB, keypairB.Public.Value,
			&EmptyTransport{bufferIn: bufferBChannel, bufferOut: bufferAChannel}, true)
		finishAChannel <- true
		if errA != nil {
			t.Fatal(errA)
		}
	}()
	go func() {
		_, errB = NewSecureTransport(testUserIdB, keypairB.Private.Value, testUserIdA, keypairA.Public.Value, &EmptyTransport{bufferIn: bufferAChannel, bufferOut: bufferBChannel}, false)
		finishBChannel <- true
		if errB != nil {
			t.Fatal(errB)

		}
	}()
	finishCount := 0
	for {
		select {
		case <-finishAChannel:
			finishCount++
			break
		case <-finishBChannel:
			finishCount++
			break
		case <-time.Tick(time.Second * 2):
			t.Fatal("too long create transport")
			return
		}
		if finishCount == 2 {
			break
		}
	}
	if errA != nil {
		t.Fatal(errA)
	}
	if errB != nil {
		t.Fatal(errB)
	}
}
