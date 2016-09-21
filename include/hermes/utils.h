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
#define HERMES_LOG(component, msg) fprintf(stdout, "log: %s - %s\n", component, msg);

#endif /* HERMES_UTILS_H_ */
