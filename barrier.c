#include "include/barrier.h"
#include "include/heap.h"
#include "include/sync.h"
#include "include/settings.h"
#include "include/semaphore.h"
#include "include/globals.h"
#include "include/mvspace.h"

int sthread_barrier_init(sthread_barrier_t *barrier, const sthread_barrierattr_t *attr, unsigned int count) 
{
	barrier->barrier = (struct barrier_struct *)mvshared_malloc(sizeof(struct barrier_struct));
	if(barrier->barrier) {
		barrier->barrier->total = count;
		barrier->barrier->num = 0;
		barrier->barrier->sema = new_sem(1);
		init_sem(barrier->barrier->sema, 0, 0);
		barrier->barrier->inited = 1;
		return 0;
	}
	return -1;
}

int sthread_barrier_wait(sthread_barrier_t *barrier)
{
	if(barrier->barrier) {
		setup_sync();
		//setup_barrier_sync(barrier->barrier);
		__DEBUG_PRINT(("tid %d barrier1\n", __selftid));
		wait_to_enter();
		__mvspace_commit();
		__threadpool[__selftid].state = E_STOPPED;
		__threadpool[__selftid].barrier = barrier->barrier;
		__DEBUG_PRINT(("tid %d barrier2\n", __selftid));
		v_next_and_wait();
		setup_barrier_sync(barrier->barrier);
		__sync_fetch_and_add(&(barrier->barrier->num), 1);
		__DEBUG_PRINT(("tid %d barrier3\n", __selftid));
		__mvspace_pull();

		if(barrier->barrier->total == barrier->barrier->num) {
			int i;
			for(i=0;i<MAXTHREADS;i++) {
				if(__threadpool[i].barrier == barrier->barrier)
					__threadpool[i].state = E_NORMAL;
			}
		__DEBUG_PRINT(("tid %d barrier3.5\n", __selftid));
			barrier->barrier->inited = 0;
			post_sem(barrier->barrier->sema, 0);
		}
		__DEBUG_PRINT(("tid %d barrier4\n", __selftid));
		wait_sem(barrier->barrier->sema, 0);
		__DEBUG_PRINT(("tid %d barrier5\n", __selftid));
		post_sem(barrier->barrier->sema, 0);
		__threadpool[__selftid].barrier = NULL;
		__threadpool[__selftid].state = E_NORMAL;

		leave_sync();

		return 0;
	}
	return -1;
}

int sthread_barrier_destroy(sthread_barrier_t *barrier)
{
	if(barrier->barrier) {
		del_sem(barrier->barrier->sema, 0);
		mvshared_free(barrier->barrier);
		return 0;
	}
	return -1;
}
