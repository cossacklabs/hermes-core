/*
 * Copyright (c) 2016 Cossack Labs Limited
 */

#ifndef COMMON_INI_H_
#define COMMON_INI_H_

#include <stdio.h>

#define INI_MAX_LINE_LENGTH 256

typedef int (*ini_node_handler_t)(void* context, const char* section, const char* name, const char* value);
int ini_parse_stream(FILE* stream, ini_node_handler_t handler, void* context);
int ini_parse(const char* filename, ini_node_handler_t handler, void* context);

#endif /* COMMON_INI_H_ */
