/*
 * Copyright (c) 2017 Cossack Labs Limited
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


#include "key_store_impl.h"

#include <hermes/key_store/client.h>

#include <assert.h>
#include <string.h> 

struct hermes_key_store_type{
  hm_key_store_client_sync_t* cl;
};

hermes_key_store_t* hermes_key_store_create(hm_rpc_transport_t* transport){
  if(!transport){
    return NULL;
  }
  hermes_key_store_t* ks=calloc(1, sizeof(hermes_key_store_t));
  if(!ks
     || !(ks->cl=hm_key_store_client_sync_create(transport))){
    hermes_key_store_destroy(&ks);
    return NULL;
  }
  return ks;
}

hermes_status_t hermes_key_store_get_rtoken(hermes_key_store_t* ks,
                                            const uint8_t* user_id,
                                            const size_t user_id_length,
                                            const uint8_t* block_id,
                                            const size_t block_id_length,
                                            uint8_t** token,
                                            size_t* token_length,
                                            uint8_t** owner_id,
                                            size_t* owner_id_length){
  if(!ks || !user_id || !user_id_length || !block_id || !block_id_length || !token || !token_length || !owner_id || !owner_id_length){
    return HM_INVALID_PARAMETER;
  }
  return hm_key_store_client_sync_call_get_rtoken(ks->cl,
                                                  block_id,
                                                  block_id_length,
                                                  user_id,
                                                  user_id_length,
                                                  token,
                                                  token_length,
                                                  owner_id,
                                                  owner_id_length);
}

hermes_status_t hermes_key_store_get_wtoken(hermes_key_store_t* ks,
                                            const uint8_t* user_id,
                                            const size_t user_id_length,
                                            const uint8_t* block_id,
                                            const size_t block_id_length,
                                            uint8_t** token,
                                            size_t* token_length,
                                            uint8_t** owner_id,
                                            size_t* owner_id_length){
  if(!ks || !user_id || !user_id_length || !block_id || !block_id_length || !token || !token_length || !owner_id || !owner_id_length){
    return HM_INVALID_PARAMETER;
  }
  return hm_key_store_client_sync_call_get_wtoken(ks->cl,
                                                  block_id,
                                                  block_id_length,
                                                  user_id,
                                                  user_id_length,
                                                  token,
                                                  token_length,
                                                  owner_id,
                                                  owner_id_length);
}

hermes_status_t hermes_key_store_set_rtoken(hermes_key_store_t* ks,
                                            const uint8_t* user_id,
                                            const size_t user_id_length,
                                            const uint8_t* block_id,
                                            const size_t block_id_length,
                                            const uint8_t* token,
                                            const size_t token_length,
                                            const uint8_t* owner_id,
                                            const size_t owner_id_length){
  if(!ks || !user_id || !user_id_length || !block_id || !block_id_length || !token || !token_length || !owner_id || !owner_id_length){
    return HM_INVALID_PARAMETER;
  }
  return hm_key_store_client_sync_call_set_rtoken(ks->cl,
                                                  block_id,
                                                  block_id_length,
                                                  user_id,
                                                  user_id_length,
                                                  owner_id,
                                                  owner_id_length,
                                                  token,
                                                  token_length);
}

hermes_status_t hermes_key_store_set_wtoken(hermes_key_store_t* ks,
                                            const uint8_t* user_id,
                                            const size_t user_id_length,
                                            const uint8_t* block_id,
                                            const size_t block_id_length,
                                            const uint8_t* token,
                                            const size_t token_length,
                                            const uint8_t* owner_id,
                                            const size_t owner_id_length){
  if(!ks || !user_id || !user_id_length || !block_id || !block_id_length || !token || !token_length || !owner_id || !owner_id_length){
    return HM_INVALID_PARAMETER;
  }
  return hm_key_store_client_sync_call_set_wtoken(ks->cl,
                                                  block_id,
                                                  block_id_length,
                                                  user_id,
                                                  user_id_length,
                                                  owner_id,
                                                  owner_id_length,
                                                  token,
                                                  token_length);
}

hermes_status_t hermes_key_store_destroy(hermes_key_store_t** ks){
  if(!ks || !(*ks)){
    return HM_INVALID_PARAMETER;
  }
  hm_key_store_client_sync_destroy(&((*ks)->cl));
  free(*ks);
  *ks=NULL;
  return HM_SUCCESS;
}

struct hermes_key_store_iterator_type{
  hermes_key_store_t* ks;
  uint8_t* block_id;
  size_t block_id_length;
  uint8_t* current_user_id;
  size_t current_user_id_length;
  uint32_t current_rights_mask;
  size_t current_index;
};

#define KEY_STORE_ITERATOR_NODE_SIZE 5 //each node  must contain 5 buffers: user id, read token, read token owner, write token, write token owner

hermes_key_store_iterator_t* hermes_key_store_iterator_create(hermes_key_store_t* ks, const uint8_t* block_id, const size_t block_id_length){
  if(!ks || !block_id || !block_id_length){
    return NULL;
  }
  hermes_key_store_iterator_t* i=calloc(1, sizeof(hermes_key_store_iterator_t));
  assert(i);
  i->block_id=malloc(block_id_length);
  assert(i->block_id);
  memcpy(i->block_id, block_id, block_id_length);
  i->block_id_length=block_id_length;
  i->ks=ks;
  if(HM_SUCCESS!=hm_key_store_client_sync_call_get_indexed_rights(ks->cl,block_id, block_id_length, i->current_index, &(i->current_user_id), &(i->current_user_id_length), &(i->current_rights_mask))){
    hermes_key_store_iterator_destroy(&i);
    return NULL;
  }
  return i;
}

hermes_status_t hermes_key_store_iterator_next(hermes_key_store_iterator_t* i){
  if(!i){
    return HM_INVALID_PARAMETER;
  }
  free(i->current_user_id);
  if(HM_SUCCESS!=hm_key_store_client_sync_call_get_indexed_rights(i->ks->cl,i->block_id, i->block_id_length, ++(i->current_index), &(i->current_user_id), &(i->current_user_id_length), &(i->current_rights_mask))){
    return HM_SUCCESS;
  }
  return HM_FAIL;
}

uint8_t* hermes_key_store_iterator_get_user_id(hermes_key_store_iterator_t* i){
  if(!i){
    return NULL;
  }
  return i->current_user_id;
}

size_t hermes_key_store_iterator_get_user_id_length(hermes_key_store_iterator_t* i){
  if(!i){
    return 0;
  }
  return i->current_user_id_length;
}

uint8_t* hermes_key_store_iterator_get_rtoken(hermes_key_store_iterator_t* i){
  if(!i){
    return NULL;
  }
  if((i->current_rights_mask)|HM_KEY_STORE_READ_ACCESS_MASK){
    return i->current_user_id;
  }
  return NULL;
}

size_t hermes_key_store_iterator_get_rtoken_length(hermes_key_store_iterator_t* i){
  if(!i){
    return 0;
  }
  if((i->current_rights_mask)|HM_KEY_STORE_READ_ACCESS_MASK){
    return i->current_user_id_length;
  }
  return 0;
}

uint8_t* hermes_key_store_iterator_get_rtoken_owner(hermes_key_store_iterator_t* i){
  if(!i){
    return NULL;
  }
  if((i->current_rights_mask)|HM_KEY_STORE_READ_ACCESS_MASK){
    return i->current_user_id;
  }
  return NULL;
}

size_t hermes_key_store_iterator_get_rtoken_owner_length(hermes_key_store_iterator_t* i){
  if(!i){
    return 0;
  }
  if((i->current_rights_mask)|HM_KEY_STORE_READ_ACCESS_MASK){
    return i->current_user_id_length;
  }
  return 0;
}

uint8_t* hermes_key_store_iterator_get_wtoken(hermes_key_store_iterator_t* i){
  if(!i){
    return NULL;
  }
  if((i->current_rights_mask)|HM_KEY_STORE_WRITE_ACCESS_MASK){
    return i->current_user_id;
  }
  return NULL;
}

size_t hermes_key_store_iterator_get_wtoken_length(hermes_key_store_iterator_t* i){
  if(!i){
    return 0;
  }
  if((i->current_rights_mask)|HM_KEY_STORE_WRITE_ACCESS_MASK){
    return i->current_user_id_length;
  }
  return 0;
}

uint8_t* hermes_key_store_iterator_get_wtoken_owner(hermes_key_store_iterator_t* i){
  if(!i){
    return NULL;
  }
  if((i->current_rights_mask)|HM_KEY_STORE_WRITE_ACCESS_MASK){
    return i->current_user_id;
  }
  return NULL;
}

size_t hermes_key_store_iterator_get_wtoken_owner_length(hermes_key_store_iterator_t* i){
  if(!i){
    return 0;
  }
  if((i->current_rights_mask)|HM_KEY_STORE_WRITE_ACCESS_MASK){
    return i->current_user_id_length;
  }
  return 0;
}

hermes_status_t hermes_key_store_iterator_destroy(hermes_key_store_iterator_t** i){
  if(!i ||  !(*i)){
    return HM_INVALID_PARAMETER;
  }
  free((*i)->block_id);
  free((*i)->current_user_id);
  free(*i);
  *i=NULL;
  return HM_SUCCESS;
}
