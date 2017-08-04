/*
* Copyright (c) 2017 Cossack Labs Limited
*
* This file is a part of Hermes-core.
*
* Hermes-core is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Hermes-core is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Hermes-core.  If not, see <http://www.gnu.org/licenses/>.
*
*/


#ifndef HERMES_ERRORS_H_
#define HERMES_ERRORS_H_

#include <stdint.h>

#define HM_SUCCESS 0
#define HM_ERR 0x000f0000
#define HM_FAIL (HM_ERR|1)
#define HM_INVALID_PARAMETER (HM_ERR|2)
#define HM_BUFFER_TOO_SMALL (HM_ERR|3)
#define HM_BAD_ALLOC (HM_ERR|4)
#define HM_NOT_IMPLEMENTED (HM_ERR|0x8000)

typedef uint32_t hermes_status_t;

#ifdef DEBUG
#define DEBUGINFO(level, msg) {}while(false)
#else
#define DEBUGINFO(level, msg) {}while(false)
#endif

#define HERMES_CHECK_IN_PARAM_RET_NULL(p) \
  do{                                     \
    if(!p){                               \
      return NULL;                        \
    }                                     \
  }while(0)

#define HERMES_CHECK_IN_PARAM(p)       \
  do{                                           \
    if(!p){                                     \
      return HM_INVALID_PARAMETER;              \
    }                                           \
  }while(0)

#endif /* HERMES_ERRORS_H_ */
