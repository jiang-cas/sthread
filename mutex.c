#include "include/mutex.h"
#include <unistd.h>
#include "include/globals.h"
#include "include/sync.h"
#include "include/semaphore.h"

int sthread_mutex_init(sthread_mutex_t *mutex, const sthread_mutexattr_t * attr) 
{
	mutex->mutex = mvshared_malloc(sizeof(struct mutex_struct));
	if(mutex->mutex) {
		int i;	
		mutex->mutex->wq = (struct wait_queue *)mvshared_malloc(sizeof(struct wait_queue));
		mutex->mutex->locked = new_sem();
		for(i=0;i<MAXTHREADS;i++) {
			mutex->mutex->wq->semaqueue[i] = new_sem();
		}
		mutex->mutex->wq->inited = 0;
		init_sem(mutex->mutex->locked, 1);
		return 0;
	}
	return -1;
}

int sthread_mutex_destroy(sthread_mutex_t *mutex)
{
	if(mutex->mutex) {
		int i;
		for(i=0;i<MAXTHREADS;i++) {
			free_sem(mutex->mutex->wq->semaqueue[i]);
		}
		mvshared_free(mutex->mutex->wq);
		free_sem(mutex->mutex->locked);
		mvshared_free(mutex->mutex);
		return 0;
	}
	return -1;
}

int sthread_mutex_trylock(sthread_mutex_t *mutex)
{
	/*trylock does not neccessarily acquired the lock, so there is no need to sync */
	if(mutex->mutex) {
		if(read_sem(mutex->mutex->locked) > 0)
			p_sem(mutex->mutex->locked);
			return 0;
	}
	return -1;
}

int sthread_mutex_unlock(sthread_mutex_t *mutex)
{
	return sync_mutex_unlock(mutex);
}

int sthread_mutex_lock(sthread_mutex_t *mutex)
{
	return sync_mutex_lock(mutex);
}
