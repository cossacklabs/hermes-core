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

typedef enum {
  ENCRYPT = 26040001,
  DECRYPT,
  SIGN,
  VERIFY,
  EXIT,
  EXITED
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
  HERMES_CHECK(*data_length == read(pipe, *data, *data_length), free(*data); return false);
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
      size_t key_length, data_length, res_data_length;
      uint8_t *key, *data, *res_data;
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

int hermes_crypter_sign(hermes_crypter_t* crypter, const uint8_t* data, const size_t data_length, uint8_t** signed_data, size_t* signed_data_length){

}

int hermes_crypter_verify(hermes_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* signed_data, const size_t signed_data_length, uint8_t** data, size_t* data_length){

}
