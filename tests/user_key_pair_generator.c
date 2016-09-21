/*
 * Copyright (c) 2016 Cossack Labs Limited
 *
 * This file is part of Hermes.
 *
 * Hermes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hermes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Hermes.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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