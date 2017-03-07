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

#include <hermes/common/errors.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "pipe.h"

int write_data_to_pipe(int pipe, const uint8_t* data, uint32_t data_length){
  if(!pipe || !data || !data_length){
    return HM_INVALID_PARAMETER;
  }
  if(sizeof(uint32_t)!=write(pipe, &data_length, sizeof(uint32_t))){
    return HM_FAIL;
  }
  if(!data_length || data_length!=write(pipe, data, data_length)){
    return HM_FAIL;
  }
  return HM_SUCCESS;
}

int read_data_from_pipe(int pipe, uint8_t* data, uint32_t data_length){
  if(!pipe || !data || !data_length){
    return HM_INVALID_PARAMETER;
  }
  if(sizeof(uint32_t) != read(pipe, &data_length, sizeof(uint32_t))){
    return HM_FAIL;
  }
  if(!data_length || data_length != read(pipe, data, data_length)){
    return HM_FAIL;
  }
  return data_length;
}

int read_from_pipe(int pipe, ...){
  if(!pipe){
    return HM_INVALID_PARAMETER;
  }
  va_list va;
  va_start(va, pipe);
  int count=0, res=HM_SUCCESS;
  char* mark = va_arg(va, char*);
  for(;mark && !res;va_arg(va, char*), ++count){
    if(HM_SC_PARAM_MAGIC!=(int64_t)mark){ // every parameter must begin with HM_SIMPLE_CRYPTER_MAGIC
      res=HM_INVALID_PARAMETER;
      continue;
    }
    int64_t type=va_arg(va, int64_t);
    ++count;
    switch(type){
    case HM_SC_PARAM_INT:{
      int64_t* data=va_arg(va, int64_t*);
      ++count;
      if(sizeof(uint64_t)!=read_data_from_pipe(pipe, (uint8_t*)data, sizeof(uint64_t))){
        res=HM_FAIL;
        continue;
      }
    }
      break;
    case HM_SC_PARAM_STR:{
      char** data=va_arg(va, char**);
      uint32_t data_length;
      if(sizeof(uint32_t)!=read_data_from_pipe(pipe, (uint8_t*)(&data_length), sizeof(uint32_t))){
        res=HM_FAIL;
        continue;
      }
      *data = malloc(data_length);
      if(!(*data)){
        res = HM_BAD_ALLOC;
        continue;
      }
      count+=2;
      if(!data_length || data_length!=read_data_from_pipe(pipe, (uint8_t*)(*data), data_length)){
        res=HM_FAIL;
        continue;
      }
    }
      break;
    case HM_SC_PARAM_BUF:{
      uint8_t** data=va_arg(va, uint8_t**);
      uint32_t* data_length=va_arg(va, uint32_t*);
      uint32_t dl;
      if(sizeof(uint32_t)!=read_data_from_pipe(pipe, (uint8_t*)(&dl), sizeof(uint32_t))){
        res =HM_FAIL;
        continue;
      }
      count+=2;
      if(dl){
        *data = malloc(dl);
        if(!(*data)){
          res = HM_BAD_ALLOC;
          continue;
        }      
        if(dl!=read_data_from_pipe(pipe, *data, dl)){
          res=HM_FAIL;
          continue;
        }
        *data_length=dl;
      }
    }
      break;
    default:
      res=HM_INVALID_PARAMETER;
      continue;
    }
  }
  va_end(va);
  if(res){
    while(count>=0){
      va_start(va, pipe);
      char* mark = va_arg(va, char*);
      if(--count){
        mark = va_arg(va, char*);
      }
      if(--count){
        switch((int64_t)mark){
        case HM_SC_PARAM_BUF:{
          uint8_t** data=va_arg(va, uint8_t**);
          uint32_t* data_length=va_arg(va, uint32_t*);
          free(*data);
          count-=2;
        }
          break;
        case HM_SC_PARAM_STR:{
          char** data=va_arg(va, char**);
          free(*data);
          --count;
        }
          break;
        }
      }
    }
  }
  return res;
}

int write_to_pipe(int pipe, ...){
  if(!pipe){
    return HM_INVALID_PARAMETER;
  }
  va_list va;
  va_start(va, pipe);
  char* mark = va_arg(va, char*);
  for(;mark;mark=va_arg(va, char*)){
    if(HM_SC_PARAM_MAGIC!=(int64_t)mark){ // every parameter must begin with HM_SIMPLE_CRYPTER_MAGIC
      va_end(va);
      return HM_INVALID_PARAMETER;
    }
    int64_t type=va_arg(va, int64_t);
    switch(type){
    case HM_SC_PARAM_INT:{
      int64_t data=va_arg(va, int64_t);
      if(sizeof(uint64_t)!=write_data_to_pipe(pipe, (uint8_t*)&data, sizeof(uint64_t))){
        va_end(va);
        return HM_FAIL;
      }
    }
      break;
    case HM_SC_PARAM_STR:{
      char* data=va_arg(va, char*);
      uint32_t data_length = strlen(data);
      if(sizeof(uint32_t)!=write_data_to_pipe(pipe, (uint8_t*)(&data_length), sizeof(uint32_t))){
        va_end(va);
        return HM_FAIL;
      }
      if(data_length!=write_data_to_pipe(pipe, (uint8_t*)data, data_length)){
        va_end(va);
        return HM_FAIL;
      }
    }
      break;
    case HM_SC_PARAM_BUF:{
      uint8_t* data=va_arg(va, uint8_t*);
      uint32_t data_length=va_arg(va, uint32_t);
      if(sizeof(uint32_t)!=write_data_to_pipe(pipe, (uint8_t*)(&data_length), sizeof(uint32_t))){
        va_end(va);
        return HM_FAIL;
      }
      if(data_length){
        if(data_length!=write_data_to_pipe(pipe, data, data_length)){
          va_end(va);
          return HM_FAIL;
        }
      }
    }
      break;
    default:
      va_end(va);
      return HM_INVALID_PARAMETER;      
    }    
  }
  va_end(va);
  return HM_SUCCESS;  
}


