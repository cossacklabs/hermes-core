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

#ifndef HERMES_COMMON_CRYPT_UTILS_PIPE_H_
#define HERMES_COMMON_CRYPT_UTILS_PIPE_H_

#define HM_SC_PARAM_MAGIC 0x0000000012345678
#define HM_SC_PARAM_INT HM_SC_MAGIC+1
#define HM_SC_PARAM_STR HM_SC_MAGIC+2
#define HM_SC_PARAM_BUF HM_SC_MAGIC+3

int write_data_to_pipe(int pipe, const uint8_t* data, uint32_t data_length);
int read_data_from_pipe(int pipe, uint8_t* data, uint32_t data_length);
int read_pipe(int pipe, ...);
int write_pipe(int pipe, ...);

#define HM_SC_INT(p) HM_SC_PARAM_MAGIC, HM_SC_PARAM_INT, (p)
#define HM_SC_STR(p) HM_SC_PARAM_MAGIC, HM_SC_PARAM_STR, (p) 
#define HM_SC_BUF(p, len) HM_SC_PARAM_MAGIC, HM_SC_PARAM_BUF, (p), (len) 

#define HM_SC_READ(pipe, ...)                   \
  read_pipe(pipe, __VA_ARGS__, (void*)NULL)

#define HM_SC_WRITE(pipe, ...)                  \
  write_pipe(pipe, __VA_ARGS__, (void*)NULL)


#endif /* HERMES_COMMON_CRYPT_UTILS_PIPE_H_ */
