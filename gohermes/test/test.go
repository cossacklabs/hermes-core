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
package main

import (
	"encoding/base64"

	"fmt"

	"github.com/cossacklabs/hermes-base/src/hc_transport"
	"github.com/cossacklabs/hermes-core/gohermes"
)

const CREDENTIAL_ID = "credential_store"
const CREDENTIAL_URL = "tcp://127.0.0.1:8888"
const CREDENTIAL_PUBLIC = "VUVDMgAAAC1x1lf9Az0bNDSYU8TG8XcBBwsciK6nOo4H9/VeSb2carumNQla"

const KEY_ID = "key_store_server"
const KEY_URL = "tcp://127.0.0.1:8889"
const KEY_PUBLIC = "VUVDMgAAAC3QMLOAAoms9u5nTh1Ir3AnTPt5RkMJY9leIfF6uMIxms/Bkywp"

const DATA_ID = "data_store_server"
const DATA_URL = "tcp://127.0.0.1:8890"
const DATA_PUBLIC = "VUVDMgAAAC0VCQ/fAt88d2N8vDFVAKbDJHsXew8HgB55PIrVfhELXrEf1N89"

func main() {
	testId := []byte("user1")
	private_key, err := base64.StdEncoding.DecodeString("UkVDMgAAAC0Tj5tGAPfpgfYMBACxX6onvlWvcc2Gb9ZylBlJdjebTpV3OCIx")
	if err != nil {
		panic(err)
	}
	credential_public, err := base64.StdEncoding.DecodeString(CREDENTIAL_PUBLIC)
	if err != nil {
		panic(err)
	}
	key_store_public, err := base64.StdEncoding.DecodeString(KEY_PUBLIC)
	if err != nil {
		panic(err)
	}
	data_store_public, err := base64.StdEncoding.DecodeString(DATA_PUBLIC)
	if err != nil {
		panic(err)
	}
	// credential store transport
	credential_connector, err := hc_transport.NewSimpleConnector(CREDENTIAL_URL, false)
	if err != nil {
		panic(err)
	}
	credential_connection, err := credential_connector.Connect()
	if err != nil {
		panic(err)
	}
	raw_credential_transport := hc_transport.NewSyncTransport(credential_connection)
	credential_transport, err := gohermes.NewSecureTransport(testId, private_key, []byte(CREDENTIAL_ID), credential_public, raw_credential_transport, false)
	if err != nil {
		panic(err)
	}

	// key store transport
	key_connector, err := hc_transport.NewSimpleConnector(KEY_URL, false)
	if err != nil {
		panic(err)
	}
	key_connection, err := key_connector.Connect()
	if err != nil {
		panic(err)
	}
	raw_key_transport := hc_transport.NewSyncTransport(key_connection)
	key_transport, err := gohermes.NewSecureTransport(testId, private_key, []byte(KEY_ID), key_store_public, raw_key_transport, false)
	if err != nil {
		panic(err)
	}

	// data store transport
	data_connector, err := hc_transport.NewSimpleConnector(DATA_URL, false)
	if err != nil {
		panic(err)
	}
	data_connection, err := data_connector.Connect()
	if err != nil {
		panic(err)
	}
	raw_data_transport := hc_transport.NewSyncTransport(data_connection)
	data_transport, err := gohermes.NewSecureTransport(testId, private_key, []byte(DATA_ID), data_store_public, raw_data_transport, false)
	if err != nil {
		panic(err)
	}

	mid_hermes, err := gohermes.NewMidHermes(
		testId, private_key, key_transport, data_transport, credential_transport)
	if err != nil {
		panic(err)
	}
	type Block struct {
		id   []byte
		data []byte
		meta []byte
	}
	testBlock := Block{testId, []byte("test data"), []byte("test meta")}
	blockId, err := mid_hermes.AddBlock(testBlock.id, testBlock.data, testBlock.meta)
	if err != nil {
		panic(err)
	}
	fmt.Println("BlockId=", blockId, "userId=", testId)
}
