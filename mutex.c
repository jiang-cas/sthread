#include "include/mutex.h"
#include "include/globals.h"

int sthread_mutex_init(sthread_mutex_t *mutex, const sthread_mutexattr_t * attr) 
{
	mutex->mutex = (struct mutex_struct *)mvmalloc(sizeof(struct mutex_struct));
	if(mutex->mutex) {
		mutex->mutex->locked = 0;
		mutex->mutex->creator = &(__threadpool[__selftid]); 
		return 0;
	}
	return -1;
}

int sthread_mutex_destroy(sthread_mutex_t *mutex)
{
	if(mutex->mutex && mutex->mutex->locked == 0) {
		mvfree(mutex->mutex);
		return 0;
	}
	return -1;
}

int sthread_mutex_trylock(sthread_mutex_t *mutex)
{
	if(mutex->mutex) {
		if(__sync_val_compare_and_swap(&(mutex->mutex->locked), 0, 1) == 0) return 0;
	}
	return -1;
}

int sthread_mutex_unlock(sthread_mutex_t *mutex)
{
	if(mutex->mutex) {
		mutex->mutex->locked = 0;
		return 0;
	}
	return -1;
}

int sthread_mutex_lock(sthread_mutex_t *mutex)
{
	if(mutex->mutex) {
		if(__sync_val_compare_and_swap(&mutex->mutex->locked, 0, 1) == 0) {
			return 0;
		} else {
			pause();
		}
	}
	return -1;
}
