#include <themis/themis.h>


int main(int argc, char* argv[]){
    uint8_t private_key[10240];
    uint8_t public_key[10240];
    size_t private_key_length=10240;
    size_t public_key_length=10240;
    themis_status_t res = themis_gen_ec_key_pair(private_key, &private_key_length, public_key, &public_key_length);
    if(res!=THEMIS_SUCCESS)
	return -1;
    if(argc ==2){
	if(0==strcmp(argv[1], "-c")){
	    int i=0;
	    fprintf(stdout, "private key:");
	    for(;i<private_key_length;++i)
		fprintf(stdout, "\\x%02x", private_key[i]);
	    fprintf(stdout, "\n");
	    fprintf(stderr, "public key :");
	    for(i=0;i<public_key_length;++i)
		fprintf(stderr, "\\x%02x", public_key[i]);
	    fprintf(stderr, "\n");
	    return 1;
	} else if(0==strcmp(argv[1], "-b")){
	    int i=0;
	    for(;i<private_key_length;++i)
		fprintf(stdout, "%c", private_key[i]);
	    for(i=0;i<public_key_length;++i)
		fprintf(stderr, "%c", public_key[i]);
	    return 1;
	}
    }
    fprintf(stdout, "usage: %s -[c|b]\n   -b - in binary format\n   -c - in C hex format (like \x02\x03)\nNote: application output private & public key to STDOUT and STDERR respectively\n", argv[0]);
    return -11;
}