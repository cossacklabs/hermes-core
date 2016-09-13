/*
 * Copyright (c) 2016 Cossack Labs Limited
 *
 * This file is part of Hermes.
 *
 * Hermes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hermes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hermes.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hermes/utils.h>
#include <hermes/functions_collection.h>

functions_collection_t* functions_collection_create(const function_t* funcs, const size_t count){
  HERMES_CHECK(funcs && count!= 0, return NULL);
  functions_collection_t* res=malloc(sizeof(functions_collection_t));
  HERMES_CHECK(res, return NULL);
  res->count=count;
  res->functions=funcs;
  return res;
  
}
void functions_collection_destroy(functions_collection_t* coll){
  HERMES_CHECK(coll, return);
  free(coll);
}


functions_collection_status_t function_collection_exec_by_name(const functions_collection_t* coll, const char* id, const char* name, const uint8_t* params, const size_t params_length, uint8_t** result, size_t* result_length, void* call_ctx){
  fprintf(stderr, "call \"%s\"\n", name);
  int i=0;
  for(; i<coll->count; ++i){
    if(strcmp(coll->functions[i].name, name)==0){
      return coll->functions[i].func(id, params, params_length, result, result_length, call_ctx);
    }
  }
  return FUNCTIONS_COLLECTION_FAIL;
}

functions_collection_status_t functions_collection_exec(functions_collection_t* coll, const char* id, const uint8_t* params, const size_t params_length, uint8_t** result, size_t* result_length, void* call_ctx){
  return function_collection_exec_by_name(coll, id, params, params+strlen(params)+1, params_length-strlen(params)-1, result, result_length, call_ctx);
}

