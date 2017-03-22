#
# Copyright (c) 2015 Cossack Labs Limited
#

#CC = clang
INCLUDE_PATH = include
SRC_PATH = src
BIN_PATH = build
OBJ_PATH = build/obj
TEST_SRC_PATH = tests
TEST_OBJ_PATH = build/tests/obj
TEST_BIN_PATH = build/tests

CFLAGS += -I$(INCLUDE_PATH) -I/usr/local/include -fPIC `pkg-config --cflags glib-2.0`
LDFLAGS += -L/usr/local/lib -Lbuild -lzmq -lczmq -lpthread -lthemis -lsoter

ifeq ($(PREFIX),)
PREFIX = /usr
endif

SHARED_EXT = so

ifdef DEBUG
# Making debug build for now
	CFLAGS += -DDEBUG -g
endif

CFLAGS += -Werror -Wno-switch -DHERMES_USE_MONGO
ifndef ERROR
include src/srpc/srpc.mk
include src/protocols/protocols.mk
include src/services/services.mk
include src/common/common.mk
include src/mid_hermes/midHermes.mk
include src/hermes/hermes.mk
endif


all: err srpc_shared protocols_shared common_shared services_shared midHermes_shared

test_all: err test

common_static: $(COMMON_OBJ)
	$(AR) rcs $(BIN_PATH)/lib$(COMMON_BIN).a $(COMMON_OBJ)

common_shared: $(COMMON_OBJ)
	$(CC) -shared -o $(BIN_PATH)/lib$(COMMON_BIN).$(SHARED_EXT) $(COMMON_OBJ) $(LDFLAGS)

db_static: $(DB_OBJ)
	$(AR) rcs $(BIN_PATH)/lib$(DB_BIN).a $(DB_OBJ)

db_shared: $(DB_OBJ)
	$(CC) -shared -o $(BIN_PATH)/lib$(DB_BIN).$(SHARED_EXT) $(DB_OBJ) $(LDFLAGS)

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
	$(CC) -shared -o $(BIN_PATH)/lib$(MIDHERMES_BIN).$(SHARED_EXT) $(MIDHERMES_OBJ) $(LDFLAGS) 

hermes: services_static services_shared $(HERMES_OBJ)
	$(CC) -o $(BIN_PATH)/$(HERMES_BIN) $(HERMES_OBJ) $(LDFLAGS) `pkg-config --libs libmongoc-1.0` -lzmq -lczmq -lc -lthemis

 
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
	mkdir -p $(PREFIX)/include/hermes $(PREFIX)/include/hermes/srpc $(PREFIX)/lib
	install $(SRC_PATH)/srpc/*.h $(PREFIX)/include/hermes/srpc
	install $(BIN_PATH)/*.a $(PREFIX)/lib
	install $(BIN_PATH)/*.$(SHARED_EXT) $(PREFIX)/lib

uninstall: 
	rm -rf $(PREFIX)/include/hermes
	rm -f $(PREFIX)/lib/libsrpc.a
	rm -f $(PREFIX)/lib/libsrpc.a
