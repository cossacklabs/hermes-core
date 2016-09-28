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
LDFLAGS += -Lbuild -lthemis -lsoter

ifeq ($(PREFIX),)
PREFIX = /usr
endif

ifneq ($(WITH_FILE_DB),)
LDFLAGS += -lfile_db
endif

SHARED_EXT = so

ifdef DEBUG
# Making debug build for now
	CFLAGS += -DDEBUG -g
endif

CFLAGS += -Werror -Wno-switch
ifndef ERROR
include src/srpc/srpc.mk
include src/protocols/protocols.mk
include src/services/services.mk
include src/common/common.mk
include src/mid_hermes/midHermes.mk
include src/db/file_db/file_db.mk
endif


all: err srpc_shared protocols_shared common_shared services_shared midHermes_shared file_db_shared

test_all: err test

common_static: $(COMMON_OBJ)
	$(AR) rcs $(BIN_PATH)/lib$(COMMON_BIN).a $(COMMON_OBJ)

common_shared: $(COMMON_OBJ)
	$(CC) -shared -o $(BIN_PATH)/lib$(COMMON_BIN).$(SHARED_EXT) $(COMMON_OBJ) $(LDFLAGS)

file_db_static: $(FILE DB_OBJ)
	$(AR) rcs $(BIN_PATH)/lib$(DB_BIN).a $(DB_OBJ)

file_db_shared: $(FILE_DB_OBJ)
	$(CC) -shared -o $(BIN_PATH)/lib$(FILE_DB_BIN).$(SHARED_EXT) $(FILE_DB_OBJ)

srpc_static: $(SRPC_OBJ)
	$(AR) rcs $(BIN_PATH)/lib$(SRPC_BIN).a $(SRPC_OBJ)

srpc_shared: $(SRPC_OBJ)
	$(CC) -shared -o $(BIN_PATH)/lib$(SRPC_BIN).$(SHARED_EXT) $(SRPC_OBJ) $(LDFLAGS)

protocols_static: $(PROTOCOLS_OBJ)
	$(AR) rcs $(BIN_PATH)/lib$(PROTOCOLS_BIN).a $(PROTOCOLS_OBJ)

protocols_shared: $(PROTOCOLS_OBJ)
	$(CC) -shared -o $(BIN_PATH)/lib$(PROTOCOLS_BIN).$(SHARED_EXT) $(PROTOCOLS_OBJ) $(LDFLAGS)

services_static: $(SERVICES_OBJ)
	$(AR) rcs $(BIN_PATH)/lib$(SERVICES_BIN).a $(SERVICES_OBJ)

services_shared: $(SERVICES_OBJ)
	$(CC) -shared -o $(BIN_PATH)/lib$(SERVICES_BIN).$(SHARED_EXT) $(SERVICES_OBJ) $(LDFLAGS)

midHermes_static: $(MIDHERMES_OBJ)
	$(AR) rcs $(BIN_PATH)/lib$(MIDHERMES_BIN).a $(MIDHERMES_OBJ)

midHermes_shared: $(MIDHERMES_OBJ)
	$(CC) -shared -o $(BIN_PATH)/lib$(MIDHERMES_BIN).$(SHARED_EXT) $(MIDHERMES_OBJ) $(LDFLAGS) -lcommon

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

install: err all
	mkdir -p $(PREFIX)/include/hermes $(PREFIX)/lib
	install include/hermes/*.h $(PREFIX)/include/hermes
#	install $(BIN_PATH)/*.a $(PREFIX)/lib
	install $(BIN_PATH)/*.$(SHARED_EXT) $(PREFIX)/lib

uninstall: 
	rm -rf $(PREFIX)/include/hermes
	rm -f $(PREFIX)/lib/libmid_hermes.a
	rm -f $(PREFIX)/lib/libsrpc.a
	rm -f $(PREFIX)/lib/libcommon.a
	rm -f $(PREFIX)/lib/libmid_hermes.so
	rm -f $(PREFIX)/lib/libsrpc.so
	rm -f $(PREFIX)/lib/libcommon.so
