#include "include/mutex.h"
#include <unistd.h>
#include "include/globals.h"
#include "include/sync.h"

int sthread_mutex_init(sthread_mutex_t *mutex, const sthread_mutexattr_t * attr) 
{
	mutex->mutex = (struct mutex_struct *)mvshared_malloc(sizeof(struct mutex_struct));
	if(mutex->mutex) {
		mutex->mutex->locked = 0;
		mutex->mutex->creator = __currenttask(); 
		return 0;
	}
	return -1;
}

int sthread_mutex_destroy(sthread_mutex_t *mutex)
{
	if(mutex->mutex && mutex->mutex->locked == 0) {
		mvshared_free(mutex->mutex);
		return 0;
	}
	return -1;
}

int sthread_mutex_trylock(sthread_mutex_t *mutex)
{
	/*trylock does not neccessarily acquired the lock, so there is no need to sync */
	if(mutex->mutex) {
		if(__sync_val_compare_and_swap(&(mutex->mutex->locked), 0, 1) == 0) return 0;
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
