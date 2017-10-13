SECURE_TRANSPORT_SRC = $(wildcard $(SRC_PATH)/secure_transport/*.c)

SECURE_TRANSPORT_OBJ = $(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o, $(SECURE_TRANSPORT_SRC))

SECURE_TRANSPORT_BIN = hermes_secure_transport
