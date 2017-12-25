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
	"github.com/cossacklabs/themis/gothemis/keys"
	"testing"
)

func TestToken(t *testing.T) {
	keypair, err := keys.New(keys.KEYTYPE_EC)
	if err != nil {
		t.Fatal(err)
	}
	user, err := NewUser([]byte("id"), keypair.Private.Value, keypair.Public.Value)
	if err != nil {
		t.Fatal(err)
	}
	token, err := NewToken(user)
	if err != nil {
		t.Fatal(err)
	}
	tokenData := token.GetData()
	if len(tokenData) == 0 {
		t.Fatal("empty token data")
	}
	ownerUser, err := NewUser([]byte("id"), nil, keypair.Public.Value)
	if err != nil{
		t.Fatal(err)
	}
	token, err = NewTokenFromEncryptedBytes(token.GetEncryptedData(), user, ownerUser)
	if err != nil {
		t.Fatal(err)
	}
	if !bytes.Equal(token.GetData(), tokenData) {
		t.Fatal("incorrect token data")
	}
	if bytes.Equal(token.GetEncryptedData(), tokenData) {
		t.Fatal("encrypted and decrypted data should be different")
	}

	// generate with incorrect user's keys
	user, err = NewUser([]byte("id"), []byte("incorrect formatted key"), []byte("incorrect formatted key"))
	if err != nil {
		t.Fatal(err)
	}
	token, err = NewToken(user)
	if err != ErrHermesTokenGenerationError {
		t.Fatal("expected error")
	}
}

func TestTokenForUser(t *testing.T) {
	keypair1, err := keys.New(keys.KEYTYPE_EC)
	if err != nil {
		t.Fatal(err)
	}
	user1, err := NewUser([]byte("id"), keypair1.Private.Value, keypair1.Public.Value)
	if err != nil {
		t.Fatal(err)
	}
	keypair2, err := keys.New(keys.KEYTYPE_EC)
	if err != nil {
		t.Fatal(err)
	}
	user2, err := NewUser([]byte("id2"), keypair2.Private.Value, keypair2.Public.Value)
	if err != nil {
		t.Fatal(err)
	}
	user1Token, err := NewToken(user1)
	if err != nil {
		t.Fatal(err)
	}

	tokenCopy, err := NewTokenFromEncryptedBytes(user1Token.GetEncryptedData(), user1, user1)
	if err != nil{
		t.Fatal(err)
	}
	user2Token, err := tokenCopy.TokenForUser(user2, user1)
	if err != nil{
		t.Fatal(err)
	}
	if !bytes.Equal(user2Token.GetData(), user1Token.GetData()){
		t.Fatal("user1Token data not equal")
	}
	if bytes.Equal(user2Token.GetEncryptedData(), user1Token.GetEncryptedData()){
		t.Fatal("encrypted tokens should be different")
	}
}

func TestNewTokenFromHermesToken(t *testing.T) {
	keypair1, err := keys.New(keys.KEYTYPE_EC)
	if err != nil {
		t.Fatal(err)
	}
	owner, err := NewUser([]byte("id"), keypair1.Private.Value, keypair1.Public.Value)
	if err != nil {
		t.Fatal(err)
	}
	ownerToken, err := NewToken(owner)
	if err != nil {
		t.Fatal(err)
	}
	newToken, err := NewTokenFromHermesToken(ownerToken.hermesToken)
	if err != nil {
		t.Fatal(err)
	}
	if !bytes.Equal(ownerToken.GetEncryptedData(), newToken.GetEncryptedData()){
		t.Fatal("encrypted token data not equal")
	}
	if !bytes.Equal(ownerToken.GetData(), newToken.GetData()){
		t.Fatal("decrypted token data not equal")
	}
	keypair2, err := keys.New(keys.KEYTYPE_EC)
	if err != nil {
		t.Fatal(err)
	}
	// test when owner and user in token different

	// create token for new user
	anotherUser, err := NewUser([]byte("id2"), keypair2.Private.Value, keypair2.Public.Value)
	if err != nil {
		t.Fatal(err)
	}
	anotherUserToken, err := ownerToken.TokenForUser(anotherUser, owner)
	if err != nil {
		t.Fatal(err)
	}
	if bytes.Equal(ownerToken.GetEncryptedData(), anotherUserToken.GetEncryptedData()){
		t.Fatal("encrypted token data not equal")
	}
	if !bytes.Equal(ownerToken.GetData(), anotherUserToken.GetData()){
		t.Fatal("decrypted token data not equal")
	}
	// create token copy from token with different user/owner
	newToken, err = NewTokenFromHermesToken(anotherUserToken.hermesToken)
	if err != nil {
		t.Fatal(err)
	}
	if !bytes.Equal(anotherUserToken.GetEncryptedData(), newToken.GetEncryptedData()){
		t.Fatal("encrypted token data not equal")
	}
	if !bytes.Equal(anotherUserToken.GetData(), newToken.GetData()){
		t.Fatal("decrypted token data not equal")
	}
}