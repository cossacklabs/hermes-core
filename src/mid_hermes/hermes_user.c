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


#include <hermes/hermes_user.h>
#include <hermes/hermes_storages.h>
#include <hermes/utils.h>

#include <stdlib.h>

char* str_(const char* str){
  if(str){
    char* str_ = calloc(strlen(str)+1,1);
    HERMES_CHECK(str_, return NULL);
    memcpy(str_, str, strlen(str)+1);
    return str_;
  }
  return NULL;
}

uint8_t* blob_(const uint8_t* blob, const size_t blob_size){
  HERMES_CHECK(blob, return NULL);
  uint8_t* blob_=calloc(blob_size,1);
  HERMES_CHECK(blob_, return NULL);
  memcpy(blob_, blob, blob_size);
  return blob_;
}


struct hermes_user_t_{
  char* id;
  uint8_t* public_key;
  size_t public_key_length;
  hermes_crypter_t* crypter;
};

hermes_user_t* hermes_user_create(const char* user_id, const uint8_t* private_key, const size_t private_key_length){
  HERMES_CHECK(user_id && private_key, return NULL);
  hermes_user_t* user = calloc(sizeof(hermes_user_t), 1);
  HERMES_CHECK(user, return NULL);
  HERMES_CHECK(user->id = str_(user_id), hermes_user_destroy(&user); return NULL);
  user->crypter = hermes_crypter_create(private_key, private_key_length);
  HERMES_CHECK(0 == hermes_credential_store_get_pub_key(hermes_storages_get_credential_store(hermes_storages), user_id, &user->public_key, &user->public_key_length), hermes_user_destroy(&user); return NULL);
  HERMES_CHECK(user->crypter, hermes_user_destroy(&user); return NULL);
  return user;
}

void hermes_user_destroy(hermes_user_t** user){
  HERMES_CHECK(*user, return);
  if((*user)->crypter)
    hermes_crypter_destroy(&((*user)->crypter));
  free((*user)->public_key);
  free(*user);
  *user=NULL;
}

int hermes_user_save_block(hermes_user_t* user, hermes_document_block_t* block){
  HERMES_CHECK(user && user->crypter && block, return -2);
  uint8_t* owner_public_key=NULL, *encrypted_data=NULL;
  size_t owner_public_key_length=0, encrypted_data_length=NULL;
  uint8_t* blob=NULL, *old_data=block->data;
  size_t blob_length=0, old_data_length = block->data_length;
  if(!block->owner_id)
    block->owner_id = str_(user->id);
  HERMES_CHECK(0 == hermes_credential_store_get_pub_key(hermes_storages_get_credential_store(hermes_storages), block->owner_id, &owner_public_key, &owner_public_key_length),  return -1);
  if(block->is_loaded && block->is_changed){
    //update previously loaded block
    HERMES_CHECK(block->update_token && block->update_token_length && block->mac && block->mac_length, return -2);
    HERMES_CHECK(0==hermes_crypter_mac_with_token(user->crypter, owner_public_key, owner_public_key_length, block->update_token, block->update_token_length, block->data, block->data_length, &(block->new_mac), &(block->new_mac_length)), free(owner_public_key); return -1);
    if(block->is_private){
      HERMES_CHECK(0==hermes_crypter_encrypt_with_token(user->crypter, owner_public_key, owner_public_key_length, block->read_token, block->read_token_length, block->data, block->data_length, &encrypted_data, &encrypted_data_length), free(owner_public_key); return -1);
      free(block->data);
      block->data = encrypted_data;
      block->data_length = encrypted_data_length;
    }
  }else if(block->is_changed){
    HERMES_CHECK(0==hermes_crypter_mac_with_creating_token(user->crypter, owner_public_key, owner_public_key_length, block->data, block->data_length, &(block->new_mac), &(block->new_mac_length), &(block->update_token), &(block->update_token_length)), free(owner_public_key); return -1);
    if(block->is_private){
      HERMES_CHECK(0==hermes_crypter_encrypt_with_creating_token(user->crypter, owner_public_key, owner_public_key_length, block->data, block->data_length, &encrypted_data, &encrypted_data_length, &(block->read_token), &(block->read_token_length)), free(owner_public_key); return -1);
      free(block->data);
      block->data = encrypted_data;
      block->data_length = encrypted_data_length;
    }
  }
  HERMES_CHECK(0==hermes_document_block_to_blob(block, &blob, &blob_length), block->data=old_data; block->data_length=old_data_length; free(encrypted_data); return -1);
  if(block->is_loaded && block->is_changed){
    HERMES_CHECK(0== hermes_record_set_set_block(hermes_storages_get_record_set_store(hermes_storages), block->doc_id, block->id, block->mac, block->mac_length, blob, blob_length), block->data=old_data; block->data_length=old_data_length; free(encrypted_data); return -1);
  }else if(block->is_changed){
    HERMES_CHECK(0== hermes_record_set_set_new_block(hermes_storages_get_record_set_store(hermes_storages), block->doc_id, blob, blob_length, &(block->id)), block->data=old_data; block->data_length=old_data_length; free(encrypted_data); return -1);
    if(!(block->doc_id))
      block->doc_id = str_(block->id);
    HERMES_CHECK(0 == hermes_access_key_store_set_read_token(hermes_storages_get_access_key_store(hermes_storages), block->doc_id, block->id, user->id, block->read_token, block->read_token_length), block->data=old_data; block->data_length=old_data_length; free(encrypted_data); return -1);
    HERMES_CHECK(0 == hermes_access_key_store_set_update_token(hermes_storages_get_access_key_store(hermes_storages), block->doc_id, block->id, user->id, block->update_token, block->update_token_length), block->data=old_data; block->data_length=old_data_length; free(encrypted_data); return -1);
    block->mac = blob_(block->new_mac, block->new_mac_length);
    block->mac_length = block->new_mac_length;
  }
  block->is_changed=false;
  block->is_loaded=true;
  return 0;
}

