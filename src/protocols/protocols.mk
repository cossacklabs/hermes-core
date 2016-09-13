#
# Copyright (c) 2015 Cossack Labs Limited
#

PROTOCOLS_SRC = $(wildcard $(SRC_PATH)/protocols/*.c)

PROTOCOLS_OBJ = $(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o, $(PROTOCOLS_SRC))

PROTOCOLS_BIN = protocols