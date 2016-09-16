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
#include <hermes/utils.h>
#include <hermes/crypt.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define HERMES_TOKEN_LENGTH 64
#define HERMES_MAC_LENGTH 16
#define HERMES_DEFAULT_CTX "hermes default context"
#define HERMES_DEFAULT_CTX_LENGTH strlen(HERMES_DEFAULT_CTX)


typedef enum {
  ENCRYPT = 26040001,
  ENCRYPT_WITH_TOKEN,
  ENCRYPT_WITH_CREATING_TOKEN,
  MAC_WITH_TOKEN,
  MAC_WITH_CREATING_TOKEN,
  DECRYPT,
  DECRYPT_WITH_TOKEN,
  SIGN,
  VERIFY,
  EXIT,
  EXITED,
  CREATE_TOKEN_FROM_TOKEN
}encrypter_command;

struct hermes_crypter_t_{
  int crypt_reader_pipe[2];
  int crypt_writer_pipe[2];
  pid_t crypter_pid;
};

bool write_data_to_pipe(int pipe, const uint8_t* data, const uint32_t data_length){
  HERMES_CHECK(sizeof(uint32_t)==write(pipe, &data_length, sizeof(uint32_t)), return false);
  HERMES_CHECK(data_length==write(pipe, data, data_length), return false);
  return true;
}

bool read_data_from_pipe(int pipe, uint8_t** data, uint32_t* data_length){
  HERMES_CHECK(sizeof(uint32_t) == read(pipe, data_length, sizeof(uint32_t)), return false);
  *data=malloc(*data_length);
  HERMES_CHECK(*data, return false);
  HERMES_CHECK(*data_length == read(pipe, *data, *data_length), free(*data); *data=NULL; return false);
  return true;
}

