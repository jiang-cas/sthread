#include "include/equeue.h"
#include "include/globals.h"

void __init_arrived_waitqueue(struct wait_queue *waits)
{
	int i;
	for(i=0;i<MAXTHREADS;i++) {
		waits->arrived[i] = 0;
	}
}

void __init_common_wait_queue(struct wait_queue *waits) {
	if(!__sync_val_compare_and_swap(&waits->inited, 0 ,1)) {
		int i;
		for(i=0;i<MAXTHREADS;i++) {
			if(__threadpool[i].state != E_NONE) {
				waits->waitqueue[i] = true;
			} else {
				waits->waitqueue[i] = false;
			}
			init_sem(waits->semaqueue[i], 0);
			waits->arrived[i] = 0;
			}
		for(i=0;i<MAXTHREADS;i++) {
			if(waits->waitqueue[i]) {
				v_sem(waits->semaqueue[i]);
				break;
			}
		}
		__init_arrived_waitqueue(waits);
	}
}


void __init_special_wait_queue(struct wait_queue *waits, int type) {
	if(!__sync_val_compare_and_swap(&waits->inited, 0 ,1)) {
		int i;
		for(i=0;i<MAXTHREADS;i++) {
			if(__threadpool[i].state == type) {
				waits->waitqueue[i] = true;
			} else {
				waits->waitqueue[i] = false;
			}
			init_sem(waits->semaqueue[i], 0);
			waits->arrived[i] = 0;
		}
		/* the first task in the waitlist get the sem first */
		for(i=0;i<MAXTHREADS;i++) {
			if(waits->waitqueue[i]) {
				v_sem(waits->semaqueue[i]);
				break;
			}
		}
	}
}

void __wait_for_myturn(struct wait_queue *waits) {
	p_sem(waits->semaqueue[__selftid]);
}

void __spin_sync(struct wait_queue *wq)
{
	int i;
	wq->arrived[__selftid] = 1;
	for(i=0;i<MAXTHREADS;i++) {
		while((__threadpool[i].state != E_NONE) && !(wq->arrived[i]));
	}
	/* bug here ! */
	wq->inited = 0;
}

void __notify_next_nowait(struct wait_queue *waits) 
{
	int i;
	for(i=__selftid+1;i<MAXTHREADS;i++) {
		if(waits->waitqueue[i]) {
			__DEBUG_PRINT(("tid %d v_next_nowait %d \n", __selftid, i));
			v_sem(waits->semaqueue[i]);
			return;
		}
	}
}

void __del_wait_queue(struct wait_queue *waits) 
{
	int i;
	for(i=0;i<MAXTHREADS;i++) {
		del_sem(waits->semaqueue[i]);
	}
	waits->inited = 0;
}

