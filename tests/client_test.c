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
#include <hermes/hermes_client.h>
#include <hermes/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#define STR_2_DATA_LEN(str) str, strlen(str)+1

int main(int argc, char* argv[]){
    uint8_t *user1_private_key=NULL, *user2_private_key=NULL;
    size_t user1_private_key_length=0, user2_private_key_length=0;
    int res=0;
    hermes_client_t *client1=NULL, *client2=NULL;
    char *new_id=NULL, *new_private_block_id=NULL, *new_public_block_id=NULL;

    res=read_private_key("user1_private_key", &user1_private_key, &user1_private_key_length);
    if(res)goto err;
    res=read_private_key("user2_private_key", &user2_private_key, &user2_private_key_length);
    if(res)goto err;

    hermes_storages = hermes_storages_create();

    client1 = hermes_client_create("user1", user1_private_key, user1_private_key_length, hermes_storages);
    if(!client1){res=-1; goto err;}
    client2 = hermes_client_create("user2", user2_private_key, user2_private_key_length, hermes_storages);
    if(!client2){res=-1; goto err;} 
    free(user1_private_key);
    user1_private_key=NULL;
    free(user2_private_key);
    user2_private_key=NULL;

    res = hermes_client_create_document(client1, STR_2_DATA_LEN("This is new document"), &new_id);
    if(res)goto err;
    printf("document \"%s\" was create\n", new_id);
    res = hermes_client_create_document_block(client1, new_id, false, STR_2_DATA_LEN("This is new document public block"), &new_public_block_id);
    if(res) goto err;
    printf("public block \"%s\" was added to document \"%s\"\n", new_public_block_id, new_id);
    res = hermes_client_create_document_block(client1, new_id, true, STR_2_DATA_LEN("This is new document private block"), &new_private_block_id);
    if(res) goto err;
    printf("private block \"%s\" was added to document \"%s\"\n", new_private_block_id, new_id);

    uint8_t* data=NULL;
    size_t data_length=0;
    res =hermes_client_read_document(client1, new_id, &data, &data_length);
    if(res)goto err;
    printf("document \"%s\" context loaded: %s\n", new_id, data);
    free(data);

    res=hermes_client_read_document_block(client1, new_id, new_public_block_id, &data, &data_length);
    if(res)goto err;
    printf("public block \"%s\" of document \"%s\" context loaded: %s\n", new_public_block_id, new_id, data);
    free(data);
    
    res=hermes_client_read_document_block(client1, new_id, new_private_block_id, &data, &data_length);
    if(res)goto err;
    printf("private block \"%s\" of document \"%s\" context loaded: %s\n", new_private_block_id, new_id, data);
    free(data);

    res = hermes_client_read_document(client2, new_id, &data, &data_length);
    if(res)goto err;
    printf("document \"%s\" context loaded: %s\n", new_id, data);
    free(data);
    res=hermes_client_read_document_block(client2, new_id, new_public_block_id, &data, &data_length);
    if(res)goto err;
    printf("public block \"%s\" of document \"%s\" context loaded: %s\n", new_public_block_id, new_id, data);
    free(data);
    
    res=hermes_client_read_document_block(client2, new_id, new_private_block_id, &data, &data_length);
    if(res)
      printf("private block \"%s\" of document \"%s\" can not be loaded.\n", new_private_block_id, new_id);
    else{
      printf("private block \"%s\" of document \"%s\" context loaded: %s\n", new_private_block_id, new_id, data);
      free(data);
    }

    res = hermes_client_update_document(client1, new_id, STR_2_DATA_LEN("This is updated document by client 1"));
    if(res)goto err;
    res = hermes_client_update_document(client2, new_id, STR_2_DATA_LEN("This is updated document by client 2"));
    if(!res)goto err;

    res = hermes_client_update_document_block(client1, new_id, new_public_block_id,  STR_2_DATA_LEN("This is updated public document block by client 1"));
    if(res)goto err;
    res = hermes_client_update_document_block(client2, new_id, new_public_block_id,  STR_2_DATA_LEN("This is updated public document block by client 2"));
    if(!res)goto err;
    res = hermes_client_update_document_block(client1, new_id, new_private_block_id,  STR_2_DATA_LEN("This is updated private document block by client 1"));
    if(res)goto err;
    res = hermes_client_update_document_block(client2, new_id, new_private_block_id,  STR_2_DATA_LEN("This is updated private document block by client 2"));
    if(!res)goto err;
    
    res = hermes_client_grant_access_to_document(client1, new_id, "user2", HERMES_UPDATE_ACCESS_MASK);
    if(res)goto err;

    res = hermes_client_update_document(client2, new_id, STR_2_DATA_LEN("This is updated document by client 2"));
    if(res)goto err;
    res = hermes_client_update_document_block(client2, new_id, new_public_block_id,  STR_2_DATA_LEN("This is updated public document block by client 2"));
    if(!res)goto err;
    res = hermes_client_update_document_block(client2, new_id, new_public_block_id,  STR_2_DATA_LEN("This is updated public document block by client 2"));
    if(!res)goto err;


    /*    if(0==hermes_user_grand_access_to_block(user1, "user2", new_private_block, HERMES_READ_ACCESS_MASK)){
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
*/
err:
    free(user1_private_key);
    free(user2_private_key);
    hermes_client_destroy(&client1);
    hermes_client_destroy(&client2);
    hermes_storages_destroy(&hermes_storages);
    free(new_id);
    free(new_public_block_id);
    free(new_private_block_id);
    printf("%i\n", res);
    return res;
}
