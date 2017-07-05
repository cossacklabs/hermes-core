#include <themis/themis.h>
#include <stdio.h>
int main(int argc, char* argv[]){
    if(argc!=3){
	printf("Usage:\n    %s <private key file name> <public key file name>\n", argv[0]);
	return 1;
    }
    uint8_t* private_key=NULL;
    size_t private_key_length=0;
    uint8_t* public_key=NULL;
    size_t public_key_length=0;
/*    if(themis_gen_key_pair(THEMIS_SECURE_SIGNED_MESSAGE_DEFAULT_ALG, private_key, &private_key_length, public_key, &public_key_length)!=THEMIS_BUFFER_TOO_SMALL){
	fprintf(stderr, "key generation error 1\n");
	return 1;
    }
*/
    if(themis_gen_ec_key_pair(private_key, &private_key_length, public_key, &public_key_length)!=THEMIS_BUFFER_TOO_SMALL){
	fprintf(stderr, "key generation error 1\n");
	return 1;
    }
    private_key=malloc(private_key_length);
    public_key=malloc(public_key_length);
    if(!private_key || !public_key){return 2;}
    if(themis_gen_ec_key_pair(private_key, &private_key_length, public_key, &public_key_length)!=THEMIS_SUCCESS){
	fprintf(stderr, "key generation error 1\n");
	return 1;
    }
/*    if(themis_gen_key_pair(THEMIS_SECURE_SIGNED_MESSAGE_DEFAULT_ALG, private_key, &private_key_length, public_key, &public_key_length)!=THEMIS_SUCCESS){
	fprintf(stderr, "key generation error\n");
	return 3;
    }
*/
    int i;
    FILE* pr=fopen(argv[1], "w");
    if(!pr){
	fprintf(stderr, "can`t open file: %s\n", argv[1]);
	return 4;
    }
    fwrite(private_key, 1, private_key_length, pr);
    fclose(pr);
    FILE* pu=fopen(argv[2], "w");
    if(!pu){
	fprintf(stderr, "can`t open file: %s\n", argv[2]);
	return 4;
    }
    fwrite(public_key, 1, public_key_length, pu);
    fclose(pu);
    for(i=0;i<private_key_length;++i){
	fprintf(stdout, "0x%02x, ", private_key[i]);
//	fprintf(stdout, "%c", private_key[i]);
    }
    fprintf(stdout, "\n");
    for(i=0;i<public_key_length;++i){
	fprintf(stderr, "0x%02x, ", public_key[i]);
//	fprintf(stderr, "%c", public_key[i]);
    }
    free(private_key);
    free(public_key);
    return 0;
}