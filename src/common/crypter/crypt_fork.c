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
#include <hermes/common/errors.h>
#include <hermes/common/crypt.h>
#include <hermes/client_interfaces/key_storage_interface.h>
#include "crypt_impl.h"
#include "utils/pipe.h"

#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


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

struct hm_crypter_t_{
  int crypt_reader_pipe[2];
  int crypt_writer_pipe[2];
  pid_t crypter_pid;
};

int simple_crypter_proccess(const uint8_t* id, const size_t id_length, int read_pipe, int write_pipe){
  if(!read_pipe || !write_pipe){
    return HM_INVALID_PARAM;
  }
  hm_keys_storage_t* key_store=hm_keys_storage_create();
  if(!key_store){
    return HM_FAIL;
  }
  uint8_t* key=NULL;
  size_t key_length=0;
  if(HM_SUCCESS!=hm_keys_storage_get_private_key_by_id_(id, id_length, &key, &res_key_length)){
    return HM_FAIL;
  }
  hm_crypter_impl_t* crypter = hm_crypter_impl_create(id, id_length, key, key_length);
  free(key);
  if(!crypter){
    return HM_FAIL;
  }
  uint64_t command=0;
  int res=HM_SUCCESS;
  while(true){
    if(HM_SUCCESS==HM_SC_READ(crypter->crypt_reader_pipe[0], HM_SC_INT(&command))){ //read command id
      res=HM_FAIL;
    }
    if(HM_SUCCESS==res){
      switch(command){
      case HM_SC_COMAND_EXIT:
        hm_crypter_impl_destroy(crypter);
        if(HM_SUCCESS!=HM_SC_WRITE(crypter->crypt_writer_pipe[1], HM_SC_INT(HM_SC_COMMAND_EXITED))){
          _exit(EXIT_FAILURE);
        }
        _exit(EXIT_SUCCESS);
      case HM_SC_COMMAND_ENCRYPT:{
        uint8_t *key=NULL, *data=NULL, *enc_data=NULL;
        uint32_t key_length=0, data_length=0, enc_data_len=0;
        if(HM_SUCCESS!=HM_SC_READ(crypter->crypt_reader_pipe[0]), HM_SC_PARAM_BUF(&key, &key_length), HM_SC_PARAM_BUF(&data, &data_length)){
          res=HM_FAIL;
        }
        int res=hm_crypter_impl_encrypt_(crypter, key, key_length, data, data_length, &enc_data, &enc_data_len);
        free(key);
        free(data);
        if(HM_SUCCESS!=res){
          if(HM_SUCCESS!=HM_SC_WRITE(crypter->crypt_writer_pipe[1], HM_SC_PARAM_INT(res))){
            res=HM_FAIL;
          }
        }else{
          if(HM_SUCCESS!=HM_SC_WRITE(crypter->crypt_writer_pipe[1], HM_SC_PARAM_INT(HM_SUCCESS), HM_SC_PARAM_BUF(enc_data, enc_data_len))){
            res=HM_FAIL;
          }
          free(enc_data);
        }
      }
        break;
      case HM_SC_COMMAND_DECRYPT:{
        uint8_t *key=NULL, *data=NULL, *dec_data=NULL;
        uint32_t key_length=0, data_length=0, dec_data_len=0;
        if(HM_SUCCESS!=HM_SC_READ(crypter->crypt_reader_pipe[0]), HM_SC_PARAM_BUF(&key, &key_length), HM_SC_PARAM_BUF(&data, &data_length)){
          hm_crypter_impl_destroy(crypter);
          _exit(EXIT_FAILURE);
        }
        int res=hm_crypter_impl_decrypt_(crypter, key, key_length, data, data_length, &dec_data, &dec_data_len);
        free(key);
        free(data);
        if(HM_SUCCESS!=res){
          if(HM_SUCCESS!=HM_SC_WRITE(crypter->crypt_writer_pipe[1], HM_SC_PARAM_INT(res))){
            hm_crypter_impl_destroy(crypter);
            _exit(EXIT_FAILURE);            
          }
        }else{
          if(HM_SUCCESS!=HM_SC_WRITE(crypter->crypt_writer_pipe[1], HM_SC_PARAM_INT(HM_SUCCESS), HM_SC_PARAM_BUF(dec_data, dec_data_len))){
            free(dec_data);
            hm_crypter_impl_destroy(crypter);
            _exit(EXIT_FAILURE);            
          }
          free(dec_data);
        }
      }
        break;
    }
    /* case ENCRYPT_WITH_TOKEN: */
    /*     HM_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &key, (uint32_t*)(&key_length)), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &data, (uint32_t*)(&data_length)), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &token, (uint32_t*)(&token_length)), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(THEMIS_SUCCESS == themis_secure_message_unwrap(private_key, private_key_length, key, key_length, token, token_length, token_key, &token_key_length) && token_key_length == HM_TOKEN_LENGTH, _exit(EXIT_FAILURE)); */
    /*     free(key); */
    /*     free(token); */
    /*     HM_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_cell_encrypt_seal(token_key, token_key_length, NULL, 0, data, data_length, NULL, &res_data_length), _exit(EXIT_FAILURE)); */
    /*     res_data=malloc(res_data_length); */
    /*     HM_CHECK(res_data && THEMIS_SUCCESS == themis_secure_cell_encrypt_seal(token_key, token_key_length, NULL, 0, data, data_length, res_data, &res_data_length), _exit(EXIT_FAILURE)); */
    /*     free(data); */
    /*     HM_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], res_data, (uint32_t)res_data_length), _exit(EXIT_FAILURE)); */
    /*     free(res_data); */
    /*     HM_LOG("encrypter", "encryption with token complete"); */
    /*     break; */
    /*   case DECRYPT_WITH_TOKEN: */
    /*     HM_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &key, (uint32_t*)(&key_length)), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &data, (uint32_t*)(&data_length)), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &token, (uint32_t*)(&token_length)), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(THEMIS_SUCCESS == themis_secure_message_unwrap(private_key, private_key_length, key, key_length, token, token_length, token_key, &token_key_length) && token_key_length == HM_TOKEN_LENGTH, _exit(EXIT_FAILURE)); */
    /*     free(key); */
    /*     free(token); */
    /*     HM_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_cell_decrypt_seal(token_key, token_key_length, NULL, 0, data, data_length, NULL, &res_data_length), _exit(EXIT_FAILURE)); */
    /*     res_data=malloc(res_data_length); */
    /*     HM_CHECK(res_data && THEMIS_SUCCESS == themis_secure_cell_decrypt_seal(token_key, token_key_length, NULL, 0, data, data_length, res_data, &res_data_length), _exit(EXIT_FAILURE)); */
    /*     free(data); */
    /*     HM_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], res_data, (uint32_t)res_data_length), _exit(EXIT_FAILURE)); */
    /*     free(res_data); */
    /*     HM_LOG("encrypter", "decryption with token complete"); */
    /*     break; */
    /*   case ENCRYPT_WITH_CREATING_TOKEN: */
    /*     HM_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &key, (uint32_t*)(&key_length)), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &data, (uint32_t*)(&data_length)), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(THEMIS_SUCCESS == soter_rand(token_key, token_key_length), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_cell_encrypt_seal(token_key, token_key_length, NULL, 0, data, data_length, NULL, &res_data_length), _exit(EXIT_FAILURE)); */
    /*     res_data=malloc(res_data_length); */
    /*     HM_CHECK(res_data && THEMIS_SUCCESS == themis_secure_cell_encrypt_seal(token_key, token_key_length, NULL, 0, data, data_length, res_data, &res_data_length), _exit(EXIT_FAILURE)); */
    /*     free(data); */
    /*     HM_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_message_wrap(private_key, private_key_length, key, key_length, token_key, token_key_length, NULL, &token_length), _exit(EXIT_FAILURE)); */
    /*     token = malloc(token_length); */
    /*     HM_CHECK(token && THEMIS_SUCCESS == themis_secure_message_wrap(private_key, private_key_length, key, key_length, token_key, token_key_length, token, &token_length), _exit(EXIT_FAILURE)); */
    /*     free(key); */
    /*     HM_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], res_data, (uint32_t)res_data_length), _exit(EXIT_FAILURE)); */
    /*     free(res_data); */
    /*     HM_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], token, (uint32_t)token_length), _exit(EXIT_FAILURE)); */
    /*     free(token); */
    /*     HM_LOG("encrypter", "encryption with creating token complete"); */
    /*     break; */
    /*   case MAC_WITH_TOKEN: */
    /*     HM_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &key, (uint32_t*)(&key_length)), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &data, (uint32_t*)(&data_length)), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &token, (uint32_t*)(&token_length)), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(THEMIS_SUCCESS == themis_secure_message_unwrap(private_key, private_key_length, key, key_length, token, token_length, token_key, &token_key_length) && token_key_length == HM_TOKEN_LENGTH, _exit(EXIT_FAILURE)); */
    /*     free(key); */
    /*     free(token); */
    /*     HM_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_cell_encrypt_context_imprint(token_key, token_key_length, data, data_length, HM_DEFAULT_CTX, HM_DEFAULT_CTX_LENGTH, NULL, &res_data_length), _exit(EXIT_FAILURE)); */
    /*     res_data=malloc(res_data_length); */
    /*     HM_CHECK(res_data && THEMIS_SUCCESS == themis_secure_cell_encrypt_context_imprint(token_key, token_key_length, data, data_length, HM_DEFAULT_CTX, HM_DEFAULT_CTX_LENGTH, res_data, &res_data_length), _exit(EXIT_FAILURE)); */
    /*     free(data); */
    /*     HM_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], res_data+res_data_length-HM_MAC_LENGTH, HM_MAC_LENGTH), _exit(EXIT_FAILURE)); */
    /*     free(res_data); */
    /*     HM_LOG("encrypter", "mac with token complete"); */
    /*     break; */
    /*   case MAC_WITH_CREATING_TOKEN: */
    /*     HM_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &key, (uint32_t*)(&key_length)), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &data, (uint32_t*)(&data_length)), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(THEMIS_SUCCESS == soter_rand(token_key, token_key_length), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_cell_encrypt_context_imprint(token_key, token_key_length, data, data_length, HM_DEFAULT_CTX, HM_DEFAULT_CTX_LENGTH, NULL, &res_data_length), _exit(EXIT_FAILURE)); */
    /*     res_data=malloc(res_data_length); */
    /*     HM_CHECK(res_data && THEMIS_SUCCESS == themis_secure_cell_encrypt_context_imprint(token_key, token_key_length, data, data_length, HM_DEFAULT_CTX, HM_DEFAULT_CTX_LENGTH, res_data, &res_data_length), _exit(EXIT_FAILURE)); */
    /*     free(data); */
    /*     HM_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_message_wrap(private_key, private_key_length, key, key_length, token_key, token_key_length, NULL, &token_length), _exit(EXIT_FAILURE)); */
    /*     token = malloc(token_length); */
    /*     HM_CHECK(token && THEMIS_SUCCESS == themis_secure_message_wrap(private_key, private_key_length, key, key_length, token_key, token_key_length, token, &token_length), _exit(EXIT_FAILURE)); */
    /*     free(key); */
    /*     HM_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], res_data+res_data_length-HM_MAC_LENGTH, HM_MAC_LENGTH), _exit(EXIT_FAILURE)); */
    /*     free(res_data); */
    /*     HM_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], token, (uint32_t)token_length), _exit(EXIT_FAILURE)); */
    /*     free(token); */
    /*     HM_LOG("encrypter", "mac with creating token complete"); */
    /*     break; */
    /*   case CREATE_TOKEN_FROM_TOKEN: */
    /*     HM_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &key, (uint32_t*)(&key_length)), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &new_key, (uint32_t*)(&new_key_length)), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(read_data_from_pipe(crypter->crypt_reader_pipe[0], &token, (uint32_t*)(&token_length)), _exit(EXIT_FAILURE)); */
    /*     HM_CHECK(THEMIS_SUCCESS == themis_secure_message_unwrap(private_key, private_key_length, key, key_length, token, token_length, token_key, &token_key_length) && token_key_length == HM_TOKEN_LENGTH, _exit(EXIT_FAILURE)); */
    /*     free(key); */
    /*     free(token); */
    /*     HM_CHECK(THEMIS_BUFFER_TOO_SMALL == themis_secure_message_wrap(private_key, private_key_length, new_key, new_key_length, token_key, token_key_length, NULL, &token_length), _exit(EXIT_FAILURE)); */
    /*     token = malloc(token_length); */
    /*     HM_CHECK(token && THEMIS_SUCCESS == themis_secure_message_wrap(private_key, private_key_length, new_key, new_key_length, token_key, token_key_length, token, &token_length), _exit(EXIT_FAILURE)); */
    /*     free(new_key); */
    /*     HM_CHECK(write_data_to_pipe(crypter->crypt_writer_pipe[1], token, (uint32_t)token_length), _exit(EXIT_FAILURE)); */
    /*     free(token); */
    /*     HM_LOG("encrypter", "creating token from token complete");	 */
    /*     break; */
    /*   case SIGN: */
    /*   case VERIFY: */
    /*   default: */
    /*     break; */
    /*   } */
    }


hm_crypter_t* hm_crypter_create(const uint8_t* id, const size_t id_length){
  hm_crypter_t* crypter = calloc(sizeof(hm_crypter_t),1);
  if(!crypter){
    return NULL;
  }
  if(0 != pipe(crypter->crypt_reader_pipe)){
    hm_crypter_destroy(&crypter);
    return NULL;
  }
  if(0 != pipe(crypter->crypt_writer_pipe)){
    hm_crypter_destroy(&crypter);
    return NULL;
  }
  crypter->crypter_pid = fork();
  if(-1 == crypter->crypter_pid){
    hm_crypter_destroy(&crypter);
    return NULL;
  }
  if(0 == crypter->crypter_pid){
    HM_LOG("encrypter", "process started");
    close(crypter->crypt_reader_pipe[1]);
    close(crypter->crypt_writer_pipe[0]);
    if(HM_SUCCESS!=simple_crypter_proccess(id, id_length, crypter->crypt_reader_pipe[0], crypter->crypt_writer_pipe[1])){
      HM_LOG_INFO("encrypter", "process stoped");      
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

void hm_crypter_destroy(hm_crypter_t** crypter){
  uint32_t command = EXIT;
  HM_CHECK(sizeof(uint32_t)==write((*crypter)->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return);
  HM_CHECK(sizeof(uint32_t) == read((*crypter)->crypt_writer_pipe[0], &command, sizeof(uint32_t)), return);
  close((*crypter)->crypt_reader_pipe[1]);
  close((*crypter)->crypt_writer_pipe[0]);
  free(*crypter);
  return;
}

int hm_crypter_encrypt(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* data, const size_t data_length, uint8_t** encrypted_data, size_t* encrypted_data_length){
  uint32_t command = ENCRYPT;
  HM_CHECK(sizeof(uint32_t)==write(crypter->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], public_key, (uint32_t)public_key_length),  return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], data, (uint32_t)data_length),  return -1);
  HM_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], encrypted_data, (uint32_t*)(encrypted_data_length)), return -1);
  return 0;
}

