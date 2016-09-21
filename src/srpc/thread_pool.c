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

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include <hermes/utils.h>
#include <hermes/thread_pool.h>

#define MAX_THREADS 64
#define MAX_QUEUE_SIZE 50000

typedef struct {
    void (*function)(void *);
    void *argument;
} job_t;

typedef struct thread_pool_t {
  pthread_mutex_t lock;
  pthread_cond_t notify;
  pthread_t *threads;
  job_t *queue;
  uint32_t threads_count;
  uint32_t queue_size;
  uint32_t queue_head;
  uint32_t queue_tail;
  uint32_t queue_count;
  uint32_t started_threads;
  uint32_t is_shuting_down;
} thread_pool_t;

static void *thread_pool_thread(void *threadpool);

thread_pool_status_t thread_pool_free(thread_pool_t *pool){
  HERMES_CHECK(pool, return THREAD_POOL_INVALID_PARAM);
  if(pool->threads) {
    free(pool->threads);
    free(pool->queue);
    pthread_mutex_lock(&(pool->lock));
    pthread_mutex_destroy(&(pool->lock));
    pthread_cond_destroy(&(pool->notify));
  }
  free(pool);    
  return THREAD_POOL_SUCCESS;
}

int thread_pool_destroy(thread_pool_t *pool)
{
  HERMES_CHECK(pool, return THREAD_POOL_INVALID_PARAM);
  HERMES_CHECK(!pool->is_shuting_down, return THREAD_POOL_SUCCESS);
  pool->is_shuting_down=1;
  HERMES_CHECK(pthread_mutex_lock(&(pool->lock)) == 0, return THREAD_POOL_LOCK_FAILURE);
  HERMES_CHECK(pthread_cond_broadcast(&(pool->notify)) == 0 && pthread_mutex_unlock(&(pool->lock)) == 0, return THREAD_POOL_LOCK_FAILURE);

  int i;
  thread_pool_status_t status = THREAD_POOL_SUCCESS;
  for(i = 0; i < pool->threads_count; i++) {
    if(pthread_join(pool->threads[i], NULL) != 0) {
      status = THREAD_POOL_THREAD_FAILURE;
    }
  }
  HERMES_CHECK(status==THREAD_POOL_SUCCESS, thread_pool_free(pool));
  return status;
}

thread_pool_t *thread_pool_create(uint32_t threads_count, uint32_t queue_size)
{
  HERMES_CHECK(threads_count <= MAX_THREADS && queue_size <= MAX_QUEUE_SIZE, return NULL);
  thread_pool_t *pool = (thread_pool_t *)malloc(sizeof(thread_pool_t));
  HERMES_CHECK(pool, return NULL);

  pool->threads_count = 0;
  pool->queue_size = queue_size;
  pool->queue_head = pool->queue_tail = pool->queue_count = 0;
  pool->started_threads = 0;
  pool->is_shuting_down=0;

  pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * threads_count);
  pool->queue = (job_t *)malloc(sizeof(job_t) * queue_size);

  HERMES_CHECK(pool->threads && pool->queue, thread_pool_free(pool); return NULL;);
  HERMES_CHECK(pthread_mutex_init(&(pool->lock), NULL) == 0 && pthread_cond_init(&(pool->notify), NULL) == 0, thread_pool_free(pool); return NULL;);

  int i;
  for(i = 0; i < threads_count; i++) {
    if(pthread_create(&(pool->threads[i]), NULL, thread_pool_thread, (void*)pool) != 0) {
      thread_pool_destroy(pool);
      return NULL;
    }
    pool->threads_count++;
    pool->started_threads++;
  }
    return pool;
}

int thread_pool_add(thread_pool_t *pool, void (*function)(void *), void *argument)
{
    HERMES_CHECK(pool && function, return THREAD_POOL_INVALID_PARAM);
    HERMES_CHECK(pthread_mutex_lock(&(pool->lock)) == 0, return THREAD_POOL_LOCK_FAILURE);

    HERMES_CHECK(pool->queue_count != pool->queue_size, HERMES_CHECK(pthread_mutex_unlock(&pool->lock) == 0, return THREAD_POOL_LOCK_FAILURE); return THREAD_POOL_QUEUE_FULL);
    HERMES_CHECK(!(pool->is_shuting_down), HERMES_CHECK(pthread_mutex_unlock(&pool->lock) == 0, return THREAD_POOL_LOCK_FAILURE); return THREAD_POOL_QUEUE_FULL);

    uint32_t next = pool->queue_tail + 1;
    next = (next == pool->queue_size) ? 0 : next;
    pool->queue[pool->queue_tail].function = function;
    pool->queue[pool->queue_tail].argument = argument;
    pool->queue_tail = next;
    pool->queue_count += 1;

    HERMES_CHECK(pthread_cond_signal(&(pool->notify)) == 0, HERMES_CHECK(pthread_mutex_unlock(&pool->lock) == 0, return THREAD_POOL_LOCK_FAILURE); return THREAD_POOL_LOCK_FAILURE);
    HERMES_CHECK(pthread_mutex_unlock(&pool->lock) == 0, return THREAD_POOL_LOCK_FAILURE);
    
    return THREAD_POOL_SUCCESS;
}

static void *thread_pool_thread(void *thread_pool)
{
    thread_pool_t *pool = (thread_pool_t *)thread_pool;
    job_t job;

    for(;;) {
        pthread_mutex_lock(&(pool->lock));

        while((pool->queue_count == 0) && (!pool->is_shuting_down)) {
	  pthread_cond_wait(&(pool->notify), &(pool->lock));
        }
	
        if(pool->is_shuting_down && pool->queue_count == 0) {
	  break;
	}
	
	job.function = pool->queue[pool->queue_head].function;
	job.argument = pool->queue[pool->queue_head].argument;
	pool->queue_head += 1;
	pool->queue_head = (pool->queue_head == pool->queue_size) ? 0 : pool->queue_head;
	pool->queue_count -= 1;
	
	pthread_mutex_unlock(&(pool->lock));

        (*(job.function))(job.argument);
    }

    pool->started_threads--;

    pthread_mutex_unlock(&(pool->lock));
    pthread_exit(NULL);
    return(NULL);
}
