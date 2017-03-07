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

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

#define HM_ERROR 0
#define HM_WARNING 1
#define HM_INFO 2

#ifndef HM_LOGLEVEL
#define HM_LOGLEVEL HM_INFO
#endif

#define HM_LOG(level, format, ...)                 \
  do{                                              \
    if(level<=HM_LOGLEVEL){                        \
       fprintf(stderr, format, __VA_ARGS__);       \
     }                                             \
  }while(false)                                    \

#endif /* COMMON_UTILS_H_ */