int hermes_user_load_block(hermes_user_t* user, const char* doc_id, const char* block_id, hermes_document_block_t** block){
  HERMES_CHECK(user && user->crypter, return -2);
  uint8_t* blob=NULL;
  size_t blob_length=0;
  HERMES_CHECK(0 == hermes_record_set_get_block(hermes_storages_get_record_set_store(hermes_storages), doc_id, block_id, &blob, &blob_length), return -1);
  *block = hermes_document_block_create_from_blob(blob, blob_length);
  HERMES_CHECK(*block, return -1);
  (*block)->id=str_(block_id);
  (*block)->doc_id = str_(doc_id);
  uint8_t* owner_public_key=NULL, *decrypted_data=NULL;
  size_t owner_public_key_length=0, decrypted_data_length=NULL;
  HERMES_CHECK(0 == hermes_credential_store_get_pub_key(hermes_storages_get_credential_store(hermes_storages), (*block)->owner_id, &owner_public_key, &owner_public_key_length), hermes_document_block_destroy(block); return -1);
  if((*block)->is_private){
    HERMES_CHECK(0 == hermes_access_key_store_get_read_token(hermes_storages_get_access_key_store(hermes_storages), doc_id, block_id, user->id, &((*block)->read_token), &((*block)->read_token_length)), hermes_document_block_destroy(block); return -1);
    HERMES_CHECK(0==hermes_crypter_decrypt_with_token(user->crypter, owner_public_key, owner_public_key_length, (*block)->read_token, (*block)->read_token_length, (*block)->data, (*block)->data_length, &decrypted_data, &decrypted_data_length), hermes_document_block_destroy(block); free(owner_public_key); return -1);
    free((*block)->data);
    (*block)->data = decrypted_data;
    (*block)->data_length = decrypted_data_length;
  }
  if(0 == hermes_access_key_store_get_update_token(hermes_storages_get_access_key_store(hermes_storages), doc_id, block_id, user->id, &((*block)->update_token), &((*block)->update_token_length))){
    HERMES_CHECK(0==hermes_crypter_mac_with_token(user->crypter, owner_public_key, owner_public_key_length, (*block)->update_token, (*block)->update_token_length, (*block)->data, (*block)->data_length, &((*block)->mac), &((*block)->mac_length)), hermes_document_block_destroy(block); free(owner_public_key); return -1);
  }
  (*block)->is_loaded = true;
  return 0;
}

