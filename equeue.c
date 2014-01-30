#include "include/equeue.h"
#include "include/globals.h"

void init_common_wait_queue(struct wait_queue *waits, int type) {
	int i;
	if(!__sync_val_compare_and_swap(&waits->inited, 0 ,1)) {
		if(type == E_NORMAL) {
			for(i=0;i<MAXTHREADS;i++) {
				if(__threadpool[i].state != E_NONE) {
					waits->waitqueue[i] = true;
				} else {
					waits->waitqueue[i] = false;
				}
				init_sem(waits->semaqueue[i], 0);
			}
		} else {
			for(i=0;i<MAXTHREADS;i++) {
				if(__threadpool[i].state == type) {
					waits->waitqueue[i] = true;
				} else {
					waits->waitqueue[i] = false;
				}
				init_sem(waits->semaqueue[i], 0);
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
	}
/*	__DEBUG_PRINT(("waitqueue \n"));
	for(i=0;i<MAXTHREADS;i++) 
		__DEBUG_PRINT(("%d ", waits->waitqueue[i]));
	__DEBUG_PRINT(("\n"));*/

}

void p_wait_self(struct wait_queue *waits) {
	p_sem(waits->semaqueue[__selftid]);
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

void del_wait_queue(struct wait_queue *waits) 
{
	int i;
	for(i=0;i<MAXTHREADS;i++) {
		del_sem(waits->semaqueue[i]);
	}
	waits->inited = 0;
}

