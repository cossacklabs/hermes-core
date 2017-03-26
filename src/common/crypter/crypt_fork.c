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
#include <hermes/common/utils.h>
#include <hermes/common/crypt.h>
#include <hermes/client_interfaces/keys_storage_interface.h>
#include "crypt_impl_heap_uni.h"
#include "utils/pipe.h"

#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


enum{
  HM_SC_COMMAND_ENCRYPT = 2604,   //not start from 0 or 1 for debug reasons (better to see in stream of data)
  HM_SC_COMMAND_ENCRYPT_WITH_TOKEN,
  HM_SC_COMMAND_ENCRYPT_WITH_CREATING_TOKEN,
  HM_SC_COMMAND_MAC_WITH_TOKEN,
  HM_SC_COMMAND_MAC_WITH_CREATING_TOKEN,
  HM_SC_COMMAND_DECRYPT,
  HM_SC_COMMAND_DECRYPT_WITH_TOKEN,
  HM_SC_COMMAND_SIGN,
  HM_SC_COMMAND_VERIFY,
  HM_SC_COMMAND_EXIT,
  HM_SC_COMMAND_CREATE_TOKEN_FROM_TOKEN,
} crypter_commands;

struct hm_crypter_t_{
  int crypt_reader_pipe[2];
  int crypt_writer_pipe[2];
  pid_t crypter_pid;
};

int simple_crypter_proccess(const uint8_t* id, const size_t id_length, int read_pipe, int write_pipe){
  if(!read_pipe || !write_pipe){
    return HM_INVALID_PARAMETER;
  }
  hm_keys_storage_t* key_store=hm_keys_storage_create();
  if(!key_store){
    return HM_FAIL;
  }
  uint8_t* key=NULL;
  size_t key_length=0;
  int res=hm_keys_storage_get_private_key_by_id_(key_store, id, id_length, &key, &key_length);
  hm_keys_storage_destroy(&key_store);
  if(HM_SUCCESS!=res){
    return HM_FAIL;
  }
  hm_crypter_impl_t* crypter = hm_crypter_impl_create(id, id_length, key, key_length);
  free(key);
  if(!crypter){
    return HM_FAIL;
  }
  uint64_t command=0;
  res = HM_SUCCESS;
  while(true){
    //read command, for simplicity, always contain 3 buffer params
    uint8_t *param1=NULL, *param2=NULL, *param3=NULL, *out_param1=NULL, *out_param2=NULL;
    size_t param1_len=0, param2_len=0, param3_len=0, out_param1_len=0, out_param2_len=0;
    if(HM_SUCCESS==HM_SC_READ(read_pipe, HM_SC_INT(&command), HM_SC_BUF(&param1, &param1_len), HM_SC_BUF(&param2, &param2_len), HM_SC_BUF(&param3, &param3_len))){
      hm_crypter_impl_destroy(&crypter);
      return HM_FAIL;
    }
    switch(command){
    case HM_SC_COMMAND_EXIT:
      free(param1);
      free(param2);
      free(param3);
      hm_crypter_impl_destroy(&crypter);
      res = HM_SC_WRITE(write_pipe, HM_SC_INT(HM_SUCCESS), HM_SC_BUF(out_param1, out_param1_len), HM_SC_BUF(out_param2, out_param2_len));
      return res;
    case HM_SC_COMMAND_ENCRYPT:
      res = hm_crypter_impl_uni_encrypt(crypter, param1, param1_len, param2, param2_len, param3, param3_len, &out_param1, &out_param1_len, &out_param2, &out_param2_len);
      break;
    case HM_SC_COMMAND_ENCRYPT_WITH_TOKEN:
      res = hm_crypter_impl_uni_encrypt_with_token(crypter, param1, param1_len, param2, param2_len, param3, param3_len, &out_param1, &out_param1_len, &out_param2, &out_param2_len);
      break;
    case HM_SC_COMMAND_ENCRYPT_WITH_CREATING_TOKEN:
      res = hm_crypter_impl_uni_encrypt_with_creating_token(crypter, param1, param1_len, param2, param2_len, param3, param3_len, &out_param1, &out_param1_len, &out_param2, &out_param2_len);
      break;
    case HM_SC_COMMAND_MAC_WITH_TOKEN:
      res = hm_crypter_impl_uni_mac_with_token(crypter, param1, param1_len, param2, param2_len, param3, param3_len, &out_param1, &out_param1_len, &out_param2, &out_param2_len);
      break;
    case HM_SC_COMMAND_MAC_WITH_CREATING_TOKEN:
      res = hm_crypter_impl_uni_mac_with_creating_token(crypter, param1, param1_len, param2, param2_len, param3, param3_len, &out_param1, &out_param1_len, &out_param2, &out_param2_len);
      break;
    case HM_SC_COMMAND_DECRYPT:
      res = hm_crypter_impl_uni_decrypt(crypter, param1, param1_len, param2, param2_len, param3, param3_len, &out_param1, &out_param1_len, &out_param2, &out_param2_len);
      break;
    case HM_SC_COMMAND_DECRYPT_WITH_TOKEN:
      res = hm_crypter_impl_uni_decrypt_with_token(crypter, param1, param1_len, param2, param2_len, param3, param3_len, &out_param1, &out_param1_len, &out_param2, &out_param2_len);
      break;
    case HM_SC_COMMAND_SIGN:
      res = hm_crypter_impl_uni_sign(crypter, param1, param1_len, param2, param2_len, param3, param3_len, &out_param1, &out_param1_len, &out_param2, &out_param2_len);
      break;
    case HM_SC_COMMAND_VERIFY:
      res = hm_crypter_impl_uni_verify(crypter, param1, param1_len, param2, param2_len, param3, param3_len, &out_param1, &out_param1_len, &out_param2, &out_param2_len);
      break;
    case HM_SC_COMMAND_CREATE_TOKEN_FROM_TOKEN:
      res = hm_crypter_impl_uni_verify(crypter, param1, param1_len, param2, param2_len, param3, param3_len, &out_param1, &out_param1_len, &out_param2, &out_param2_len);
      break;
    default:
      res = HM_FAIL;
    }
    free(param1);
    free(param2);
    free(param3);
    res = HM_SC_WRITE(write_pipe, HM_SC_INT(res), HM_SC_BUF(out_param1, out_param1_len), HM_SC_BUF(out_param2, out_param2_len));
    free(out_param1);
    free(out_param2);
    if(HM_SUCCESS!=res){
      hm_crypter_impl_destroy(&crypter);
      return res;
    }
  }
  hm_crypter_impl_destroy(&crypter);
  return res;
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
    close(crypter->crypt_reader_pipe[1]);
    close(crypter->crypt_writer_pipe[0]);
    simple_crypter_proccess(id, id_length, crypter->crypt_reader_pipe[0], crypter->crypt_writer_pipe[1]);
    close(crypter->crypt_reader_pipe[0]);
    close(crypter->crypt_writer_pipe[1]);
    _exit(EXIT_SUCCESS);
  }else{
    close(crypter->crypt_reader_pipe[1]);
    close(crypter->crypt_writer_pipe[0]);
  }
  return crypter;
}

