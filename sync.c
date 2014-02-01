#include <unistd.h>
#include <sys/types.h>
#include "include/mvspace.h"
#include "include/globals.h"
#include "include/sync.h"
#include "include/settings.h"
#include "include/task.h"
#include "include/semaphore.h"



/* the state is determined previously, so there is no need to worry about the coherency */
void sthread_sync_normal(int type)
{
	while(!(*(__synced.synced)));
	init_common_wait_queue(__common_waits.wq, E_NORMAL);
	__DEBUG_PRINT(("tid %d sync1\n", __selftid));
	p_wait_self(__common_waits.wq);
	__DEBUG_PRINT(("tid %d sync2\n", __selftid));
	__mvspace_commit();
	if((type == SIG_JOIN) | (type == SIG_EXIT)) {
		__threadpool[__selftid].state = E_NONE;
	}
	if(type == SIG_MUTEX_LOCK)
		__threadpool[__selftid].state = E_MUTEX;
	if(type == SIG_MUTEX_UNLOCK)
		__threadpool[__selftid].state = E_NORMAL;
		
	v_next_nowait(__common_waits.wq);
	spin_on_normal(__common_waits.wq);
	__DEBUG_PRINT(("tid %d sync3\n", __selftid));
	__mvspace_pull();
}

int sync_mutex_lock(sthread_mutex_t *mutex)
{
	sthread_sync_normal(SIG_MUTEX_LOCK);
	if(mutex->mutex) {
		__init_arrived_waitqueue(mutex->mutex->wq);

		__DEBUG_PRINT(("tid %d inlock\n", __selftid));
		init_special_wait_queue(mutex->mutex->wq, E_MUTEX);
		__threadpool[__selftid].state = E_NONE;
		p_wait_self(mutex->mutex->wq);
		__DEBUG_PRINT(("tid %d sync lock 1\n", __selftid));
	
		p_sem(mutex->mutex->locked);

		__threadpool[__selftid].state = E_MUTEX;

		__DEBUG_PRINT(("tid %d sync lock 2\n", __selftid));
		v_next_nowait(mutex->mutex->wq);

		spin_on_normal(mutex->mutex->wq);

		__DEBUG_PRINT(("tid %d sync lock 3\n", __selftid));
		return 0;
	}
	return -1;
}

int sync_mutex_unlock(sthread_mutex_t *mutex)
{
	sthread_sync_normal(SIG_MUTEX_LOCK);
	if(mutex->mutex) {
		int i;
		__init_arrived_waitqueue(mutex->mutex->wq);
		for(i=__selftid+1;i<MAXTHREADS;i++) {
			if(mutex->mutex->wq->waitqueue[i]) {
				__threadpool[i].state = E_MUTEX;
				__DEBUG_PRINT(("tid %d unlock tid %d\n", __selftid, i));
				break;
			}
		}
		v_sem(mutex->mutex->locked);
		spin_on_normal(mutex->mutex->wq);
		return 0;
	}
	return -1;	
}


int sync_barrier_wait(sthread_barrier_t *barrier)
{
	if(barrier->barrier) {
//		sthread_sync(SIG_BARRIER, barrier);
		return 0;
	}

	return -1;
}
