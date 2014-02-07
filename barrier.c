#include "include/barrier.h"
#include "include/heap.h"
#include "include/sync.h"
#include "include/settings.h"
#include "include/semaphore.h"
#include "include/globals.h"
#include "include/mvspace.h"

int sthread_barrier_init(sthread_barrier_t *barrier, const sthread_barrierattr_t *attr, unsigned int count) 
{
	barrier->barrier = mvshared_malloc(sizeof(struct barrier_struct));
	if(barrier->barrier) {
		barrier->barrier->total = count;
		barrier->barrier->num = 0;
		barrier->barrier->sema = new_sem();
		init_sem(barrier->barrier->sema, 0);
		return 0;
	}
	return -1;
}

int sthread_barrier_wait(sthread_barrier_t *barrier)
{
	if(barrier->barrier) {
		setup_sync();
		wait_to_enter();
		__mvspace_commit();
		__threadpool[__selftid].state = E_STOPPED;
		__threadpool[__selftid].barrier = barrier->barrier;
		__sync_fetch_and_add(&(barrier->barrier->num), 1);
		v_next_and_wait();
		__mvspace_pull();
		if(barrier->barrier->total == barrier->barrier->num)
			post_sem(barrier->barrier->sema);
		wait_sem(barrier->barrier->sema);
		post_sem(barrier->barrier->sema);

		leave_sync();

		return 0;
	}
	return -1;
}

int sthread_barrier_destroy(sthread_barrier_t *barrier)
{
	if(barrier->barrier) {
		del_sem(barrier->barrier->sema);
		mvshared_free(barrier->barrier);
		return 0;
	}
	return -1;
}
