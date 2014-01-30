#include <unistd.h>
#include <sys/types.h>
#include "include/mvspace.h"
#include "include/globals.h"
#include "include/sync.h"
#include "include/settings.h"
#include "include/task.h"
#include "include/semaphore.h"


void sthread_sync_normal(void)
{
	while(!(*(__synced.synced)));
	init_common_wait_queue(__common_waits.wq, E_NORMAL);
	__DEBUG_PRINT(("tid %d sync1\n", __selftid));
	p_wait_self(__common_waits.wq);
	__DEBUG_PRINT(("tid %d sync2\n", __selftid));
	__mvspace_commit();
	v_next_wait(__common_waits.wq);
	__DEBUG_PRINT(("tid %d sync3\n", __selftid));
	__mvspace_pull();
}

int sync_mutex_lock(sthread_mutex_t *mutex)
{
	if(mutex->mutex) {
	//	sthread_sync(SIG_MUTEX_LOCK, mutex);
		return 0;
	}
	return -1;
}

int sync_mutex_unlock(sthread_mutex_t *mutex)
{
	if(mutex->mutex) {
//		sthread_sync(SIG_MUTEX_UNLOCK, mutex);
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