int crypter_call(int read_pipe, int write_pipe, uint64_t command, const uint8_t* p1, const size_t p1_l, const uint8_t* p2, const size_t p2_l, const uint8_t* p3, const size_t p3_l, uint8_t** op1, size_t* op1_l, uint8_t** op2, size_t* op2_l){
  if(HM_SUCCESS!= HM_SC_WRITE(write_pipe, HM_SC_INT(command), HM_SC_BUF(p1, p1_l), HM_SC_BUF(p2, p2_l), HM_SC_BUF(p3, p3_l))){
    return HM_FAIL;
  }
  uint64_t res=0;
  if(HM_SUCCESS!= HM_SC_READ(read_pipe, HM_SC_INT(&res), HM_SC_BUF(&op1, &op1_l), HM_SC_BUF(op2, op2_l))){
    return HM_FAIL;
  }
  return res;
}

void hm_crypter_destroy(hm_crypter_t** crypter){
  uint8_t* op1=NULL, *op2=NULL;
  size_t op1_l=0, op2_l=0;
  crypter_call((*crypter)->crypt_writer_pipe[0], (*crypter)->crypt_reader_pipe[1], HM_SC_COMMAND_EXIT, NULL, 0, NULL, 0, NULL, 0, &op1, &op1_l, &op2, &op2_l);
  close((*crypter)->crypt_reader_pipe[1]);
  close((*crypter)->crypt_writer_pipe[0]);
  free(*crypter);
  return;
}

int hm_crypter_encrypt(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* data, const size_t data_length, uint8_t** encrypted_data, size_t* encrypted_data_length){
  uint8_t* op1=NULL;
  size_t op1_l=0;
  return crypter_call(crypter->crypt_writer_pipe[0], crypter->crypt_reader_pipe[1], HM_SC_COMMAND_ENCRYPT, public_key, public_key_length, data, data_length, NULL, 0, encrypted_data, encrypted_data_length, &op1, &op1_l);
}

