#include <unistd.h>
#include <sys/types.h>
#include "include/mvspace.h"
#include "include/globals.h"
#include "include/sync.h"
#include "include/settings.h"
#include "include/task.h"
#include "include/semaphore.h"

/* the first task has semval == 1, others semval == 0 */
void __init_sync_sem(void)
{
	if(__selftid == 0)
		init_sem(__currenttask()->sync.commit_sem, 1);
	else
		init_sem(__currenttask()->sync.commit_sem, 0);

	if(__selftid == __lasttask()->tid)
		init_sem(__currenttask()->sync.pull_sem, 1);
	else 
		init_sem(__currenttask()->sync.pull_sem, 0);
}

void __wakeup_all(void)
{
	int i;
	for(i=0;i<MAXTHREADS;i++)
	{
		if(__threadpool[i].inlist) {
			v_sem(__threadpool[i].sync.pull_sem);
		}
	}
}

#define SYNC_STATE(x) ((x)->sync.sync_state)

void sthread_sync(int type, void *item)
{

	SYNC_STATE(__currenttask()) = SYNC_IN;

	if(type == SIG_NORMAL) {
		while(!__pre_have_committed_or_suspended());
		/* all tasks before have committed */
		__mvspace_commit();
		SYNC_STATE(__currenttask()) = SYNC_COMMITTED;
		while(!__all_have_committed());
		__mvspace_pull();
		SYNC_STATE(__currenttask()) = SYNC_OUT;
	}	

	
/*	if(type == SIG_MUTEX_LOCK && item) {
		sthread_mutex_t *mutex = (sthread_mutex_t *)item;
		while(!__pre_have_committed_or_suspended());
		__mvspace_commit();
		while(1) {
			if(__pre_have_committed()) {
				__get_the_lock();
				SYNC_STATE(__currenttask()) = SYNC_COMMITTED;
			} else {
				SYNC_STATE(__currenttask()) = SYNC_SUSPENDED;
				__get_the_lock();
			}
		}


		if(read_sem(mutex->mutex) == 0 && read_sem(__currenttask()->sync.commit_sem) == 0) {
			if(__nexttask(__currenttask()))
				v_sem(__nexttask(__currenttask())->sync.commit_sem);
		} else {
			p_sem(__currenttask()->sync.commit_sem);
			p_sem(mutex->mutex);
			__mvspace_commit();
			if(__nexttask(__currenttask()))
				v_sem(__nexttask(__currenttask())->sync.commit_sem);
		}
	}

	if(type == SIG_NORMAL) {
		p_sem(__currenttask()->sync.commit_sem);
		__mvspace_commit();
		if(__nexttask(__currenttask()))
			v_sem(__nexttask(__currenttask())->sync.commit_sem);
	}

*/	/* the threads reach this point inorder, so that they can ask for the lock inorder */
/*	if(type == SIG_MUTEX_LOCK && item) {
		sthread_mutex_t *mutex = (sthread_mutex_t *)item;
		p_sem(mutex->mutex);
		if(__nexttask(__currenttask()))
			v_sem(__nexttask(__currenttask())->sync.commit_sem);
	}
	if(type == SIG_MUTEX_UNLOCK && item) {
		sthread_mutex_t *mutex = (sthread_mutex_t *)item;
		v_sem(mutex->mutex);
	}
*/	/*if(type == SIG_BARRIER && item) {
		sthread_barrier_t *barrier = (sthread_barrier_t *)item;
		barrier_spin = 1;
		if(__sync_add_and_fetch(&(barrier->barrier->count), 1) == barrier->totalcount) {
			value.sival_int = SIG_BARRIER;
			ret = sigqueue(__nexttask(__currenttask())->pid, SIGUSR1, value);
			if(ret) 
				perror("sigqueue");
		} else {

			while(barrier_spin) {
				pause();
			}
		}
	}*/

/*	__DEBUG_PRINT(("tid %d , sync3\n", __selftid));	

	p_sem(__currenttask()->sync.pull_sem);

	if(__selftid == __lasttask()->tid)
		__wakeup_all();
	
	__DEBUG_PRINT(("tid %d , sync5\n", __selftid));	

	__mvspace_pull();
*/
}

int sync_mutex_lock(sthread_mutex_t *mutex)
{
	if(mutex->mutex) {
		sthread_sync(SIG_MUTEX_LOCK, mutex);
		return 0;
	}
	return -1;
}

int sync_mutex_unlock(sthread_mutex_t *mutex)
{
	if(mutex->mutex) {
		sthread_sync(SIG_MUTEX_UNLOCK, mutex);
		return 0;
	}
	return -1;	
}


int sync_barrier_wait(sthread_barrier_t *barrier)
{
	if(barrier->barrier) {
		sthread_sync(SIG_BARRIER, barrier);
		return 0;
	}

	return -1;
}
