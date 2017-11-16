/*
* Copyright (c) 2017 Cossack Labs Limited
*
* This file is a part of Hermes-core.
*
* Hermes-core is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Hermes-core is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Hermes-core.  If not, see <http://www.gnu.org/licenses/>.
*
*/


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
    free(private_key);
    free(public_key);
    return 0;
}
