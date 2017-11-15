all:
	@gcc -g -I../../../../../include -L../../../../../build  \
	main.c \
	../common/transport.c \
    ../common/credential_store_client.c \
	db.c \
	../../utils/utils.c \
	../../utils/base64.c \
	-lhermes_key_store \
	-lhermes_rpc \
	-lpthread \
	-lhermes_common \
	-lhermes_credential_store \
	-lhermes_secure_transport \
	-lthemis \
    -lsoter \
    $(CFLAGS) \
	-o ks

clean:
	@rm ./ks