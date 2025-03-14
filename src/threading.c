#include <pthread.h>
#include <stdio.h>
#include "threading.h"

#define THREAD_COUNT 8

pthread_mutex_t queue_mutex;
pthread_cond_t queue_cond;
pthread_t threads[THREAD_COUNT];
Request request_queue[265];
int request_count = 0;
int curr_backend = 0;

void execute_request(Request *request) {
  request->request_function(request->value, request->sockfd, request->backend);
}

void *start_thread(void *args) {
  while (1) {
    Request request;

    pthread_mutex_lock(&queue_mutex);

    while(request_count == 0) {
      pthread_cond_wait(&queue_cond, &queue_mutex);
    }

    request = request_queue[0];
    request.backend = backends[curr_backend % num_backends];
    int i;

    for (i = 0; i < request_count - 1; i++) {
      request_queue[i] = request_queue[i + 1];
    }

    request_count--;
    curr_backend++;
    pthread_mutex_unlock(&queue_mutex);
    execute_request(&request);
  }
}

void add_request(Request request) {
  pthread_mutex_lock(&queue_mutex);

  request_queue[request_count] = request;
  request_count++;

  pthread_mutex_unlock(&queue_mutex);
  pthread_cond_signal(&queue_cond);
}

void create_thread_pool() {
  pthread_mutex_init(&queue_mutex, NULL);
  pthread_cond_init(&queue_cond, NULL);

  int i;

  for (i = 0; i < THREAD_COUNT; i++) {
    if (pthread_create(&threads[i], NULL, &start_thread, NULL) != 0) {
      perror("failed to create thread");
    }
  }
}

void destroy_thread_pool() {
  int i;

  for (i = 0; i < THREAD_COUNT; i++) {
    if (pthread_join(threads[i], NULL) != 0) {
      perror("failed to join thread");
    }
  }

  pthread_mutex_destroy(&queue_mutex);
  pthread_cond_destroy(&queue_cond);
}