hermes_crypter_t* hermes_crypter_create(const uint8_t* private_key, const size_t private_key_length){
  hermes_crypter_t* crypter = calloc(sizeof(hermes_crypter_t),1);
  HERMES_CHECK(crypter, return NULL);
  HERMES_CHECK(0 == pipe(crypter->crypt_reader_pipe), hermes_crypter_destroy(&crypter); return NULL);
  HERMES_CHECK(0 == pipe(crypter->crypt_writer_pipe), hermes_crypter_destroy(&crypter); return NULL);
  crypter->crypter_pid = fork();
  HERMES_CHECK(-1 != crypter->crypter_pid, hermes_crypter_destroy(&crypter); return NULL);
  if(0 == crypter->crypter_pid){
    HERMES_LOG("encrypter", "process started");
    close(crypter->crypt_reader_pipe[1]);
    close(crypter->crypt_writer_pipe[0]);
    uint32_t command=0;
    uint8_t* buf;
    while(true){
      ssize_t a=read(crypter->crypt_reader_pipe[0], &command, sizeof(uint32_t));
      HERMES_CHECK(sizeof(uint32_t) == a, _exit(EXIT_FAILURE));
      size_t key_length=0, data_length=0, res_data_length=0, token_length=0, token_key_length=HERMES_TOKEN_LENGTH, new_key_length=0;
      uint8_t *key=NULL, *data=NULL, *res_data=NULL, *token=NULL, *new_key=NULL;
      uint8_t token_key[HERMES_TOKEN_LENGTH];
      switch(command){
      case EXIT:
	command = EXITED;
	HERMES_CHECK(sizeof(uint32_t)==write(crypter->crypt_writer_pipe[1], &command, sizeof(uint32_t)), _exit(EXIT_FAILURE));
	HERMES_LOG("encrypter", "process stoped");
	_exit(EXIT_SUCCESS);
      case ENCRYPT:
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &key, (uint32_t*)(&key_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &data, (uint32_t*)(&data_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_message_wrap(private_key, private_key_length, key, key_length, data, data_length, NULL, &res_data_length), _exit(EXIT_FAILURE));
	res_data=malloc(res_data_length);
	HERMES_CHECK(res_data && THEMIS_SUCCESS == themis_secure_message_wrap(private_key, private_key_length, key, key_length, data, data_length, res_data, &res_data_length), _exit(EXIT_FAILURE));
	free(key);
	free(data);
	HERMES_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], res_data, (uint32_t)res_data_length), _exit(EXIT_FAILURE));
	free(res_data);
	HERMES_LOG("encrypter", "encryption complete");
	break;
      case DECRYPT:
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &key, (uint32_t*)(&key_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &data, (uint32_t*)(&data_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_message_unwrap(private_key, private_key_length, key, key_length, data, data_length, NULL, &res_data_length), _exit(EXIT_FAILURE));
	res_data=malloc(res_data_length);
	HERMES_CHECK(res_data && THEMIS_SUCCESS == themis_secure_message_unwrap(private_key, private_key_length, key, key_length, data, data_length, res_data, &res_data_length), _exit(EXIT_FAILURE));
	free(key);
	free(data);
	HERMES_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], res_data, (uint32_t)res_data_length), _exit(EXIT_FAILURE));
	free(res_data);
	HERMES_LOG("encrypter", "decryption complete");
	break;
      case ENCRYPT_WITH_TOKEN:
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &key, (uint32_t*)(&key_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &data, (uint32_t*)(&data_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &token, (uint32_t*)(&token_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(THEMIS_SUCCESS == themis_secure_message_unwrap(private_key, private_key_length, key, key_length, token, token_length, token_key, &token_key_length) && token_key_length == HERMES_TOKEN_LENGTH, _exit(EXIT_FAILURE));
	free(key);
	free(token);
	HERMES_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_cell_encrypt_seal(token_key, token_key_length, NULL, 0, data, data_length, NULL, &res_data_length), _exit(EXIT_FAILURE));
	res_data=malloc(res_data_length);
	HERMES_CHECK(res_data && THEMIS_SUCCESS == themis_secure_cell_encrypt_seal(token_key, token_key_length, NULL, 0, data, data_length, res_data, &res_data_length), _exit(EXIT_FAILURE));
	free(data);
	HERMES_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], res_data, (uint32_t)res_data_length), _exit(EXIT_FAILURE));
	free(res_data);
	HERMES_LOG("encrypter", "encryption with token complete");
	break;
      case DECRYPT_WITH_TOKEN:
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &key, (uint32_t*)(&key_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &data, (uint32_t*)(&data_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &token, (uint32_t*)(&token_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(THEMIS_SUCCESS == themis_secure_message_unwrap(private_key, private_key_length, key, key_length, token, token_length, token_key, &token_key_length) && token_key_length == HERMES_TOKEN_LENGTH, _exit(EXIT_FAILURE));
	free(key);
	free(token);
	HERMES_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_cell_decrypt_seal(token_key, token_key_length, NULL, 0, data, data_length, NULL, &res_data_length), _exit(EXIT_FAILURE));
	res_data=malloc(res_data_length);
	HERMES_CHECK(res_data && THEMIS_SUCCESS == themis_secure_cell_decrypt_seal(token_key, token_key_length, NULL, 0, data, data_length, res_data, &res_data_length), _exit(EXIT_FAILURE));
	free(data);
	HERMES_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], res_data, (uint32_t)res_data_length), _exit(EXIT_FAILURE));
	free(res_data);
	HERMES_LOG("encrypter", "decryption with token complete");
	break;
      case ENCRYPT_WITH_CREATING_TOKEN:
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &key, (uint32_t*)(&key_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &data, (uint32_t*)(&data_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(THEMIS_SUCCESS == soter_rand(token_key, token_key_length), _exit(EXIT_FAILURE));
	HERMES_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_cell_encrypt_seal(token_key, token_key_length, NULL, 0, data, data_length, NULL, &res_data_length), _exit(EXIT_FAILURE));
	res_data=malloc(res_data_length);
	HERMES_CHECK(res_data && THEMIS_SUCCESS == themis_secure_cell_encrypt_seal(token_key, token_key_length, NULL, 0, data, data_length, res_data, &res_data_length), _exit(EXIT_FAILURE));
	free(data);
	HERMES_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_message_wrap(private_key, private_key_length, key, key_length, token_key, token_key_length, NULL, &token_length), _exit(EXIT_FAILURE));
	token = malloc(token_length);
	HERMES_CHECK(token && THEMIS_SUCCESS == themis_secure_message_wrap(private_key, private_key_length, key, key_length, token_key, token_key_length, token, &token_length), _exit(EXIT_FAILURE));
	free(key);
	HERMES_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], res_data, (uint32_t)res_data_length), _exit(EXIT_FAILURE));
	free(res_data);
	HERMES_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], token, (uint32_t)token_length), _exit(EXIT_FAILURE));
	free(token);
	HERMES_LOG("encrypter", "encryption with creating token complete");
	break;
      case MAC_WITH_TOKEN:
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &key, (uint32_t*)(&key_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &data, (uint32_t*)(&data_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &token, (uint32_t*)(&token_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(THEMIS_SUCCESS == themis_secure_message_unwrap(private_key, private_key_length, key, key_length, token, token_length, token_key, &token_key_length) && token_key_length == HERMES_TOKEN_LENGTH, _exit(EXIT_FAILURE));
	free(key);
	free(token);
	HERMES_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_cell_encrypt_context_imprint(token_key, token_key_length, data, data_length, HERMES_DEFAULT_CTX, HERMES_DEFAULT_CTX_LENGTH, NULL, &res_data_length), _exit(EXIT_FAILURE));
	res_data=malloc(res_data_length);
	HERMES_CHECK(res_data && THEMIS_SUCCESS == themis_secure_cell_encrypt_context_imprint(token_key, token_key_length, data, data_length, HERMES_DEFAULT_CTX, HERMES_DEFAULT_CTX_LENGTH, res_data, &res_data_length), _exit(EXIT_FAILURE));
	free(data);
	HERMES_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], res_data+res_data_length-HERMES_MAC_LENGTH, HERMES_MAC_LENGTH), _exit(EXIT_FAILURE));
	free(res_data);
	HERMES_LOG("encrypter", "mac with token complete");
	break;
      case MAC_WITH_CREATING_TOKEN:
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &key, (uint32_t*)(&key_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &data, (uint32_t*)(&data_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(THEMIS_SUCCESS == soter_rand(token_key, token_key_length), _exit(EXIT_FAILURE));
	HERMES_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_cell_encrypt_context_imprint(token_key, token_key_length, data, data_length, HERMES_DEFAULT_CTX, HERMES_DEFAULT_CTX_LENGTH, NULL, &res_data_length), _exit(EXIT_FAILURE));
	res_data=malloc(res_data_length);
	HERMES_CHECK(res_data && THEMIS_SUCCESS == themis_secure_cell_encrypt_context_imprint(token_key, token_key_length, data, data_length, HERMES_DEFAULT_CTX, HERMES_DEFAULT_CTX_LENGTH, res_data, &res_data_length), _exit(EXIT_FAILURE));
	free(data);
	HERMES_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_message_wrap(private_key, private_key_length, key, key_length, token_key, token_key_length, NULL, &token_length), _exit(EXIT_FAILURE));
	token = malloc(token_length);
	HERMES_CHECK(token && THEMIS_SUCCESS == themis_secure_message_wrap(private_key, private_key_length, key, key_length, token_key, token_key_length, token, &token_length), _exit(EXIT_FAILURE));
	free(key);
	HERMES_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], res_data+res_data_length-HERMES_MAC_LENGTH, HERMES_MAC_LENGTH), _exit(EXIT_FAILURE));
	free(res_data);
	HERMES_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], token, (uint32_t)token_length), _exit(EXIT_FAILURE));
	free(token);
	HERMES_LOG("encrypter", "mac with creating token complete");
	break;
      case CREATE_TOKEN_FROM_TOKEN:
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &key, (uint32_t*)(&key_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &new_key, (uint32_t*)(&new_key_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &token, (uint32_t*)(&token_length)), _exit(EXIT_FAILURE));
	HERMES_CHECK(THEMIS_SUCCESS == themis_secure_message_unwrap(private_key, private_key_length, key, key_length, token, token_length, token_key, &token_key_length) && token_key_length == HERMES_TOKEN_LENGTH, _exit(EXIT_FAILURE));
	free(key);
	free(token);
	HERMES_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_message_wrap(private_key, private_key_length, new_key, new_key_length, token_key, token_key_length, NULL, &token_length), _exit(EXIT_FAILURE));
	token = malloc(token_length);
	HERMES_CHECK(token && THEMIS_SUCCESS == themis_secure_message_wrap(private_key, private_key_length, new_key, new_key_length, token_key, token_key_length, token, &token_length), _exit(EXIT_FAILURE));
	free(new_key);
	HERMES_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], token, (uint32_t)token_length), _exit(EXIT_FAILURE));
	free(token);
	HERMES_LOG("encrypter", "creating token from token complete");	
	break;
      case SIGN:
      case VERIFY:
      default:
	break;
      }
    }
    close(crypter->crypt_reader_pipe[0]);
    close(crypter->crypt_writer_pipe[1]);
    _exit(EXIT_SUCCESS);
  }else{
    close(crypter->crypt_reader_pipe[0]);
    close(crypter->crypt_writer_pipe[1]);
  }
  return crypter;
}

