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

package main

import (
	"net"
	"github.com/cossacklabs/hermes-core/gohermes"
	"github.com/cossacklabs/hermes-core/gohermes/midhermes/rpc"
	"encoding/base64"
	"flag"
	"fmt"
	"io/ioutil"
	"github.com/BurntSushi/toml"
)

type TCPTransport struct {
	connection net.Conn
}

func NewTransport(uri string) (TCPTransport, error) {
	conn, err := net.Dial("tcp", uri)
	if err != nil {
		return TCPTransport{}, err
	}
	return TCPTransport{conn}, nil
}

func (t TCPTransport) Write(buf []byte) error {
	var total_send = 0
	for total_send < len(buf) {
		sended, err := t.connection.Write(buf[total_send:])
		if nil != err {
			return err
		}
		total_send += sended
	}
	return nil
}

func (t TCPTransport) Read(buf []byte) error {
	var total_received = 0
	for total_received < len(buf) {
		received, err := t.connection.Read(buf[total_received:])
		if nil != err {
			return err
		}
		total_received += received
	}
	return nil
}

func (t TCPTransport) Close() error {
	return t.connection.Close()
}

type Config struct {
	CredentialStoreUrl       string `toml:"credential_store_url"`
	CredentialStoreId        string `toml:"credential_store_id"`
	CredentialStorePublicKey string `toml:"credential_store_public_key"`

	DataStoreUrl       string `toml:"data_store_url"`
	DataStoreId        string `toml:"data_store_id"`
	DataStorePublicKey string `toml:"data_store_public_key"`

	KeyStoreUrl       string `toml:"key_store_url"`
	KeyStoreId        string `toml:"key_store_id"`
	KeyStorePublicKey string `toml:"key_store_public_key"`
}

func usage() {
	fmt.Println(`hermes_client:
  -command string
    	command [add_block | read_block | update_block | delete_block | rotate_block | grant_read_access | grant_update_access | revoke_read_access | revoke_update_access ]
  -doc string
    	doc
  -for_user string
    	for user id
  -meta string
    	meta`)
}

