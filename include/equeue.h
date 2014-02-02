#ifndef EQUEUE_H
#define EQUEUE_H

#include "settings.h"
#include "semaphore.h"


struct wait_queue
{
	bool waitqueue[MAXTHREADS];
	sem_t *semaqueue[MAXTHREADS];
	bool arrived[MAXTHREADS];
	int inited;
};	

void __init_common_wait_queue(struct wait_queue *waits);
void __init_special_wait_queue(struct wait_queue *waits, int type);
void __wait_for_myturn(struct wait_queue *waits);
void __spin_sync(struct wait_queue *waits);
void __notify_next_nowait(struct wait_queue *waits);
void __del_wait_queue(struct wait_queue *waits);
void __init_arrived_waitqueue(struct wait_queue *waits);

#endif
