#include "include/semaphore.h"
#include "include/globals.h"
#include "include/settings.h"
#include "include/barrier.h"
#include "include/sync.h"

/* the lock will be inited once */
void setup_sync(void)
{
	//int i;
	while((*__synced.val) != 1);
	__setup_sync();
/*	for(i=0;i<MAXTHREADS;i++) {
		while((__threadpool[i].state != E_NONE) && (__threadpool[i].state != E_STOPPED) && (__threadpool[i].leaved != 1));
	}*/
}

void __setup_sync(void)
{
	if(__sync_val_compare_and_swap(__initsync.val, 0, 1) == 0) {
		int i;
//		__DEBUG_PRINT(("tid %d __setup barrier %d \n", __selftid, *(__global_barrier1.val)));
		init_sem(*(__global_barrier1.val), 0, 0);
		for(i=0;i<MAXTHREADS;i++) {
			init_sem(__threadpool[i].lock1, 0, 0);
		}
		for(i=0;i<MAXTHREADS;i++) {
			if(__threadpool[i].state != E_NONE && __threadpool[i].state != E_STOPPED) {
				post_sem(__threadpool[i].lock1, 0);
				break;
			}
		}
	}
}

void setup_mutex_sync(struct mutex_struct *mutex)
{
//	__DEBUG_PRINT(("in setup_mutx_sync tid %d\n", __selftid));
	if(__sync_val_compare_and_swap(&(mutex->inited), 0, 1) == 0) {
		int i;
//	__DEBUG_PRINT(("in setup_mutx_sync2 tid %d\n", __selftid));
		for(i=0;i<MAXTHREADS;i++) {
			init_sem(mutex->locks, i, 0);
		}
		for(i=0;i<MAXTHREADS;i++) {
			if(__threadpool[i].mutex == mutex) {
				__DEBUG_PRINT(("first thread tid %d\n", __selftid));
				post_sem(mutex->locks, i);
				break;
			}
		}
	}
}

void setup_barrier_sync(struct barrier_struct *barrier)
{

	if(__sync_val_compare_and_swap(&(barrier->inited), 0, 1) == 0) {
		barrier->num = 0;
		init_sem(barrier->sema, 0, 0);
	}
}



void v_next_mutex(struct mutex_struct *mutex)
{;
	int i;
	init_sem(mutex->locks, __selftid, 0);
	for(i=0;i<MAXTHREADS;i++) {
		if(__threadpool[i].state == E_STOPPED && __threadpool[i].mutex == mutex) {
			__threadpool[i].state = E_MUTEX;
			post_sem(mutex->locks, i);
			return;
		}
	}
	__DEBUG_PRINT(("********* mutex inted %d\n", mutex->inited));
	mutex->inited = 0;
	
}

void v_next_and_wait(void)
{
	int i;
	for(i=__selftid+1;i<MAXTHREADS;i++) {
//		__DEBUG_PRINT(("tid %d v_next 0 barrier %d \n", __selftid, *(__global_barrier1.val)));
		if(__threadpool[i].state != E_NONE && __threadpool[i].state != E_STOPPED) {
//		__DEBUG_PRINT(("tid %d v_next 0.5 \n", __selftid));
			post_sem(__threadpool[i].lock1, 0);
//			__DEBUG_PRINT(("tid %d barrier value1 %d \n", __selftid, read_sem(*(__global_barrier1.val))));
			
//		__DEBUG_PRINT(("tid %d v_next 1 barrier %d \n", __selftid, *(__global_barrier1.val)));
			wait_sem(*(__global_barrier1.val), 0);
//		__DEBUG_PRINT(("tid %d v_next 2 \n", __selftid));
			__threadpool[__selftid].arrived = 0;
			post_sem(*(__global_barrier1.val), 0);
//			__DEBUG_PRINT(("tid %d barrier value2 %d \n", __selftid, read_sem(*(__global_barrier1.val))));
			return;
		}
	}
	post_sem(*(__global_barrier1.val), 0);
	__threadpool[__selftid].arrived = 0;
}

void leave_sync(void)
{
	int i;
	__threadpool[__selftid].leaved = 1;
	(*__initsync.val) = 0;
	for(i=0;i<MAXTHREADS;i++) {
//		__DEBUG_PRINT(("tiid %d status %d\n", i, __threadpool[i].state));
		while((__threadpool[i].state != E_NONE) && (__threadpool[i].state != E_STOPPED) && (__threadpool[i].leaved == 0) && (__threadpool[i].arrived == 0)) {
//		__DEBUG_PRINT(("tiid %d status %d\n", i, __threadpool[i].state));
		};
	}
	__DEBUG_PRINT(("tid %d has leaved\n", __selftid));
}

void wait_to_enter(void)
{
	wait_sem(__threadpool[__selftid].lock1, 0);
	__threadpool[__selftid].arrived = 1;
	__threadpool[__selftid].leaved = 0;
}

