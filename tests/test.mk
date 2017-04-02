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

rpc_test: CMD = $(CC) -o $(TEST_BIN_PATH)/rpc_test $(RPC_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH)  $(LDFLAGS) -lrpc -lcommon -lsoter $(COVERLDFLAGS) -lpthread

rpc_test: rpc_static $(RPC_TEST_OBJ) $(COMMON_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

credential_store_test: CMD = $(CC) -o $(TEST_BIN_PATH)/credential_store_test $(CREDENTIAL_STORE_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH)  $(LDFLAGS) -lcredential_store -lrpc -lcommon -lsoter $(COVERLDFLAGS) -lpthread

credential_store_test: credential_store_static rpc_static $(CREDENTIAL_STORE_TEST_OBJ) $(COMMON_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

test: rpc_test credential_store_test

check: 
	$(TEST_BIN_PATH)/rpc_test