#
# Copyright (c) 2015 Cossack Labs Limited
#

COMMON_SRC = $(wildcard $(SRC_PATH)/common/*.c)

COMMON_OBJ = $(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o, $(COMMON_SRC))

COMMON_BIN = common
