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

#include "utils.h"
#include "base64.h"

#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h> 


char* buf_to_string(const uint8_t* buf, const size_t buf_length, char* string){
  base64_encode(string, 2048, buf, buf_length);
  return string;
}

size_t string_to_buf(const char* string, uint8_t* buf){
  return base64_decode(buf, string, 2048);
}

int remove_directory(const char *path)
{
  DIR *d = opendir(path);
  size_t path_len = strlen(path);
  int r=1;
  if (d){
    struct dirent *p;
    r = 0;
    while (!r && (p=readdir(d))){
      int r2 = -1;
      char *buf;
      size_t len;
      if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")){
        continue;
      }
      len = path_len + strlen(p->d_name) + 2; 
      buf = malloc(len);
      if (buf){
        struct stat statbuf;
        snprintf(buf, len, "%s/%s", path, p->d_name);
        if (!stat(buf, &statbuf)){
          if (S_ISDIR(statbuf.st_mode)){
            r2 = remove_directory(buf);
          }else{
            r2 = unlink(buf);
          }
        }
        free(buf);
      }
      r = r2;
    }
    closedir(d);
  }
  if (!r){
    r = rmdir(path);
  }
  return 0;
}

int create_directory(const char *path){
  char subpath[FILENAME_MAX] = "", *delim;
  if (NULL != (delim = strrchr(path, '/'))){
    strncat(subpath, path, delim - path);
    create_directory(subpath);    
  }
  mkdir(path,0777);
  return 0;
}


uint32_t read_whole_file(const char* file_name, uint8_t** buf, size_t* buf_length){
  FILE* f=fopen(file_name, "rb");
  if(!f){
    return 1;
  }
  size_t read_to_length=0;
  fseek(f, 0L, SEEK_END);
  read_to_length = ftell(f);
  fseek(f, 0L, SEEK_SET);
  *buf = malloc(read_to_length);
  assert(*buf);
  *buf_length = fread(*buf, 1, read_to_length, f);
  fclose(f);
  return 0;
}

uint32_t write_whole_file(const char* filename, const uint8_t* buf, const size_t buf_length){
  FILE* f=fopen(filename, "wb");
  if(!f){
    return 1;
  }
  size_t writed_bytes=fwrite(buf, 1, buf_length, f);
  fclose(f);
  return 0;
}


char* build_path(char* to, ...){
  va_list valist;
  va_start(valist, to);
  const char* u=va_arg(valist, const char*);
  while(u){
    sprintf(to, "%s/%s", to, u);
    u=va_arg(valist, const char*);
  }
  return to;
}

char* build_typed_path(char* to, ...){
  sprintf(to, ".");
  va_list valist;
  va_start(valist, to);
  char b64encoded[2048];
  char t=va_arg(valist, int);
  while(t != 'f'){
    switch(t){
    case 'c':{
      const char* u=va_arg(valist, const char*);
      sprintf(to, "%s/%s", to, u);
    }
      break;
    case 'e':{
      const uint8_t* b=va_arg(valist, const uint8_t*);
      const size_t bl=va_arg(valist, const size_t);
      buf_to_string(b, bl, b64encoded);
      sprintf(to, "%s/%s", to, b64encoded);
    }
      break;
    default:
      return NULL;
    }
    t=va_arg(valist, int);
  }
  return to;
}


bool check_file_exist(const char* file_name){
  if( access( file_name, F_OK ) != -1){
    return true;
  }
  return false;
}