int hm_crypter_decrypt(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* encrypted_data, const size_t encrypted_data_length, uint8_t** data, size_t* data_length){
  uint32_t command = DECRYPT;
  HM_CHECK(sizeof(uint32_t)==write(crypter->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], public_key, (uint32_t)public_key_length),  return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], encrypted_data, (uint32_t)encrypted_data_length),  return -1);
  HM_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], data, (uint32_t*)(data_length)), return -1);
  return 0;
}

int hm_crypter_encrypt_with_token(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* token, const size_t token_length, const uint8_t* data, const size_t data_length, uint8_t** encrypted_data, size_t* encrypted_data_length){
  uint32_t command = ENCRYPT_WITH_TOKEN;
  HM_CHECK(sizeof(uint32_t)==write(crypter->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], public_key, (uint32_t)public_key_length),  return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], data, (uint32_t)data_length),  return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], token, (uint32_t)token_length),  return -1);
  HM_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], encrypted_data, (uint32_t*)(encrypted_data_length)), return -1);
  return 0;
}

int hm_crypter_encrypt_with_creating_token(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* data, const size_t data_length, uint8_t** encrypted_data, size_t* encrypted_data_length, uint8_t** token, size_t* token_length){
  uint32_t command = ENCRYPT_WITH_CREATING_TOKEN;
  HM_CHECK(sizeof(uint32_t)==write(crypter->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], public_key, (uint32_t)public_key_length),  return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], data, (uint32_t)data_length),  return -1);
  HM_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], encrypted_data, (uint32_t*)(encrypted_data_length)), return -1);
  HM_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], token, (uint32_t*)(token_length)), return -1);
  return 0;
}