int hermes_user_delete_block(hermes_user_t* user, hermes_document_block_t* block){
  HERMES_CHECK(user && user->crypter && block && block->is_loaded && block->mac && block->mac_length, return -2);
  HERMES_CHECK(0== hermes_record_set_delete_block(hermes_storages_get_record_set_store(hermes_storages), block->doc_id, block->id,  block->mac, block->mac_length), return -1);
  return 0;
}


int hermes_user_grand_access_to_block(hermes_user_t* user, const char* user_id_granting_access_to, hermes_document_block_t* block, int rights_mask){
  HERMES_CHECK(user && user->crypter, return -2);
  HERMES_CHECK(block && block->is_loaded && block->id && block->doc_id, return -2);
  uint8_t *owner_public_key=NULL, *user_granting_to_public_key=NULL;
  size_t owner_public_key_length=0, user_granting_to_public_key_length=0;
  HERMES_CHECK(0 == hermes_credential_store_get_pub_key(hermes_storages_get_credential_store(hermes_storages), block->owner_id, &owner_public_key, &owner_public_key_length), return -1);  
  HERMES_CHECK(0 == hermes_credential_store_get_pub_key(hermes_storages_get_credential_store(hermes_storages), user_id_granting_access_to, &user_granting_to_public_key, &user_granting_to_public_key_length), free(owner_public_key);return -1);
  uint8_t *new_token=NULL;
  size_t new_token_length=0;
  switch(rights_mask){
  case HERMES_UPDATE_ACCESS_MASK:
    free(block->update_token);
    block->update_token=NULL;
    HERMES_CHECK(0==hermes_access_key_store_get_update_token(hermes_storages_get_access_key_store(hermes_storages), block->doc_id, block->id, user->id, &(block->update_token), &(block->update_token_length)), free(owner_public_key); free(user_granting_to_public_key); return -1);
    HERMES_CHECK(0==hermes_crypter_create_token_from_token(user->crypter, owner_public_key, owner_public_key_length, user_granting_to_public_key, user_granting_to_public_key_length, block->update_token, block->update_token_length, &new_token, &new_token_length), free(owner_public_key); free(user_granting_to_public_key); return -1);
    HERMES_CHECK(0 == hermes_access_key_store_set_update_token(hermes_storages_get_access_key_store(hermes_storages), block->doc_id, block->id, user_id_granting_access_to, new_token, new_token_length), free(owner_public_key); free(user_granting_to_public_key); free(new_token); return -1);
    free(new_token);
  case HERMES_READ_ACCESS_MASK:
    free(block->read_token);
    block->read_token=NULL;
    HERMES_CHECK(0==hermes_access_key_store_get_read_token(hermes_storages_get_access_key_store(hermes_storages), block->doc_id, block->id, user->id, &(block->read_token), &(block->read_token_length)), free(owner_public_key); free(user_granting_to_public_key); return -1);
    if(block->read_token_length){
      HERMES_CHECK(0==hermes_crypter_create_token_from_token(user->crypter, owner_public_key, owner_public_key_length, user_granting_to_public_key, user_granting_to_public_key_length, block->read_token, block->read_token_length, &new_token, &new_token_length), free(owner_public_key); free(user_granting_to_public_key); return -1);
      HERMES_CHECK(0 == hermes_access_key_store_set_read_token(hermes_storages_get_access_key_store(hermes_storages), block->doc_id, block->id, user_id_granting_access_to, new_token, new_token_length), free(owner_public_key); free(user_granting_to_public_key); free(new_token); return -1);
      free(new_token);
    }
    break;
  default:
    break;
  }
  free(owner_public_key);
  free(user_granting_to_public_key);
  return 0;
}

int hermes_user_deny_access_to_block(hermes_user_t* user, const char* user_id_denying_access_to, hermes_document_block_t* block){
  HERMES_CHECK(user && user->crypter && block && block->is_loaded && block->mac && block->mac_length, return -2);
  HERMES_CHECK(0==hermes_access_key_store_rem_read_token(hermes_storages_get_access_key_store(hermes_storages), block->doc_id, block->id, user->id, user_id_denying_access_to), return -1);
  HERMES_CHECK(0==hermes_access_key_store_rem_update_token(hermes_storages_get_access_key_store(hermes_storages), block->doc_id, block->id, user->id, user_id_denying_access_to), return -1);
  return 0;
}
