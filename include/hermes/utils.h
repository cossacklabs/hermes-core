/*
 * Copyright (c) 2015 Cossack Labs Limited
 */

#ifndef HERMES_UTILS_H_
#define HERMES_UTILS_H_

#define HERMES_MAC_LENGTH 16
#define HERMES_ID_LENGTH 24
#define HERMES_TOKEN_LENGTH 64

#ifdef DEBUG
#include <stdio.h>

#define HERMES_CHECK(cond, on_fail_call) do{if(!(cond)){		\
	fprintf(stderr, "%s:%u - error: %s\n",__FILE__,__LINE__,#cond);\
	on_fail_call;}}while(0)

#else

#define HERMES_CHECK(cond, on_fail_call) do{if(!(cond)){on_fail_call;}}while(0)

#endif

#define HERMES_CHECK_PARAM(cond) HERMES_CHECK(cond, HERMES_INVALID_PARAM)

#endif /* HERMES_UTILS_H_ */
