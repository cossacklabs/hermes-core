#
# Copyright (c) 2016 Cossack Labs Limited
#
# This file is part of Hermes.
#
# Hermes is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Hermes is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with Hermes.  If not, see <http://www.gnu.org/licenses/>.
#
#


CC = clang
INCLUDE_PATH = include
SRC_PATH = src
BIN_PATH = build
OBJ_PATH = build/obj
TEST_SRC_PATH = tests
TEST_OBJ_PATH = build/tests/obj
TEST_BIN_PATH = build/tests

CFLAGS += -I$(INCLUDE_PATH) -fPIC
LDFLAGS += -Lbuild -Llibs/themis/build

ifeq ($(PREFIX),)
PREFIX = /usr
endif

SHARED_EXT = so

ifdef DEBUG
# Making debug build for now
	CFLAGS += -DDEBUG -g
endif

CFLAGS += -Werror -Wno-switch

NO_COLOR=\033[0m
OK_COLOR=\033[32;01m
ERROR_COLOR=\033[31;01m
WARN_COLOR=\033[33;01m

OK_STRING=$(OK_COLOR)[OK]$(NO_COLOR)
ERROR_STRING=$(ERROR_COLOR)[ERRORS]$(NO_COLOR)
WARN_STRING=$(WARN_COLOR)[WARNINGS]$(NO_COLOR)

AWK_CMD = awk '{ printf "%-30s %-10s\n",$$1, $$2; }'
PRINT_OK = printf "$@ $(OK_STRING)\n" | $(AWK_CMD)
PRINT_OK_ = printf "$(OK_STRING)\n" | $(AWK_CMD)
PRINT_ERROR = printf "$@ $(ERROR_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n" && false
PRINT_ERROR_ = printf "$(ERROR_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n" && false
PRINT_WARNING = printf "$@ $(WARN_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n"
PRINT_WARNING_ = printf "$(WARN_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n"
BUILD_CMD = LOG=$$($(CMD) 2>&1) ; if [ $$? -eq 1 ]; then $(PRINT_ERROR); elif [ "$$LOG" != "" ] ; then $(PRINT_WARNING); else $(PRINT_OK); fi;
BUILD_CMD_ = LOG=$$($(CMD) 2>&1) ; if [ $$? -eq 1 ]; then $(PRINT_ERROR_); elif [ "$$LOG" != "" ] ; then $(PRINT_WARNING_); else $(PRINT_OK_); fi;

ifndef ERROR
include src/common/common.mk
include src/rpc/rpc.mk
include src/credential_store/credential_store.mk
include src/data_store/data_store.mk
include src/key_store/key_store.mk
endif


all: err core

test_all: err test

core: rpc_shared credential_store_shared data_store_shared key_store_shared

common_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(COMMON_BIN).a $(COMMON_OBJ)

common_static: $(COMMON_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)


rpc_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(RPC_BIN).a $(RPC_OBJ)

rpc_static: common_static $(RPC_OBJ) 
	@echo -n "link "
	@$(BUILD_CMD)

rpc_shared: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(RPC_BIN).$(SHARED_EXT) $(RPC_OBJ) $(LDFLAGS) -lcommon

rpc_shared: common_static $(RPC_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

credential_store_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(CREDENTIAL_STORE_BIN).a $(CREDENTIAL_STORE_OBJ)

credential_store_static: common_static rpc_static $(CREDENTIAL_STORE_OBJ) 
	@echo -n "link "
	@$(BUILD_CMD)

credential_store_shared: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(CREDENTIAL_STORE_BIN).$(SHARED_EXT) $(CREDENTIAL_STORE_OBJ) $(LDFLAGS) -lcommon -lrpc

credential_store_shared: common_static rpc_shared $(CREDENTIAL_STORE_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

data_store_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(DATA_STORE_BIN).a $(DATA_STORE_OBJ)

data_store_static: common_static rpc_static $(DATA_STORE_OBJ) 
	@echo -n "link "
	@$(BUILD_CMD)

data_store_shared: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(DATA_STORE_BIN).$(SHARED_EXT) $(DATA_STORE_OBJ) $(LDFLAGS) -lcommon -lrpc

data_store_shared: common_static rpc_shared $(DATA_STORE_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

key_store_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(KEY_STORE_BIN).a $(KEY_STORE_OBJ)

key_store_static: common_static rpc_static $(KEY_STORE_OBJ) 
	@echo -n "link "
	@$(BUILD_CMD)

key_store_shared: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(KEY_STORE_BIN).$(SHARED_EXT) $(KEY_STORE_OBJ) $(LDFLAGS) -lcommon -lrpc

key_store_shared: common_static rpc_shared $(KEY_STORE_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)


$(OBJ_PATH)/%.o: CMD = $(CC) $(CFLAGS) -c $< -o $@

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(@D)
	@echo -n "compile "
	@$(BUILD_CMD)

$(TEST_OBJ_PATH)/%.o: CMD = $(CC) $(CFLAGS) -I$(TEST_SRC_PATH) -c $< -o $@

$(TEST_OBJ_PATH)/%.o: $(TEST_SRC_PATH)/%.c
	@mkdir -p $(@D)
	@echo -n "compile "
	@$(BUILD_CMD)

include tests/test.mk

err: ; $(ERROR)

clean: CMD = rm -rf $(BIN_PATH)

clean:
	@$(BUILD_CMD)
