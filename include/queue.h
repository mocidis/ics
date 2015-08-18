//
//  queue_type.h
//  CallLogicManager
//
//  Created by Tung HOANG on 12/22/13.
//  Copyright (c) 2013 TungMacBook. All rights reserved.
//

#ifndef _CONCURRENT_QUEUE_H_
#define _CONCURRENT_QUEUE_H_
#include <pthread.h>
#include <pjlib.h>
#define QUEUE_CAP 100

#define QE_EXIT 18834
typedef struct __event {
	int is_used;
	int type;
	void *data;
	void *p_user;
} queue_event_t;

/* OBJECT POOL */
typedef struct {
	int is_used;
	char *data;
} opool_item_t;

typedef struct {
	opool_item_t *items;
	char *data;
	int capacity;
	int data_size;
	int count;
	pj_mutex_t *p_mutex;
} opool_t;

/* QUEUE EVENT POOL */
typedef struct {
	queue_event_t *p_queue_events;
	char *__p_qe_data;
	int capacity;
	int data_size;
	int count;
	pj_mutex_t *p_mutex;
} qepool_t;

typedef struct __queue queue_t;
struct __queue
{
	qepool_t qepool;
	opool_t opool;	

	void **buffer;
	int capacity;
	int size;
	int in;
	int out;
	pthread_mutex_t mutex;
	pthread_cond_t cond_full;
	pthread_cond_t cond_empty;
};
#define QUEUE_INITIALIZER(buffer) { (buffer), sizeof(buffer) / sizeof((buffer)[0]), 0, 0, 0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER}
void queue_enqueue(queue_t *queue, void *value);
void *queue_dequeue(queue_t *queue);
int queue_size(queue_t *queue);
void queue_init(queue_t *queue, int cap, int data_size, pj_pool_t *p_mempool);

void qepool_init(qepool_t *p_qepool, int capacity, int data_size, pj_pool_t *p_mempool);
queue_event_t *qepool_get(qepool_t *p_qepool);
void qepool_free(qepool_t *p_qepool, queue_event_t *p_event);

void opool_init(opool_t *p_opool, int capacity, int data_size, pj_pool_t *p_mempool);
opool_item_t *opool_get(opool_t *p_opool);
void opool_free(opool_t *p_opool, opool_item_t *p_item);

#endif
