#include <hermes/crypt.h>
#include <themis/themis.h>


int main(){
    uint8_t private_key[10240];
    uint8_t public_key[10240];
    size_t private_key_length=10240;
    size_t public_key_length=10240;

    uint8_t peer_private_key[10240];
    uint8_t peer_public_key[10240];
    size_t peer_private_key_length=10240;
    size_t peer_public_key_length=10240;

    char data[]="This is test message";

    themis_status_t res = themis_gen_ec_key_pair(private_key, &private_key_length, public_key, &public_key_length);
    if(res!=THEMIS_SUCCESS)
	return -1;

    res = themis_gen_ec_key_pair(peer_private_key, &peer_private_key_length, peer_public_key, &peer_public_key_length);
    if(res!=THEMIS_SUCCESS)
	return -1;

    hermes_crypter_t* crypt = hermes_crypter_create(private_key, private_key_length);
    if(!crypt)
	return -1;

    hermes_crypter_t* crypt2 = hermes_crypter_create(peer_private_key, peer_private_key_length);
    if(!crypt)
	return -1;

    uint8_t *enc_data, *dec_data;
    size_t enc_data_len, dec_data_len;
    int err = hermes_crypter_encrypt(crypt, peer_public_key, peer_public_key_length, data, strlen(data)+1, &enc_data, &enc_data_len);
    if(err){
	hermes_crypter_destroy(&crypt);
	hermes_crypter_destroy(&crypt2);
	return -1;
    }


    err = hermes_crypter_decrypt(crypt2, public_key, public_key_length, enc_data, enc_data_len, &dec_data, &dec_data_len);
    if(err){
	free(enc_data);
	hermes_crypter_destroy(&crypt);
	hermes_crypter_destroy(&crypt2);
	return -1;
    }
    free(enc_data);
    printf("%s\n", (char*)dec_data);
    free(dec_data);

    uint8_t* token=NULL;
    size_t token_length=0;

    err = hermes_crypter_encrypt_with_creating_token(crypt, peer_public_key, peer_public_key_length, data, strlen(data)+1, &enc_data, &enc_data_len, &token, &token_length);
    if(err){
	hermes_crypter_destroy(&crypt);
	hermes_crypter_destroy(&crypt2);
	return -1;
    }

    err = hermes_crypter_decrypt_with_token(crypt2, public_key, public_key_length, token, token_length, enc_data, enc_data_len, &dec_data, &dec_data_len);
    if(err){
	hermes_crypter_destroy(&crypt);
	hermes_crypter_destroy(&crypt2);
	free(token);
	free(enc_data);
	return -1;
    }
    free(token);
    free(enc_data);
    printf("%s\n", (char*)dec_data);
    free(dec_data);

    uint8_t* mac=NULL, *mac2=NULL;
    size_t mac_length=0, mac2_length=0;

    err = hermes_crypter_mac_with_creating_token(crypt, peer_public_key, peer_public_key_length, data, strlen(data)+1, &mac, &mac_length, &token, &token_length);
    if(err){
	hermes_crypter_destroy(&crypt);
	hermes_crypter_destroy(&crypt2);
	return -1;
    }

    err = hermes_crypter_mac_with_token(crypt2, public_key, public_key_length, token, token_length, data, strlen(data)+1, &mac2, &mac2_length);
    if(err){
	hermes_crypter_destroy(&crypt);
	hermes_crypter_destroy(&crypt2);
	free(token);
	free(enc_data);
	return -1;
    }
    free(token);
    if(mac_length!=mac2_length || 0!=memcmp(mac, mac2, mac_length))
        printf("mac not match\n");
    else
        printf("mac match\n");

    free(mac);
    free(mac2);

    hermes_crypter_destroy(&crypt);
    hermes_crypter_destroy(&crypt2);
    return 1;
}