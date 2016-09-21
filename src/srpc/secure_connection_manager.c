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

#include <hermes/utils.h>

#include <hermes/secure_connection_manager.h>
#include <hermes/connection_manager.h>

struct secure_connection_manager_t{
  connection_manager_t* connection_manager_;
  uint8_t* id_;
  size_t id_length_;
  uint8_t* private_key_;
  size_t private_key_length_;
  secure_server_function_t server_function_;
  secure_response_handler_t response_handler_;
  public_key_by_id_geter_t pub_key_geter_;
};

secure_connection_manager_status_t secure_connection_manager_destroy(secure_connection_manager_t* manager);

secure_connection_manager_t* secure_connection_manager_create_with_treads(const uint8_t* id, const size_t id_length, const uint8_t* private_key, const size_t private_key_length, public_key_by_id_geter_t pub_key_geter, const uint64_t threads_count){
  secure_connection_manager_t* manager=malloc(sizeof(secure_connection_manager_t));
  HERMES_CHECK(manager, return NULL);
  manager->id_=malloc(id_length);
  HERMES_CHECK(manager->id_, secure_connection_manager_destroy(manager); return NULL);
  memcpy(manager->id_, id, id_length);
  manager->private_key_=malloc(private_key_length);
  HERMES_CHECK(manager->private_key_, secure_connection_manager_destroy(manager); return NULL);
  memcpy(manager->private_key_, private_key, private_key_length);
  manager->pub_key_geter_=pub_key_geter;
}

secure_connection_manager_t* secure_connection_manager_create(const uint8_t* id, const size_t id_length, const uint8_t* private_key, const size_t private_key_length, public_key_by_id_geter_t pub_key_geter){
  return secure_connection_manager_create_with_treads(id, id_length, private_key, private_key_length, pub_key_geter, DEFAULT_THREADS_COUNT);
}

secure_connection_manager_status_t secure_connection_manager_destroy(secure_connection_manager_t* manager){
  HERMES_CHECK(manager, return SECURE_CONNECTION_MANAGER_INVALID_PARAM);
  if(manager->connection_manager_)
    connection_manager_destroy(manager->connection_manager_);
  free(manager);
  return SECURE_CONNECTION_MANAGER_SUCCESS;
}

secure_connection_manager_status_t secure_connection_manager_bind(secure_connection_manager_t* manager, const char* endpoint, secure_server_function_t server_function, void* user_data){

}

secure_connection_manager_status_t secure_connection_manager_connect(secure_connection_manager_t* manager, const char* endpoint){

}

secure_connection_manager_status_t secure_connection_manager_send_request(secure_connection_manager_t* manager, const uint8_t* request, const size_t request_length, secure_response_handler_t response_handler, void* user_data){

}


