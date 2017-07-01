SRC_PATH = .
OBJ_PATH = .

SRC = $(wildcard $(SRC_PATH)/*.c)
OBJ =  $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRC))

CFLAGS += -I../../../include
LDFLAGS += -L../../../build -lsoter -lmid_hermes -lcredential_store -lrpc -lkey_store -ldata_store -lthemis

all: $(OBJ)
	$(CC) -g $(OBJ) $(LDFLAGS) -o hermes_client_ll

%.o: %.c
	$(CC) $(CFLAGS) -g -c $< -o $@

clean:
	rm -f *.o
	rm -f hermes_client_ll
