#ifndef EQUEUE_H
#define EQUEUE_H

#include "settings.h"
#include "semaphore.h"


struct wait_queue
{
	bool waitqueue[MAXTHREADS];
	sem_t *semaqueue[MAXTHREADS];
	sem_t *barrier;
	int inited;
};	

void init_common_wait_queue(struct wait_queue *waits, int type);

void p_wait_self(struct wait_queue *waits);

void v_next_wait(struct wait_queue *waits);

void del_wait_queue(struct wait_queue *waits);

#endif
