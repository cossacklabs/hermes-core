#
# Copyright (c) 2015 Cossack Labs Limited
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

COMMON_TEST_SRC = $(wildcard tests/common/*.c)
COMMON_TEST_OBJ = $(patsubst $(TEST_SRC_PATH)/%.c,$(TEST_OBJ_PATH)/%.o, $(COMMON_TEST_SRC))

include tests/rpc/rpc.mk
include tests/credential_store/credential_store.mk
include tests/key_store/key_store.mk
include tests/data_store/data_store.mk

rpc_test: CMD = $(CC) -o $(TEST_BIN_PATH)/rpc_test $(RPC_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH)  $(LDFLAGS) -lhermes_rpc -lhermes_common -lthemis -lsoter $(COVERLDFLAGS) -lpthread

rpc_test: rpc_static $(RPC_TEST_OBJ) $(COMMON_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

credential_store_test: CMD = $(CC) -o $(TEST_BIN_PATH)/credential_store_test $(CREDENTIAL_STORE_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH)  $(LDFLAGS) -lhermes_credential_store -lhermes_rpc -lhermes_common -lthemis -lsoter $(COVERLDFLAGS) -lpthread

credential_store_test: credential_store_static rpc_static $(CREDENTIAL_STORE_TEST_OBJ) $(COMMON_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

key_store_test: CMD = $(CC) -o $(TEST_BIN_PATH)/key_store_test $(KEY_STORE_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH)  $(LDFLAGS) -lhermes_key_store -lhermes_rpc -lhermes_common -lthemis -lsoter $(COVERLDFLAGS) -lpthread

key_store_test: key_store_static rpc_static $(KEY_STORE_TEST_OBJ) $(COMMON_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

data_store_test: CMD = $(CC) -o $(TEST_BIN_PATH)/data_store_test $(DATA_STORE_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH)  $(LDFLAGS) -lhermes_data_store -lhermes_rpc -lhermes_common -lthemis -lsoter $(COVERLDFLAGS) -lpthread

data_store_test: data_store_static rpc_static $(DATA_STORE_TEST_OBJ) $(COMMON_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

test: rpc_test credential_store_test key_store_test data_store_test

check: 
	$(TEST_BIN_PATH)/rpc_test
	$(TEST_BIN_PATH)/credential_store_test
	$(TEST_BIN_PATH)/key_store_test
	$(TEST_BIN_PATH)/data_store_test
