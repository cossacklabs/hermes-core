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


#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

char* buf_to_string(const uint8_t* buf, const size_t buf_length, char* string);
size_t string_to_buf(const char* string, uint8_t* buf);

char* build_path(char* to, ...);

#define BUILD_PATH(to, ...) build_path(to, __VA_ARGS__, NULL)

char* build_typed_path(char* to, ...);

#define C(a) 'c', a
#define E(b, bl) 'e', b, bl

#define BUILD_TYPED_PATH(to, ...) build_typed_path(to, __VA_ARGS__, 'f', NULL)

int remove_directory(const char *path);
int create_directory(const char *path);
bool check_file_exist(const char* file_name);


uint32_t read_whole_file(const char* filename, uint8_t** buf, size_t* buf_length);
uint32_t write_whole_file(const char* filename, const uint8_t* buf, const size_t buf_length);


#endif //UTILS_H