int hm_crypter_decrypt(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* encrypted_data, const size_t encrypted_data_length, uint8_t** data, size_t* data_length){
  uint8_t* op1=NULL;
  size_t op1_l=0;
  return crypter_call(crypter->crypt_writer_pipe[0], crypter->crypt_reader_pipe[1], HM_SC_COMMAND_DECRYPT, public_key, public_key_length, encrypted_data, encrypted_data_length, NULL, 0, data, data_length, &op1, &op1_l);
}

int hm_crypter_encrypt_with_token(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* token, const size_t token_length, const uint8_t* data, const size_t data_length, uint8_t** encrypted_data, size_t* encrypted_data_length){
  uint8_t* op1=NULL;
  size_t op1_l=0;
  return crypter_call(crypter->crypt_writer_pipe[0], crypter->crypt_reader_pipe[1], HM_SC_COMMAND_ENCRYPT_WITH_TOKEN, public_key, public_key_length, token, token_length, data, data_length, encrypted_data, encrypted_data_length, &op1, &op1_l);
}

int hm_crypter_encrypt_with_creating_token(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* data, const size_t data_length, uint8_t** encrypted_data, size_t* encrypted_data_length, uint8_t** token, size_t* token_length){
  return crypter_call(crypter->crypt_writer_pipe[0], crypter->crypt_reader_pipe[1], HM_SC_COMMAND_ENCRYPT_WITH_CREATING_TOKEN, public_key, public_key_length, data, data_length, NULL, 0, encrypted_data, encrypted_data_length, token, token_length);
}

int hm_crypter_decrypt_with_token(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* token, const size_t token_length, const uint8_t* encrypted_data, const size_t encrypted_data_length, uint8_t** data, size_t* data_length){
  uint8_t* op1=NULL;
  size_t op1_l=0;
  return crypter_call(crypter->crypt_writer_pipe[0], crypter->crypt_reader_pipe[1], HM_SC_COMMAND_DECRYPT_WITH_TOKEN, public_key, public_key_length, token, token_length, encrypted_data, encrypted_data_length, data, data_length, &op1, &op1_l);
}

int hm_crypter_mac_with_token(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* token, const size_t token_length, const uint8_t* data, const size_t data_length, uint8_t** mac, size_t* mac_length){
  uint8_t* op1=NULL;
  size_t op1_l=0;
  return crypter_call(crypter->crypt_writer_pipe[0], crypter->crypt_reader_pipe[1], HM_SC_COMMAND_MAC_WITH_TOKEN, public_key, public_key_length, token, token_length, data, data_length, mac, mac_length, &op1, &op1_l);
}

int hm_crypter_mac_with_creating_token(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* data, const size_t data_length, uint8_t** mac, size_t* mac_length, uint8_t** token, size_t* token_length){
  return crypter_call(crypter->crypt_writer_pipe[0], crypter->crypt_reader_pipe[1], HM_SC_COMMAND_MAC_WITH_CREATING_TOKEN, public_key, public_key_length, data, data_length, NULL, 0, mac, mac_length, token, token_length);
}

int hm_crypter_create_token_from_token(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* new_public_key, const size_t new_public_key_length, const uint8_t* token, const size_t token_length, uint8_t** new_token, size_t* new_token_length){
  uint8_t* op1=NULL;
  size_t op1_l=0;
  return crypter_call(crypter->crypt_writer_pipe[0], crypter->crypt_reader_pipe[1], HM_SC_COMMAND_CREATE_TOKEN_FROM_TOKEN, public_key, public_key_length, new_public_key, new_public_key_length, token, token_length, new_token, new_token_length, &op1, &op1_l);
}

int hm_crypter_sign(hm_crypter_t* crypter, const uint8_t* data, const size_t data_length, uint8_t** signed_data, size_t* signed_data_length){
  uint8_t* op1=NULL;
  size_t op1_l=0;
  return crypter_call(crypter->crypt_writer_pipe[0], crypter->crypt_reader_pipe[1], HM_SC_COMMAND_SIGN, data, data_length, NULL, 0, NULL, 0, signed_data, signed_data_length, &op1, &op1_l);
}

int hm_crypter_verify(hm_crypter_t* crypter, const uint8_t* public_key, const size_t public_key_length, const uint8_t* signed_data, const size_t signed_data_length, uint8_t** data, size_t* data_length){
  uint8_t* op1=NULL;
  size_t op1_l=0;
  return crypter_call(crypter->crypt_writer_pipe[0], crypter->crypt_reader_pipe[1], HM_SC_COMMAND_SIGN, public_key, public_key_length, signed_data, signed_data_length, NULL, 0, data, data_length, &op1, &op1_l);
}
