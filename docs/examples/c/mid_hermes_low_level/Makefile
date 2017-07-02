SRC_PATH = .
OBJ_PATH = ./obj

SRC = $(wildcard $(SRC_PATH)/*.c)
OBJ = $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRC))

UTILS_SRC_PATH = $(SRC_PATH)/../utils
UTILS_OBJ_PATH = $(SRC_PATH)/obj

UTILS_SRC = $(wildcard $(UTILS_SRC_PATH)/*.c)
UTILS_OBJ = $(patsubst $(UTILS_SRC_PATH)/%.c, $(UTILS_OBJ_PATH)/%.o, $(UTILS_SRC))

CFLAGS += -I../../../../include
LDFLAGS += -L../../../../build -lsoter -lmid_hermes_ll -lthemis

all: $(UTILS_OBJ) $(OBJ) 
	$(CC) -g $(OBJ) $(UTILS_OBJ) $(LDFLAGS) -o hermes_client_ll

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	mkdir -p $(OBJ_PATH)
	$(CC) $(CFLAGS) -g -c $< -o $@

$(UTILS_OBJ_PATH)/%.o: $(UTILS_SRC_PATH)/%.c
	mkdir -p $(UTILS_OBJ_PATH)
	$(CC) $(CFLAGS) -g -c $< -o $@

clean:
	rm -rf $(UTILS_OBJ_PATH)
	rm -rf $(OBJ_PATH)
	rm -f hermes_client_ll