int hm_crypter_decrypt_with_token(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* token, const size_t token_length, const uint8_t* encrypted_data, const size_t encrypted_data_length, uint8_t** data, size_t* data_length){
  uint32_t command = DECRYPT_WITH_TOKEN;
  HM_CHECK(sizeof(uint32_t)==write(crypter->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], public_key, (uint32_t)public_key_length),  return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], encrypted_data, (uint32_t)encrypted_data_length),  return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], token, (uint32_t)token_length),  return -1);
  HM_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], data, (uint32_t*)(data_length)), return -1);
  return 0;
}

int hm_crypter_mac_with_token(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* token, const size_t token_length, const uint8_t* data, const size_t data_length, uint8_t** mac, size_t* mac_length){
  uint32_t command = MAC_WITH_TOKEN;
  HM_CHECK(sizeof(uint32_t)==write(crypter->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], public_key, (uint32_t)public_key_length),  return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], data, (uint32_t)data_length),  return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], token, (uint32_t)token_length),  return -1);
  HM_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], mac, (uint32_t*)(mac_length)), return -1);
  return 0;
}

int hm_crypter_mac_with_creating_token(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* data, const size_t data_length, uint8_t** mac, size_t* mac_length, uint8_t** token, size_t* token_length){
  uint32_t command = MAC_WITH_CREATING_TOKEN;
  HM_CHECK(sizeof(uint32_t)==write(crypter->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], public_key, (uint32_t)public_key_length),  return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], data, (uint32_t)data_length),  return -1);
  HM_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], mac, (uint32_t*)(mac_length)), return -1);
  HM_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], token, (uint32_t*)(token_length)), return -1);
  return 0;
}

int hm_crypter_create_token_from_token(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* new_public_key, const size_t new_public_key_length, const uint8_t* token, const size_t token_length, uint8_t** new_token, size_t* new_token_length){
  uint32_t command = CREATE_TOKEN_FROM_TOKEN;
  HM_CHECK(sizeof(uint32_t)==write(crypter->crypt_reader_pipe[1], &command, sizeof(uint32_t)), return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], public_key, (uint32_t)public_key_length),  return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], new_public_key, (uint32_t)new_public_key_length),  return -1);
  HM_CHECK(write_data_to_pipe(crypter->crypt_reader_pipe[1], token, (uint32_t)token_length),  return -1);
  HM_CHECK(read_data_from_pipe(crypter->crypt_writer_pipe[0], new_token, (uint32_t*)(new_token_length)), return -1);
  return 0;  
}

int hm_crypter_sign(hm_crypter_t* crypter, const uint8_t* data, const size_t data_length, uint8_t** signed_data, size_t* signed_data_length){
  return -1;
}

int hm_crypter_verify(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* signed_data, const size_t signed_data_length, uint8_t** data, size_t* data_length){
  return -1;
}