func main() {
	var config = flag.String("config", "client.conf", "Config with settings")
	var userId = flag.String("id", "", "User's id that run command")
	var userPrivateKey = flag.String("private_key", "", "User's private key")
	var docFileName = flag.String("doc", "", "doc")
	var meta = flag.String("meta", "", "meta")
	var forUser = flag.String("for_user", "", "for user id")
	var command = flag.String("command", "", "command [add_block | read_block | update_block | delete_block | rotate_block | grant_read_access | grant_update_access | revoke_read_access | revoke_update_access ]")
	flag.Parse()
	var conf Config
	if _, err := toml.DecodeFile(*config, &conf); err != nil {
		panic(err)
	}
	flag.Usage = usage

	if *userId == "" || *userPrivateKey == "" ||
		conf.CredentialStoreId == "" || conf.CredentialStorePublicKey == "" || conf.CredentialStoreUrl == "" ||
		conf.KeyStoreUrl == "" || conf.KeyStoreId == "" || conf.KeyStorePublicKey == "" ||
		conf.DataStoreUrl == "" || conf.DataStoreId == "" || conf.DataStorePublicKey == "" {
		fmt.Println("all parameters from config file are required")
		flag.Usage()
		return
	}
	if *docFileName == "" || *command == "" {
		fmt.Println("<command> and <doc> parameters are required")
	}
	privateKey, err := base64.StdEncoding.DecodeString(*userPrivateKey)
	if nil != err {
		panic(err)
		return
	}
	CredentialStoreTransport, err := NewTransport(conf.CredentialStoreUrl)
	if nil != err {
		panic(err)
		return
	}
	credentialPublic, err := base64.StdEncoding.DecodeString(conf.CredentialStorePublicKey)
	if err != nil {
		panic(err)
	}
	defer CredentialStoreTransport.Close()
	secureCredentialTransport, err := rpc.NewSecureTransport([]byte(*userId), privateKey, []byte(conf.CredentialStoreId), credentialPublic, CredentialStoreTransport, false)
	if err != nil {
		panic(err)
	}

	dataStorePublic, err := base64.StdEncoding.DecodeString(conf.DataStorePublicKey)
	if err != nil {
		panic(err)
	}
	DataStoreTransport, err := NewTransport(conf.DataStoreUrl)
	if nil != err {
		panic(err)
		return
	}
	defer DataStoreTransport.Close()
	secureDataStoreTransport, err := rpc.NewSecureTransport([]byte(*userId), privateKey, []byte(conf.DataStoreId), dataStorePublic, DataStoreTransport, false)
	if err != nil {
		panic(err)
	}

	keyPublic, err := base64.StdEncoding.DecodeString(conf.KeyStorePublicKey)
	if err != nil {
		panic(err)
	}
	KeyStoreTransport, err := NewTransport(conf.KeyStoreUrl)
	if nil != err {
		panic(err)
	}
	defer KeyStoreTransport.Close()
	secureKeyStoreTransport, err := rpc.NewSecureTransport([]byte(*userId), privateKey, []byte(conf.KeyStoreId), keyPublic, KeyStoreTransport, false)
	if err != nil {
		panic(err)
	}
	rpcHermes, err := rpc.NewRPCMidHermes([]byte(*userId), privateKey, secureCredentialTransport, secureDataStoreTransport, secureKeyStoreTransport)
	if err != nil{
		panic(err)
	}
	mid_hermes, err := gohermes.NewMidHermes(rpcHermes)
	if nil != err {
		panic(err)
		return
	}
	defer mid_hermes.Close()

	switch *command {
	case "add_block":
		if *meta == "" {
			flag.Usage()
		}
		data, err := ioutil.ReadFile(*docFileName)
		if nil != err {
			panic(err)
		}
		err = mid_hermes.AddBlock([]byte(*docFileName), data, []byte(*meta))
		if nil != err {
			panic(err)
		}
	case "read_block":
		data, meta, err := mid_hermes.ReadBlock([]byte(*docFileName))
		if nil != err {
			panic(err)
		}
		fmt.Println(string(data))
		fmt.Println(string(meta))
	case "update_block":
		if *meta == "" {
			flag.Usage()
		}
		data, err := ioutil.ReadFile(*docFileName)
		if nil != err {
			panic(err)
		}
		err = mid_hermes.UpdateBlock([]byte(*docFileName), data, []byte(*meta))
		if nil != err {
			panic(err)
		}
	case "delete_block":
		err := mid_hermes.DeleteBlock([]byte(*docFileName))
		if nil != err {
			panic(err)
		}
	case "rotate_block":
		err := mid_hermes.RotateBlock([]byte(*docFileName))
		if nil != err {
			panic(err)
		}
	case "grant_read_access":
		if *forUser == "" {
			flag.Usage()
			return
		}
		err := mid_hermes.GrantReadAccess([]byte(*docFileName), []byte(*forUser))
		if nil != err {
			panic(err)
		}
	case "grant_update_access":
		if *forUser == "" {
			flag.Usage()
			return
		}
		err := mid_hermes.GrantWriteAccess([]byte(*docFileName), []byte(*forUser))
		if nil != err {
			panic(err)
		}
	case "revoke_read_access":
		if *forUser == "" {
			flag.Usage()
			return
		}
		err := mid_hermes.RevokeReadAccess([]byte(*docFileName), []byte(*forUser))
		if nil != err {
			panic(err)
		}
	case "revoke_update_access":
		if *forUser == "" {
			flag.Usage()
			return
		}
		err := mid_hermes.RevokeWriteAccess([]byte(*docFileName), []byte(*forUser))
		if nil != err {
			panic(err)
		}
	default:
		flag.Usage()
	}
	fmt.Println("success")
}
