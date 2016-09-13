#
# Copyright (c) 2015 Cossack Labs Limited
#

SERVICES_SRC = $(wildcard $(SRC_PATH)/services/*.c)

SERVICES_OBJ = $(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o, $(SERVICES_SRC))

SERVICES_BIN = services

LDFLAGS+= -lmongoc-1.0