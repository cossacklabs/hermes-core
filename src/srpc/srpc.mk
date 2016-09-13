#
# Copyright (c) 2015 Cossack Labs Limited
#

SRPC_SRC = $(wildcard $(SRC_PATH)/srpc/*.c)

SRPC_OBJ = $(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o, $(SRPC_SRC))

SRPC_BIN = srpc

LDFLAGS+= -lpthread