#
# Copyright (c) 2015 Cossack Labs Limited
#

FILE_DB_SRC = $(wildcard $(SRC_PATH)/db/file_db/*.c)

FILE_DB_OBJ = $(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o, $(FILE_DB_SRC))

FILE_DB_BIN = file_db
