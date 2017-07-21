//
// Copyright (c) 2017 Cossack Labs Limited
//
// This file is part of Hermes.
//
// Hermes is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hermes is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with Hermes.  If not, see <http://www.gnu.org/licenses/>.
//
//

package main

import (
	"net"
	"../../../gohermes"
	"flag"
	"fmt"
	"io/ioutil"
	"encoding/base64"
)

const CredentialStoreURI = "127.0.0.1:8888" 
const DataStoreURI = "127.0.0.1:8889" 
const KeyStoreURI = "127.0.0.1:8890" 

type Transport struct{
	connection net.Conn
}

func NewTransport(uri string) (Transport, error){
	conn, err := net.Dial("tcp", uri)
	if err != nil {
		return Transport{}, err
	}
	return Transport{conn}, nil
}

func (t Transport)Send(buf []byte) error{
	var total_send = 0
	for total_send < len(buf) {
		sended, err := t.connection.Write(buf[total_send:]);
		if nil != err {
			return err 
		}
		total_send += sended
	}
	return nil
}

func (t Transport)Recv(buf []byte) error{
	var total_received = 0
	for total_received < len(buf){
		received, err := t.connection.Read(buf[total_received:])
		if nil != err {
			return err 
		}
		total_received += received		
	}
	return nil
}

func (t Transport)Close(){
	t.connection.Close()
}

func usage() string{
	return "Usage of ./hermes_client:\n  -command string\n    	command\n  -doc string\n    	doc\n  -for_user string\n    	for user id\n  -id string\n    	user id\n  -meta string\n    	meta\n  -sk string\n    	user private key"
}

func main(){
	var id = flag.String("id", "", "user id")
	var private_key = flag.String("sk", "", "user private key")
	var doc_file_name = flag.String("doc", "", "doc")
	var meta = flag.String("meta", "", "meta")
	var for_user = flag.String("for_user", "", "for user id")
	var command = flag.String("command", "", "command")
	flag.Parse()

	if *id == "" || *private_key == "" || *doc_file_name == "" || *command == "" {
		fmt.Println(usage())
		return
	}
	sk, err := base64.StdEncoding.DecodeString(*private_key)
	if nil != err {
		panic(err)
		return
	}	
	CredentialStoreTransport, err := NewTransport(CredentialStoreURI)
	if nil != err {
		panic(err)
		return
	}
	defer CredentialStoreTransport.Close()
	DataStoreTransport, err := NewTransport(DataStoreURI)
	if nil != err {
		panic(err)
		return
	}
	defer DataStoreTransport.Close()
	KeyStoreTransport, err := NewTransport(KeyStoreURI)
	if nil != err {
		panic(err)
		return
	}
	defer KeyStoreTransport.Close()

	mid_hermes, err := gohermes.NewMidHermes([]byte(*id), sk, KeyStoreTransport, DataStoreTransport, CredentialStoreTransport)
	if nil != err {
		panic(err)
		return
	}
	defer mid_hermes.Close()

	switch *command{
	case "ba":
		if *meta == ""{
			fmt.Println(usage())
		}
		data, err := ioutil.ReadFile(*doc_file_name)
		if nil != err {
			panic(err)
		}
		err = mid_hermes.AddBlock([]byte(*doc_file_name), data, []byte(*meta));
	case "br":
		data, meta, err := mid_hermes.ReadBlock([]byte(*doc_file_name));
		if nil != err {
			panic(err)
		}
		fmt.Println(string(data))
		fmt.Println(string(meta))
	case "bu":
		if *meta == ""{
			fmt.Println(usage())
		}
		data, err := ioutil.ReadFile(*doc_file_name)
		if nil != err {
			panic(err)
		}
		err = mid_hermes.UpdateBlock([]byte(*doc_file_name), data, []byte(*meta))
		if nil != err {
			panic(err)
		}
	case "bd":
		err := mid_hermes.DeleteBlock([]byte(*doc_file_name));
		if nil != err {
			panic(err)
		}
	case "brotate":
		err := mid_hermes.RotateBlock([]byte(*doc_file_name));
		if nil != err {
			panic(err)
		}
	case "rgr":
		if *for_user == "" {
			fmt.Println(usage())
			return
		}
		err := mid_hermes.GrantReadAccess([]byte(*doc_file_name), []byte(*for_user));
		if nil != err {
			panic(err)
		}
	case "rgu":
		if *for_user == "" {
			fmt.Println(usage())
			return
		}
		err := mid_hermes.GrantUpdateAccess([]byte(*doc_file_name), []byte(*for_user));
		if nil != err {
			panic(err)
		}
	case "rdr":
		if *for_user == "" {
			fmt.Println(usage())
			return
		}
		err := mid_hermes.RevokeReadAccess([]byte(*doc_file_name), []byte(*for_user));
		if nil != err {
			panic(err)
		}
	case "rdu":
		if *for_user == "" {
			fmt.Println(usage())
			return
		}
		err := mid_hermes.RevokeUpdateAccess([]byte(*doc_file_name), []byte(*for_user));
		if nil != err {
			panic(err)
		}
	default:
		fmt.Println(usage(), *for_user)
	}
	fmt.Println("success")
}


