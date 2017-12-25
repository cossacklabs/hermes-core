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
	"testing"
	"bytes"
)

func TestNewUser(t *testing.T) {
	userId := []byte("id")
	_, err := NewUser(nil, nil, []byte("public key"))
	if err != ErrUserCreation {
		t.Fatal("err != ErrUserCreation with missed id")
	}
	_, err = NewUser(userId, nil, nil)
	if err != ErrUserCreation {
		t.Fatal("err != ErrUserCreation with missed public key")
	}
	publicKey := []byte("public key")
	user, err := NewUser(userId, nil, publicKey)
	if err != nil {
		t.Fatal("err != nil for correct user creation")
	}
	if user == nil {
		t.Fatal("user == nil for correct user creation")
	}
	if !bytes.Equal(user.GetId(), userId){
		t.Fatal("created user has incorrect id")
	}
	if !bytes.Equal(user.GetPublicKey(), publicKey){
		t.Fatal("created user has incorrect public key")
	}
}

func TestUser_Copy(t *testing.T) {
	userId := []byte("someid")
	publicKey := []byte("public key")
	user, err := NewUser(userId, nil, publicKey)
	if err != nil {
		t.Fatal("err != nil for correct user creation")
	}
	userCopy, err := user.Copy()
	if err != nil{
		t.Fatal(err)
	}
	if !bytes.Equal(user.GetId(), userCopy.GetId()){
		t.Fatal("created user has incorrect id")
	}
	if !bytes.Equal(user.GetPublicKey(), userCopy.GetPublicKey()){
		t.Fatal("created user has incorrect public key")
	}
}

func TestCopyHermesUser(t *testing.T) {
	userId := []byte("someid")
	publicKey := []byte("public key")
	user, err := NewUser(userId, nil, publicKey)
	if err != nil {
		t.Fatal("err != nil for correct user creation")
	}
	hermesUser, err := user.GetHermesUser()
	if err != nil{
		t.Fatal(err)
	}
	hermesUserCopy, err := CopyHermesUser(hermesUser)
	if err != nil{
		t.Fatal(err)
	}

	userCopy, err := NewUserFromHermesUser(hermesUserCopy)
	if err != nil{
		t.Fatal(err)
	}
	if !bytes.Equal(user.GetId(), userCopy.GetId()){
		t.Fatal("created user has incorrect id")
	}
	if !bytes.Equal(user.GetPublicKey(), userCopy.GetPublicKey()){
		t.Fatal("created user has incorrect public key")
	}
}