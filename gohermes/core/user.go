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

// User wraps mid_hermes_ll_user_t C struct
type User struct {
	hermesUser *C.mid_hermes_ll_user_t
}

// GetHermesUser return copy of wrapped mid_hermes_ll_user_t struct
func (user *User) GetHermesUser() (*C.mid_hermes_ll_user_t, error) {
	userCopy, err := CopyHermesUser(user.hermesUser)
	if err != nil{
		return nil, err
	}
	return userCopy, nil
}

// finalizeHermesUser free memory allocated on C side
func finalizeHermesUser(user *User) {
	result := C.mid_hermes_ll_user_destroy(&(user.hermesUser))
	if result != C.HM_SUCCESS {
		log.Println("can't free memory for hermes user")
	}
}

// NewUser return new User object. privateKey can be empty []byte or nil but publicKey not
// hermes uses themis key generation functions so private/public keys should be generates via
// github.com/cossacklabs/themis/gothemis/keys.NewBlock function
func NewUser(id, privateKey, publicKey []byte) (*User, error) {
	if len(id) == 0 || len(publicKey) == 0 {
		return nil, ErrUserCreation
	}
	user := &User{}
	if len(privateKey) > 0 {
		user.hermesUser = C.mid_hermes_ll_local_user_create(
			C.mid_hermes_ll_buffer_create((*C.uint8_t)(unsafe.Pointer(&id[0])), C.size_t(len(id))),
			C.mid_hermes_ll_buffer_create((*C.uint8_t)(unsafe.Pointer(&privateKey[0])), C.size_t(len(privateKey))),
			C.mid_hermes_ll_buffer_create((*C.uint8_t)(unsafe.Pointer(&publicKey[0])), C.size_t(len(publicKey))))
	} else {
		user.hermesUser = C.mid_hermes_ll_user_create(
			C.mid_hermes_ll_buffer_create((*C.uint8_t)(unsafe.Pointer(&id[0])), C.size_t(len(id))),
			C.mid_hermes_ll_buffer_create((*C.uint8_t)(unsafe.Pointer(&publicKey[0])), C.size_t(len(publicKey))))
	}
	if user.hermesUser == nil {
		return nil, ErrUserCreation
	}
	runtime.SetFinalizer(user, finalizeHermesUser)
	return user, nil
}

// NewUserFromHermesUser return new *User that wraps copy of hermesUser
func NewUserFromHermesUser(hermesUser *C.mid_hermes_ll_user_t)(*User, error){
	userCopy, err := CopyHermesUser(hermesUser)
	if err != nil{
		return nil, err
	}
	user := &User{hermesUser: userCopy}
	runtime.SetFinalizer(user, finalizeHermesUser)
	return user, nil
}

func (user *User) GetId() []byte {
	return HermesBufferToBytes(user.hermesUser.id)
}

// Copy return copy of user without runtime.finalizer and copied hermesUser object should be free manually or by C code
func (user *User) Copy() (*User, error) {
	hermesUserCopy, err := CopyHermesUser(user.hermesUser)
	if err != nil{
		return nil, err
	}
	userCopy := &User{hermesUser: hermesUserCopy}
	runtime.SetFinalizer(userCopy, finalizeHermesUser)
	return userCopy, nil
}

func (user *User) GetPublicKey()[]byte{
	return HermesBufferToBytes(user.hermesUser.pk)
}

func CopyHermesUser(hermesUser *C.mid_hermes_ll_user_t)(*C.mid_hermes_ll_user_t, error){
	var userCopy *C.mid_hermes_ll_user_t = C.mid_hermes_ll_user_copy(hermesUser)
	if userCopy == nil{
		return nil, ErrUserCopy
	}
	return userCopy, nil
}