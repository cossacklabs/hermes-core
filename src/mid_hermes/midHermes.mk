#
# Copyright (c) 2015 Cossack Labs Limited
#

MIDHERMES_SRC = $(wildcard $(SRC_PATH)/mid_hermes/*.c)

MIDHERMES_OBJ = $(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o, $(MIDHERMES_SRC))

MIDHERMES_BIN = mid_hermes