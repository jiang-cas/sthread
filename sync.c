#include "include/semaphore.h"
#include "include/globals.h"
#include "include/settings.h"

/* the lock will be inited once */
void setup_sync(void)
{
	while((*__synced.val) != 1);
	while((__threadpool[__selftid].state != E_NONE) && (__threadpool[__selftid].state != E_STOPPED) && (__threadpool[__selftid].leaved != 1));

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

void v_next_and_wait(void)
{
	int i;
	for(i=__selftid+1;i<MAXTHREADS;i++) {
		if(__threadpool[i].state != E_NONE && __threadpool[i].state != E_STOPPED) {
			post_sem(__threadpool[i].lock1);
			wait_sem(*(__global_barrier1.val));
			post_sem(*(__global_barrier1.val));
			return;
		}
	}
	post_sem(*(__global_barrier1.val));
}

void leave_sync(void)
{
	__threadpool[__selftid].leaved = 1;
	(*__initsync.val) = 0;
}