void hermes_crypter_destroy(hermes_crypter_t** crypter){
  uint32_t command = EXIT;
  HERMES_CHECK(sizeof(uint32_t)==write((*crypter)->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return);
  HERMES_CHECK(sizeof(uint32_t) == read((*crypter)->crypt_writer_pipe[0], &command, sizeof(uint32_t)), return);
  close((*crypter)->crypt_reader_pipe[1]);
  close((*crypter)->crypt_writer_pipe[0]);
  free(*crypter);
  return;
}

int hermes_crypter_encrypt(hermes_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* data, const size_t data_length, uint8_t** encrypted_data, size_t* encrypted_data_length){
  uint32_t command = ENCRYPT;
  HERMES_CHECK(sizeof(uint32_t)==write(crypter->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], public_key, (uint32_t)public_key_length),  return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], data, (uint32_t)data_length),  return -1);
  HERMES_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], encrypted_data, (uint32_t*)(encrypted_data_length)), return -1);
  return 0;
}

int hermes_crypter_decrypt(hermes_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* encrypted_data, const size_t encrypted_data_length, uint8_t** data, size_t* data_length){
  uint32_t command = DECRYPT;
  HERMES_CHECK(sizeof(uint32_t)==write(crypter->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], public_key, (uint32_t)public_key_length),  return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], encrypted_data, (uint32_t)encrypted_data_length),  return -1);
  HERMES_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], data, (uint32_t*)(data_length)), return -1);
  return 0;
}

