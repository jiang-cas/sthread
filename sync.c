#include <unistd.h>
#include <sys/types.h>
#include "include/mvspace.h"
#include "include/globals.h"
#include "include/sync.h"
#include "include/settings.h"



int commit_spin;
int pull_spin;
int barrier_spin;


void __sigusr1_handler(int signo, siginfo_t *si, void *ucontext)
{
	if(si->si_code == SI_QUEUE) {
		if(si->si_int == SIG_COMMIT) {
			commit_spin = 0;
		} else if(si->si_int == SIG_BARRIER) {
			barrier_spin = 0;
		}
	}
}

void __sigusr2_handler(int signo)
{
	pull_spin = 0;
}


void __arrived_syncpoint1(void)
{
	__currenttask()->syncpoint1 = 1;
}
int __has_arrived_syncpoint1(sthread_t *task)
{
	return task->syncpoint1;	
}
void __arrived_syncpoint2(void)
{
	__currenttask()->syncpoint2 = 1;
}
int __all_have_arrived_syncpoint2()
{
	sthread_t *t = __threadlist;
	while(t->next)
	{
		if(t->syncpoint2 == 0)return 0;
	}
	return 1;
}
void __init_syncpoints(void)
{
	__currenttask()->syncpoint1 = 0;
	__currenttask()->syncpoint2 = 0;
}

void sthread_sync(int type, void *item)
{
	int ret;
	union sigval value;
	sthread_t *nexttask;

	
	__init_syncpoints();

	/* if it is the main thread, go on committing */
	if(__selftid == 0) commit_spin = 0;
	else commit_spin = 1;
	while(commit_spin) {
		pause();
	}

	//__debug_print("tid %d , sync1\n", __selftid);	
	__arrived_syncpoint1();
	
	
	__mvspace_commit();


/* some cases here */
	if(type == SIG_MUTEX_UNLOCK && item) {
		sthread_mutex_t *mutex = (sthread_mutex_t *)item;
		mutex->mutex->locked = 0;
	}



	/*notify the next task to commit */
	value.sival_int = SIG_COMMIT;
	/* chances are that a thread signals next, but the next thread hasn't reach pause, how to fix it ?? */

	nexttask = __nexttask(__currenttask());
	//__debug_print("next task %d\n", nexttask);
	if(nexttask) {
		while(!__has_arrived_syncpoint1(__nexttask(__currenttask()))) {

			//__debug_print("next pid %d \n", __nexttask(__currenttask())->pid);
			ret = sigqueue(__nexttask(__currenttask())->pid, SIGUSR1, value);
			if(ret) 
				perror("sigqueue");

		}
	//__debug_print("tid %d , sync2   next tid %d\n", __selftid, nexttask->tid);	
	}

	if(type == SIG_MUTEX_LOCK && item) {
		sthread_mutex_t *mutex = (sthread_mutex_t *)item;
		while(!(__sync_val_compare_and_swap(&(mutex->mutex->locked), 0, 1) == 0)) {
			/* this pause will answer to the sigusr1 sent by previous mutex_unlock, the sighanlder doesn't matter */
			pause();
		}
	}

	if(type == SIG_BARRIER && item) {
		sthread_barrier_t *barrier = (sthread_barrier_t *)item;
		barrier_spin = 1;
		/* when the count is reach, notify all threads with SIG_BARRIER, so that it'll only work in the barrier spin loop */
		if(__sync_add_and_fetch(&(barrier->barrier->count), 1) == barrier->totalcount) {
			value.sival_int = SIG_BARRIER;
			/* chances are that a thread signals next, but the next thread hasn't reach pause, how to fix it ?? */
			ret = sigqueue(__nexttask(__currenttask())->pid, SIGUSR1, value);
			if(ret) 
				perror("sigqueue");
		} else {

			while(barrier_spin) {
				pause();
			}
		}
		
	}



		//__debug_print("sync2.5   current == last %d\n", __currenttask() == (*__lasttask));	
	if(__currenttask() == (*__lasttask)) {

		//__debug_print("sync3   current == last %d\n", __currenttask() == (*__lasttask));	
		pull_spin = 0;
		/* notify all process in the group to pull */
		value.sival_int = SIG_PULL;
		/* same problem, even probably, because all threads in the group are notified, how to fix it ?? */
		while(!__all_have_arrived_syncpoint2()) {
			/* sigqueue cannot send to a process group.... */
			ret = kill(0, SIGUSR2);
			if(ret) 
				perror("kill");
		}
	}
	else pull_spin = 1;
	//__debug_print("sync4   current == last %d\n", __currenttask() == (*__lasttask));	

	while(pull_spin) {
		pause();
	}

	__arrived_syncpoint2();
	//__debug_print("tid %d , sync3\n", __selftid);	

	__mvspace_pull();
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
