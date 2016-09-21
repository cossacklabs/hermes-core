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

#ifndef SRPC_THREAD_POOL_H_
#define SRPC_THREAD_POOL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct thread_pool_t thread_pool_t;
  typedef int32_t thread_pool_status_t; 
  
#define THREAD_POOL_SUCCESS 0
#define THREAD_POOL_FAIL -1
#define THREAD_POOL_INVALID_PARAM -2
#define THREAD_POOL_LOCK_FAILURE -3
#define THREAD_POOL_THREAD_FAILURE -4
#define THREAD_POOL_QUEUE_FULL -5

  thread_pool_t *thread_pool_create(uint32_t threads_count, uint32_t queue_size);
  thread_pool_status_t thread_pool_add(thread_pool_t *pool, void (*routine)(void *), void *arg);
  thread_pool_status_t thread_pool_destroy(thread_pool_t *pool);

#ifdef __cplusplus
}
#endif

#endif /* _THREADPOOL_H_ */

