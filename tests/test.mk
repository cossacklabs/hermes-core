#
# Copyright (c) 2017 Cossack Labs Limited
#
# This file is a part of Hermes-core.
#
# Hermes-core is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Hermes-core is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with Hermes-core.  If not, see <http://www.gnu.org/licenses/>.
#
#

COMMON_TEST_SRC = $(wildcard tests/common/*.c)
COMMON_TEST_OBJ = $(patsubst $(TEST_SRC_PATH)/%.c,$(TEST_OBJ_PATH)/%.o, $(COMMON_TEST_SRC))

include tests/rpc/rpc.mk
include tests/credential_store/credential_store.mk
include tests/key_store/key_store.mk
include tests/data_store/data_store.mk
include tests/mid_hermes/mid_hermes.mk
include tests/mid_hermes_ll/mid_hermes_ll.mk

rpc_test: CMD = $(CC) -o $(TEST_BIN_PATH)/rpc_test $(RPC_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH) $(CFLAGS) $(LDFLAGS) -lhermes_rpc -lhermes_common -lthemis -lsoter $(COVERLDFLAGS) -lpthread

rpc_test: rpc_static $(RPC_TEST_OBJ) $(COMMON_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

credential_store_test: CMD = $(CC) -o $(TEST_BIN_PATH)/credential_store_test $(CREDENTIAL_STORE_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH) $(CFLAGS) $(LDFLAGS) -lhermes_credential_store -lhermes_rpc -lhermes_common -lthemis -lsoter $(COVERLDFLAGS) -lpthread

credential_store_test: credential_store_static rpc_static $(CREDENTIAL_STORE_TEST_OBJ) $(COMMON_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

key_store_test: CMD = $(CC) -o $(TEST_BIN_PATH)/key_store_test $(KEY_STORE_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH) $(CFLAGS)  $(LDFLAGS) -lhermes_key_store -lhermes_rpc -lhermes_common -lthemis -lsoter $(COVERLDFLAGS) -lpthread

key_store_test: key_store_static rpc_static $(KEY_STORE_TEST_OBJ) $(COMMON_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

data_store_test: CMD = $(CC) -o $(TEST_BIN_PATH)/data_store_test $(DATA_STORE_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH) $(CFLAGS)  $(LDFLAGS) -lhermes_data_store -lhermes_rpc -lhermes_common -lthemis -lsoter $(COVERLDFLAGS) -lpthread

data_store_test: data_store_static rpc_static $(DATA_STORE_TEST_OBJ) $(COMMON_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

mid_hermes_test: CMD = $(CC) -o $(TEST_BIN_PATH)/mid_hermes_test $(MID_HERMES_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH) $(CFLAGS)  $(LDFLAGS)  -lhermes_mid_hermes -lhermes_credential_store -lhermes_key_store -lhermes_data_store -lhermes_mid_hermes_ll -lhermes_rpc -lhermes_common -lthemis -lsoter $(COVERLDFLAGS) -lpthread

mid_hermes_test: data_store_static key_store_static credential_store_static mid_hermes_static rpc_static $(MID_HERMES_TEST_OBJ) $(COMMON_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

mid_hermes_ll_test: CMD = $(CC) -o $(TEST_BIN_PATH)/mid_hermes_ll_test $(MID_HERMES_OBJ) $(MID_HERMES_LL_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH) $(CFLAGS) $(LDFLAGS) -lhermes_mid_hermes -lhermes_credential_store -lhermes_key_store -lhermes_data_store -lhermes_mid_hermes_ll -lhermes_rpc -lhermes_common -lthemis -lsoter $(COVERLDFLAGS) -lpthread

mid_hermes_ll_test: data_store_static key_store_static credential_store_static mid_hermes_static mid_hermes_ll_static rpc_static $(MID_HERMES_TEST_OBJ) $(MID_HERMES_LL_TEST_OBJ) $(COMMON_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)


prepare_tests: rpc_test credential_store_test key_store_test data_store_test mid_hermes_test mid_hermes_ll_test

test: err prepare_tests
	$(TEST_BIN_PATH)/rpc_test
	$(TEST_BIN_PATH)/credential_store_test
	$(TEST_BIN_PATH)/key_store_test
	$(TEST_BIN_PATH)/data_store_test
	$(TEST_BIN_PATH)/mid_hermes_test
	$(TEST_BIN_PATH)/mid_hermes_ll_test


# same as test, used in build bot. maybe later will test more packages
test_all: test