int hermes_crypter_encrypt_with_token(hermes_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* token, const size_t token_length, const uint8_t* data, const size_t data_length, uint8_t** encrypted_data, size_t* encrypted_data_length){
  uint32_t command = ENCRYPT_WITH_TOKEN;
  HERMES_CHECK(sizeof(uint32_t)==write(crypter->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], public_key, (uint32_t)public_key_length),  return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], data, (uint32_t)data_length),  return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], token, (uint32_t)token_length),  return -1);
  HERMES_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], encrypted_data, (uint32_t*)(encrypted_data_length)), return -1);
  return 0;
}

int hermes_crypter_encrypt_with_creating_token(hermes_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* data, const size_t data_length, uint8_t** encrypted_data, size_t* encrypted_data_length, uint8_t** token, size_t* token_length){
  uint32_t command = ENCRYPT_WITH_CREATING_TOKEN;
  HERMES_CHECK(sizeof(uint32_t)==write(crypter->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], public_key, (uint32_t)public_key_length),  return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], data, (uint32_t)data_length),  return -1);
  HERMES_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], encrypted_data, (uint32_t*)(encrypted_data_length)), return -1);
  HERMES_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], token, (uint32_t*)(token_length)), return -1);
  return 0;
}

int hermes_crypter_decrypt_with_token(hermes_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* token, const size_t token_length, const uint8_t* encrypted_data, const size_t encrypted_data_length, uint8_t** data, size_t* data_length){
  uint32_t command = DECRYPT_WITH_TOKEN;
  HERMES_CHECK(sizeof(uint32_t)==write(crypter->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], public_key, (uint32_t)public_key_length),  return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], encrypted_data, (uint32_t)encrypted_data_length),  return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], token, (uint32_t)token_length),  return -1);
  HERMES_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], data, (uint32_t*)(data_length)), return -1);
  return 0;
}

int hermes_crypter_mac_with_token(hermes_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* token, const size_t token_length, const uint8_t* data, const size_t data_length, uint8_t** mac, size_t* mac_length){
  uint32_t command = MAC_WITH_TOKEN;
  HERMES_CHECK(sizeof(uint32_t)==write(crypter->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], public_key, (uint32_t)public_key_length),  return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], data, (uint32_t)data_length),  return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], token, (uint32_t)token_length),  return -1);
  HERMES_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], mac, (uint32_t*)(mac_length)), return -1);
  return 0;
}

int hermes_crypter_mac_with_creating_token(hermes_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* data, const size_t data_length, uint8_t** mac, size_t* mac_length, uint8_t** token, size_t* token_length){
  uint32_t command = MAC_WITH_CREATING_TOKEN;
  HERMES_CHECK(sizeof(uint32_t)==write(crypter->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], public_key, (uint32_t)public_key_length),  return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], data, (uint32_t)data_length),  return -1);
  HERMES_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], mac, (uint32_t*)(mac_length)), return -1);
  HERMES_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], token, (uint32_t*)(token_length)), return -1);
  return 0;
}

int hermes_crypter_create_token_from_token(hermes_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* new_public_key, const size_t new_public_key_length, const uint8_t* token, const size_t token_length, uint8_t** new_token, size_t* new_token_length){
  uint32_t command = CREATE_TOKEN_FROM_TOKEN;
  HERMES_CHECK(sizeof(uint32_t)==write(crypter->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], public_key, (uint32_t)public_key_length),  return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], new_public_key, (uint32_t)new_public_key_length),  return -1);
  HERMES_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], token, (uint32_t)token_length),  return -1);
  HERMES_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], new_token, (uint32_t*)(new_token_length)), return -1);
  return 0;  
}

int hermes_crypter_sign(hermes_crypter_t* crypter, const uint8_t* data, const size_t data_length, uint8_t** signed_data, size_t* signed_data_length){
  return -1;
}

int hermes_crypter_verify(hermes_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* signed_data, const size_t signed_data_length, uint8_t** data, size_t* data_length){
  return -1;
}
