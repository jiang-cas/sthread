#include "include/mutex.h"
#include <unistd.h>
#include "include/globals.h"
#include "include/sync.h"
#include "include/mvspace.h"
#include "include/semaphore.h"
#include <sys/mman.h>

int sthread_mutex_init(sthread_mutex_t *mutex, const sthread_mutexattr_t * attr) 
{
	mutex->mutex = (struct mutex_struct *)mvshared_malloc(sizeof(struct mutex_struct));
	if(mutex->mutex) {
		int i;
		for(i=0;i<MAXTHREADS;i++)
			mutex->mutex->lock[i] = new_sem();
		return 0;
	}
	return -1;
}

int sthread_mutex_destroy(sthread_mutex_t *mutex)
{
	if(mutex->mutex) {
		int i;
		for(i=0;i<MAXTHREADS;i++)
			del_sem(mutex->mutex->lock[i]);
		mvshared_free(mutex->mutex);
		return 0;
	}
	return -1;
}

int sthread_mutex_lock(sthread_mutex_t *mutex)
{
	if(mutex->mutex) {
		setup_sync();
		__DEBUG_PRINT(("tid %d lock0 \n", __selftid));
		wait_to_enter();
		__mvspace_commit();
		__threadpool[__selftid].state = E_STOPPED;
		__threadpool[__selftid].mutex = mutex->mutex;
		mutex->mutex->inited = 0;

		__DEBUG_PRINT(("tid %d lock1 \n", __selftid));
		v_next_and_wait();
		__mvspace_pull();

		__DEBUG_PRINT(("tid %d lock2 \n", __selftid));

		setup_mutex_sync(mutex->mutex);	
		wait_sem(mutex->mutex->lock[__selftid]);
		__threadpool[__selftid].state = E_MUTEX;

		__DEBUG_PRINT(("tid %d lock3 \n", __selftid));
	
		leave_sync();
		return 0;
	}
	return -1;
}

int sthread_mutex_unlock(sthread_mutex_t *mutex)
{
	if(mutex->mutex) {
		setup_sync();
		__DEBUG_PRINT(("tid %d unlock0 \n", __selftid));
		wait_to_enter();
		__mvspace_commit();
		__threadpool[__selftid].state = E_NORMAL;
		__DEBUG_PRINT(("tid %d unlock1 \n", __selftid));
		v_next_and_wait();
		__mvspace_pull();

		v_next_mutex(mutex->mutex);
		__DEBUG_PRINT(("tid %d unlock2 \n", __selftid));

		leave_sync();
		__DEBUG_PRINT(("tid %d unlock3 \n", __selftid));
		return 0;
	}
	return -1;
}

