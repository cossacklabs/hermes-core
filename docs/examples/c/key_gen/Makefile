SRC_PATH = .
OBJ_PATH = ./obj

SRC = $(wildcard $(SRC_PATH)/*.c)
OBJ = $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRC))

LDFLAGS += -lsoter -lthemis

all: $(OBJ) 
	$(CC) -g $(OBJ) $(LDFLAGS) -o key_pair_gen

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	mkdir -p $(OBJ_PATH)
	$(CC) $(CFLAGS) -g -c $< -o $@

clean:
	rm -rf $(OBJ_PATH)
	rm -f key_pair_gen
