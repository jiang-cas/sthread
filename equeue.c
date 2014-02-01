#include "include/equeue.h"
#include "include/globals.h"

void __init_arrived_waitqueue(struct wait_queue *waits)
{
	int i;
	for(i=0;i<MAXTHREADS;i++) {
		waits->arrived[i] = 0;
	}
}

void init_common_wait_queue(struct wait_queue *waits, int type) {
	int i;
	if(!__sync_val_compare_and_swap(&waits->inited, 0 ,1)) {
		waits->total = 0;
		waits->p_count = 0;
		if(type == E_NORMAL) {
			for(i=0;i<MAXTHREADS;i++) {
				if(__threadpool[i].state != E_NONE) {
					waits->waitqueue[i] = true;
					(waits->total)++;
				} else {
					waits->waitqueue[i] = false;
				}
				init_sem(waits->semaqueue[i], 0);
				waits->arrived[i] = 0;
			}
		} else {
			for(i=0;i<MAXTHREADS;i++) {
				if(__threadpool[i].state == type) {
					waits->waitqueue[i] = true;
					(waits->total)++;
				} else {
					waits->waitqueue[i] = false;
				}
				init_sem(waits->semaqueue[i], 0);
				waits->arrived[i] = 0;
			}
		}
		/* the first task in the waitlist get the sem first */
		for(i=0;i<MAXTHREADS;i++) {
			if(waits->waitqueue[i]) {
				v_sem(waits->semaqueue[i]);
				break;
			}
		}
		init_sem(waits->barrier, 0);
		init_sem(waits->p_barrier, 0);
		__init_arrived_waitqueue(waits);
		
	}
	
/*	__DEBUG_PRINT(("waitqueue \n"));
	for(i=0;i<MAXTHREADS;i++) 
		__DEBUG_PRINT(("%d ", waits->waitqueue[i]));
	__DEBUG_PRINT(("\n"));*/
}


void init_special_wait_queue(struct wait_queue *waits, int type) {
	int i;
	if(!__sync_val_compare_and_swap(&waits->inited, 0 ,1)) {
		for(i=0;i<MAXTHREADS;i++) {
			if(__threadpool[i].state == type) {
				waits->waitqueue[i] = true;
				(waits->total)++;
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
		init_sem(waits->barrier, 0);
		init_sem(waits->p_barrier, 0);
	}
	/*	__DEBUG_PRINT(("waitqueue \n"));
		for(i=0;i<MAXTHREADS;i++) 
		__DEBUG_PRINT(("%d ", waits->waitqueue[i]));
	__DEBUG_PRINT(("\n"));*/
}
void p_wait_self(struct wait_queue *waits) {
	__sync_fetch_and_add(&(waits->p_count), 1);
//	__DEBUG_PRINT(("p_wait\n"));
//	__DEBUG_PRINT(("wait on p total %d, p_count %d\n", waits->total, waits->p_count));
	if(waits->p_count == waits->total) {
		waits->inited = 0;
		v_sem(waits->p_barrier);
	}
	p_sem(waits->semaqueue[__selftid]);
}

void wait_on_p(struct wait_queue *waits)
{
//	__DEBUG_PRINT(("wait on p\n"));
//	__DEBUG_PRINT(("wait on p total %d, p_count %d\n", waits->total, waits->p_count));
	if(waits->p_count == waits->total) {
		waits->inited = 0;
		v_sem(waits->p_barrier);
	}
	p_sem(waits->p_barrier);
	v_sem(waits->p_barrier);
}

void spin_on_normal(struct wait_queue *wq)
{
	int i;
	wq->arrived[__selftid] = 1;
	for(i=0;i<MAXTHREADS;i++) {
		while((__threadpool[i].state != E_NONE) && !(wq->arrived[i])) {
//		__DEBUG_PRINT(("tid %d spin task %d \n", __selftid, i));
	}
	}
}

void v_next_wait(struct wait_queue *waits) 
{
	int i;
	for(i=__selftid+1;i<MAXTHREADS;i++) {
		if(waits->waitqueue[i]) {
			__DEBUG_PRINT(("tid %d v_next %d \n", __selftid, i));
			v_sem(waits->semaqueue[i]);

			p_sem(waits->barrier);
			v_sem(waits->barrier);
			return;
		}
	}
	/*it is the last task in the waitqueue */
	waits->inited = 0;	
	v_sem(waits->barrier);
	
}

void v_next_nowait(struct wait_queue *waits) 
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
void del_wait_queue(struct wait_queue *waits) 
{
	int i;
	for(i=0;i<MAXTHREADS;i++) {
		del_sem(waits->semaqueue[i]);
	}
	waits->inited = 0;
}

