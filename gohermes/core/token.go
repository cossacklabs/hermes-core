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
)
// Token wrap mid_hermes_ll_token_t C struct
type Token struct {
	hermesToken *C.mid_hermes_ll_token_t
}

//finalizeToken free memory of mid_hermes_ll_token_t C struct allocated on C side
func finalizeToken(token *Token) {
	result := C.mid_hermes_ll_token_destroy(&(token.hermesToken))
	if result != C.HM_SUCCESS {
		log.Println("can't free memory for hermes token")
	}
}

// NewToken generate new token
func NewToken(ownerUser *User) (*Token, error) {
	hermesOwnerUser, err := ownerUser.GetHermesUser()
	if err != nil{
		return nil, err
	}
	token := &Token{
		hermesToken: C.mid_hermes_ll_token_generate(hermesOwnerUser),
	}
	if token.hermesToken == nil {
		return nil, ErrHermesTokenGenerationError
	}
	runtime.SetFinalizer(token, finalizeToken)
	return token, nil
}

// NewTokenFromHermesToken return new Token that use copy of hermesToken data
func NewTokenFromHermesToken(hermesToken *C.mid_hermes_ll_token_t)(*Token, error){
	hermesUserCopy, err := CopyHermesUser(hermesToken.user)
	if err != nil{
		return nil, err
	}
	var hermesOwnerCopy *C.mid_hermes_ll_user_t
	if hermesToken.user == hermesToken.owner{
		hermesOwnerCopy = hermesUserCopy
	} else {
		hermesOwnerCopy, err = CopyHermesUser(hermesToken.owner)
		if err != nil{
			C.mid_hermes_ll_user_destroy(&(hermesUserCopy))
			return nil, err
		}
	}
	tokenDataCopy := C.mid_hermes_ll_buffer_create(hermesToken.token.data, hermesToken.token.length)
	if tokenDataCopy == nil{
		if hermesOwnerCopy == hermesUserCopy{
			C.mid_hermes_ll_user_destroy(&(hermesUserCopy))
		} else {
			C.mid_hermes_ll_user_destroy(&(hermesOwnerCopy))
			C.mid_hermes_ll_user_destroy(&(hermesUserCopy))
		}
		return nil, ErrHermesTokenCreateError
	}
	tokenCopy := C.mid_hermes_ll_token_create(hermesUserCopy, hermesOwnerCopy, tokenDataCopy)
	if tokenCopy == nil{
		if hermesOwnerCopy == hermesUserCopy{
			C.mid_hermes_ll_user_destroy(&(hermesUserCopy))
		} else {
			C.mid_hermes_ll_user_destroy(&(hermesOwnerCopy))
			C.mid_hermes_ll_user_destroy(&(hermesUserCopy))
		}
		C.mid_hermes_ll_buffer_destroy(&(tokenDataCopy))
		return nil, ErrHermesTokenCreateError
	}
	newToken := &Token{hermesToken: tokenCopy}
	runtime.SetFinalizer(newToken, finalizeToken)
	return newToken, nil
}

// newHermesToken return copy of C struct
func newHermesToken(data []byte, user, owner *User)(*C.mid_hermes_ll_token_t, error){
	hermesOwnerUser, err := owner.GetHermesUser()
	if err != nil{
		return nil, err
	}
	var hermesUser *C.mid_hermes_ll_user_t
	if user == owner{
		hermesUser = hermesOwnerUser
	} else {
		hermesUser, err = user.GetHermesUser()
		if err != nil{
			C.mid_hermes_ll_user_destroy(&(hermesOwnerUser))
			return nil, err
		}
	}
	hermesToken := C.mid_hermes_ll_token_create(hermesUser, hermesOwnerUser, BytesToHermesBuffer(data))
	if hermesToken == nil{
		// doesn't need to free hermesOwnerUser and hermesUser because it should do mid_hermes_ll_token_create
		return nil, ErrHermesTokenCreateError
	}
	return hermesToken, nil
}

// GetHermesToken return pointer to wrapped copy of mid_hermes_ll_token_t
func (token *Token) GetHermesToken()(*C.mid_hermes_ll_token_t, error){
	// it's hack with direct create User struct (without using factory method NewUser) to avoid double
	// allocation/deallocation of hermesUser field because newHermesToken also will copy user params
	return newHermesToken(
		token.GetEncryptedData(), &User{hermesUser: token.hermesToken.user},
		&User{hermesUser: token.hermesToken.owner})
}

// NewTokenFromEncryptedBytes create new hermes token object with encrypted token tokenData
func NewTokenFromEncryptedBytes(encryptedTokenData []byte, user, ownerUser *User) (*Token, error) {
	hermesToken, err := newHermesToken(encryptedTokenData, user, ownerUser)
	if err != nil{
		return nil, err
	}
	token := &Token{
		hermesToken: hermesToken,
	}
	runtime.SetFinalizer(token, finalizeToken)
	return token, nil
}

// GetData return decrypted token
func (token *Token) GetData() []byte {
	newHermesBuffer := C.mid_hermes_ll_token_get_data(token.hermesToken)
	// data will has copy of newHermessBuffer data as byte slice
	data := HermesBufferToBytes(newHermesBuffer)
	// free newHermesBuffer that allocated on C side and doesn't need in the future
	finalizeHermesBuffer(newHermesBuffer)
	return data
}

// GetEncryptedData return encrypted token as is
func (token *Token) GetEncryptedData() []byte {
	return HermesBufferToBytes(token.hermesToken.token)
}

// TokenForUser return new token encrypted for userFor
func (token *Token) TokenForUser(userFor *User, ownerUser *User)(*Token, error){
	hermesUserFor, err := userFor.GetHermesUser()
	if err != nil{
		return nil, err
	}
	newHermesToken := C.mid_hermes_ll_token_get_token_for_user(token.hermesToken, hermesUserFor)
	if newHermesToken == nil{
		return nil, ErrHermesTokenCreateError
	}
	newToken := &Token{hermesToken:newHermesToken}
	runtime.SetFinalizer(newToken, finalizeToken)
	return newToken, nil
}