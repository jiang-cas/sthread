#include "include/semaphore.h"
#include "include/globals.h"
#include "include/settings.h"
#include "include/barrier.h"
#include "include/sync.h"

/* the lock will be inited once */
void setup_sync(void)
{
	int i;
	while((*__synced.val) != 1);
	for(i=0;i<MAXTHREADS;i++) {
		while((__threadpool[i].state != E_NONE) && (__threadpool[i].state != E_STOPPED) && (__threadpool[i].leaved != 1));
	}
	if(__sync_val_compare_and_swap(__initsync.val, 0, 1) == 0) {
		int i;
		init_sem(*(__global_barrier1.val), 0);
		for(i=0;i<MAXTHREADS;i++) {
			init_sem(__threadpool[i].lock1, 0);
		}
		for(i=0;i<MAXTHREADS;i++) {
			if(__threadpool[i].state != E_NONE && __threadpool[i].state != E_STOPPED) {
				post_sem(__threadpool[i].lock1);
				break;
			}
		}
	}
}

void setup_mutex_sync(struct mutex_struct *mutex)
{
	if(__sync_val_compare_and_swap(&(mutex->inited), 0, 1) == 0) {
		int i;
		for(i=0;i<MAXTHREADS;i++) {
			init_sem(mutex->lock[i], 0);
		}
		for(i=0;i<MAXTHREADS;i++) {
			if(__threadpool[i].mutex == mutex) {
				post_sem(mutex->lock[i]);
				break;
			}
		}
	}
	
}

void v_next_mutex(struct mutex_struct *mutex)
{
	int i;
	for(i=__selftid+1;i<MAXTHREADS;i++) {
		if(__threadpool[i].state == E_STOPPED && __threadpool[i].mutex == mutex) {
			__threadpool[i].state = E_MUTEX;
			post_sem(mutex->lock[i]);
			return;
		}
	}
	
}

void v_next_and_wait(void)
{
	int i;
	for(i=__selftid+1;i<MAXTHREADS;i++) {
		if(__threadpool[i].state != E_NONE && __threadpool[i].state != E_STOPPED) {
			post_sem(__threadpool[i].lock1);
//			__DEBUG_PRINT(("tid %d barrier value1 %d \n", __selftid, read_sem(*(__global_barrier1.val))));
			wait_sem(*(__global_barrier1.val));
			post_sem(*(__global_barrier1.val));
//			__DEBUG_PRINT(("tid %d barrier value2 %d \n", __selftid, read_sem(*(__global_barrier1.val))));
			return;
		}
	}
	post_sem(*(__global_barrier1.val));
			__DEBUG_PRINT(("tid %d barrier value3 %d \n", __selftid, read_sem(*(__global_barrier1.val))));
}

void leave_sync(void)
{
	__threadpool[__selftid].leaved = 1;
	(*__initsync.val) = 0;
}

void wait_to_enter(void)
{
	wait_sem(__threadpool[__selftid].lock1);
	__threadpool[__selftid].leaved = 0;
}

