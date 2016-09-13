/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef COMMON_CONFIG_H_
#define COMMON_CONFIG_H_

#include <stdint.h>
#include "ini.h"

#define MAX_KEY_LENGTH 4096
#define MAX_KEY_LENGTH 4096

typedef struct service_config_t_{
  char db_endpoint[INI_MAX_LINE_LENGTH];
  char db_name[INI_MAX_LINE_LENGTH];
  char collection[INI_MAX_LINE_LENGTH];
  char docs_collection[INI_MAX_LINE_LENGTH];
  char public_blocks_collection[INI_MAX_LINE_LENGTH];
  char private_blocks_collection[INI_MAX_LINE_LENGTH];  
  char docs_collections[INI_MAX_LINE_LENGTH];  
  char endpoint[INI_MAX_LINE_LENGTH];
  uint8_t private_key[MAX_KEY_LENGTH];
  size_t private_key_length;
  uint8_t public_key[MAX_KEY_LENGTH];
  size_t public_key_length;
}service_config_t;

typedef struct client_config_t_{
  char endpoint[INI_MAX_LINE_LENGTH];
  char id[INI_MAX_LINE_LENGTH];
  uint8_t public_key[MAX_KEY_LENGTH];
  size_t public_key_length;
}client_config_t;

struct config_t_{
  char name[INI_MAX_LINE_LENGTH];
  char id[INI_MAX_LINE_LENGTH];
  uint8_t private_key[MAX_KEY_LENGTH];
  size_t private_key_length;
  uint8_t public_key[MAX_KEY_LENGTH];
  size_t public_key_length;
  
  service_config_t credential_store;
  service_config_t key_service;
  service_config_t record_set;

  client_config_t credential_store_client;
  client_config_t key_service_client;
  client_config_t record_set_client;
};

typedef struct config_t_ config_t;
config_t* config_create(const char* init_file_name);
int config_destroy(config_t* cfg);

void config_get_private_key(config_t* config, uint8_t** key, size_t* key_length);
void config_get_public_key(config_t* config, uint8_t** key, size_t* key_length);
#endif /* COMMON_CONFIG_H_ */
