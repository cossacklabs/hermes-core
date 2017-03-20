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


#CC = clang
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
ifndef ERROR
include src/common/common.mk
include src/rpc/rpc.mk
endif


all: err core

test_all: err test

common_static: $(COMMON_OBJ)
	$(AR) rcs $(BIN_PATH)/lib$(COMMON_BIN).a $(COMMON_OBJ)

rpc_static: common_static $(RPC_OBJ) 
	$(AR) rcs $(BIN_PATH)/lib$(RPC_BIN).a $(RPC_OBJ)

rpc_shared: $(RPC_OBJ)
	$(CC) -shared -o $(BIN_PATH)/lib$(RPC_BIN).$(SHARED_EXT) $(RPC_OBJ) $(LDFLAGS)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_OBJ_PATH)/%.o: $(TEST_SRC_PATH)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(TEST_SRC_PATH) -c $< -o $@

include tests/test.mk

err: ; $(ERROR)

clean:
	rm -rf $(BIN_PATH)