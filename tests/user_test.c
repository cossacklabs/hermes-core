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

#include <hermes/hermes_storages.h>
#include <hermes/hermes_user.h>
#include <hermes/hermes_document_block.h>
#include <hermes/utils.h>
#include <stdio.h>
#include <stdlib.h>

hermes_storages_t* hermes_storages=NULL;

int read_private_key(const char* file_name, uint8_t** private_key, size_t* private_key_length){
  FILE* key_file = fopen(file_name, "rb");
  HERMES_CHECK(key_file, return -1);

  //determine public key file size
  fseek(key_file, 0L, SEEK_END);
  *private_key_length = ftell(key_file);
  fseek(key_file, 0L, SEEK_SET);

  *private_key = malloc(*private_key_length);
  HERMES_CHECK(*private_key, fclose(key_file); return -1);
  HERMES_CHECK((*private_key_length) == fread(*private_key, 1, *private_key_length, key_file), free(*private_key); fclose(key_file); return -1);
  fclose(key_file);
  return 0;
}

int main(int argc, char* argv[]){
    uint8_t *user1_private_key=NULL, *user2_private_key=NULL;
    size_t user1_private_key_length=0, user2_private_key_length=0;

    if(0!=read_private_key("user1_private_key", &user1_private_key, &user1_private_key_length) || 0!=read_private_key("user2_private_key", &user2_private_key, &user2_private_key_length)){
	free(user1_private_key);
	free(user2_private_key);
	return -1;
    }

    hermes_storages = hermes_storages_create();
    hermes_user_t* user1 = hermes_user_create("user1", user1_private_key, user1_private_key_length, hermes_storages);
    hermes_user_t* user2 = hermes_user_create("user2", user2_private_key, user2_private_key_length, hermes_storages);
    free(user1_private_key);
    free(user2_private_key);
    if(!user1 || !user2){
	hermes_user_destroy(&user1);
	hermes_user_destroy(&user2);
	hermes_storages_destroy(&hermes_storages);
	return -1;
    }

    hermes_document_block_t* new_block = hermes_document_block_create_new(NULL, "new_document", false, "This is test document", strlen("This is test document"));
    if(!new_block){
	hermes_user_destroy(&user1);
	hermes_user_destroy(&user2);
	hermes_storages_destroy(&hermes_storages);
	return -1;
    }
    hermes_user_save_block(user1, new_block);

    hermes_document_block_t* new_private_block = hermes_document_block_create_new(new_block->doc_id, "new_private_document", true, "This is test document", strlen("This is test document"));
    if(!new_private_block){
	hermes_user_destroy(&user1);
	hermes_user_destroy(&user2);
	hermes_storages_destroy(&hermes_storages);
	hermes_document_block_destroy(&new_block);
 	return -1;
    }

    hermes_user_save_block(user1, new_private_block);
    

    hermes_document_block_t* loaded_block=NULL;
    if(0==hermes_user_load_block(user1, new_block->doc_id, new_block->doc_id, &loaded_block)){
      fprintf(stdout, "%s\n", loaded_block->data);
    }

    hermes_document_block_destroy(&loaded_block);
    
    if(0==hermes_user_load_block(user1, new_private_block->doc_id, new_private_block->id, &loaded_block)){
      fprintf(stdout, "%s\n", loaded_block->data);
    }
    
    hermes_document_block_destroy(&loaded_block);

    if(0==hermes_user_load_block(user2, new_block->doc_id, new_block->doc_id, &loaded_block)){
      fprintf(stdout, "%s\n", loaded_block->data);
    }

    hermes_document_block_destroy(&loaded_block);

    if(0==hermes_user_load_block(user2, new_private_block->doc_id, new_private_block->id, &loaded_block)){
      fprintf(stdout, "%s\n", loaded_block->data);
      hermes_document_block_destroy(&loaded_block);
    }

    if(0==hermes_user_grand_access_to_block(user1, "user2", new_private_block, HERMES_READ_ACCESS_MASK)){
      fprintf(stdout, "granting read access for new_private_block for \"user2\"\n");
    }

    if(0==hermes_user_load_block(user2, new_private_block->doc_id, new_private_block->id, &loaded_block)){
      fprintf(stdout, "%s\n", loaded_block->data);
      hermes_document_block_destroy(&loaded_block);
    }

    if(0==hermes_document_block_set_data(new_block, "This is updated doc_context", strlen("This is updated doc_context")+1)){
      fprintf(stdout, "%s\n", new_block->data);
    }

    if(0==hermes_user_save_block(user1, new_block)){
      fprintf(stdout, "block updated\n");      
    }


    if(0==hermes_user_load_block(user2, new_block->doc_id, new_block->doc_id, &loaded_block)){
      fprintf(stdout, "loaded block %s\n", loaded_block->data);
    }    
    
    if(0==hermes_document_block_set_data(loaded_block, "This is also updated doc_context", strlen("This is also updated doc_context")+1)){
      fprintf(stdout, "%s\n", loaded_block->data);
    }
    
    if(0==hermes_user_save_block(user2, loaded_block)){
      fprintf(stdout, "block updated\n");      
    }

    hermes_document_block_destroy(&loaded_block);
    
    if(0==hermes_user_grand_access_to_block(user1, "user2", new_block, HERMES_UPDATE_ACCESS_MASK)){
      fprintf(stdout, "granting read access for new_private_block for \"user2\"\n");
    }


    if(0==hermes_user_load_block(user2, new_block->doc_id, new_block->doc_id, &loaded_block)){
      fprintf(stdout, "loaded block %s\n", loaded_block->data);
    }    
    
    if(0==hermes_document_block_set_data(loaded_block, "This is also updated doc_context", strlen("This is also updated doc_context")+1)){
      fprintf(stdout, "%s\n", loaded_block->data);
    }
    
    if(0==hermes_user_save_block(user2, loaded_block)){
      fprintf(stdout, "block updated\n");      
    }

    hermes_document_block_destroy(&loaded_block);
    
    hermes_document_block_destroy(&new_block);
    hermes_document_block_destroy(&new_private_block);

    hermes_user_destroy(&user1);
    hermes_user_destroy(&user2);
    hermes_storages_destroy(&hermes_storages);
    return 1;
}
