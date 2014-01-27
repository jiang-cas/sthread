#include "include/mutex.h"
#include <unistd.h>
#include "include/globals.h"
#include "include/sync.h"
#include "include/semaphore.h"

int sthread_mutex_init(sthread_mutex_t *mutex, const sthread_mutexattr_t * attr) 
{
	mutex->mutex = new_sem();
	if(mutex->mutex) {
		init_sem(mutex->mutex, 1);
		return 0;
	}
	return -1;
}

int sthread_mutex_destroy(sthread_mutex_t *mutex)
{
	if(mutex->mutex) {
		del_sem(mutex->mutex);
		return 0;
	}
	return -1;
}

int sthread_mutex_trylock(sthread_mutex_t *mutex)
{
	/*trylock does not neccessarily acquired the lock, so there is no need to sync */
	if(mutex->mutex) {
		if(read_sem(mutex->mutex) > 0)
			p_sem(mutex->mutex);
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
