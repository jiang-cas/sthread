#include <unistd.h>
#include <sys/types.h>
#include "include/mvspace.h"
#include "include/globals.h"
#include "include/sync.h"
#include "include/settings.h"


#define SIG_COMMIT 0
#define SIG_PULL 1
#define SIG_BARRIER 2

int commit_spin;
int pull_spin;
int barrier_spin;


void __sigusr1_handler(int signo, siginfo_t *si, void *ucontext)
{
	if(si->si_code == SI_QUEUE) {
		if(si->si_int == SIG_COMMIT) {
			commit_spin = 0;
		} else if(si->si_int == SIG_PULL) {
			pull_spin = 0;
		} else if(si->si_int == SIG_BARRIER) {
			barrier_spin = 0;
		}
	}
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

void sync(void)
{
	int ret;
	union sigval value;

	
	__init_syncpoints();
	__debug_print("tid %d , sync0\n", __selftid);	

	/* if it is the main thread, go on committing */
	if(__selftid == 0) commit_spin = 0;
	else commit_spin = 1;
	while(commit_spin) {
		pause();
	}

	__debug_print("tid %d , sync1\n", __selftid);	
	__arrived_syncpoint1();
	
	
	__mvspace_commit();
	/*notify the next task to commit */
	value.sival_int = SIG_COMMIT;
	/* chances are that a thread signals next, but the next thread hasn't reach pause, how to fix it ?? */

	while(!__has_arrived_syncpoint1(__nexttask(__currenttask()))) {

	__debug_print("tid %d , sync1.5   next tid %d\n", __selftid, __nexttask(__currenttask())->tid);	
		ret = sigqueue(__nexttask(__currenttask())->pid, SIGUSR1, value);
		if(ret) 
			perror("sigqueue");

	}
	__debug_print("tid %d , sync2   next tid %d\n", __selftid, __nexttask(__currenttask())->tid);	

	if(__currenttask() == __lasttask) {
		pull_spin = 0;
		/* notify all process in the group to pull */
		value.sival_int = SIG_PULL;
		/* same problem, even probably, because all threads in the group are notified, how to fix it ?? */
		while(!__all_have_arrived_syncpoint2()) {
			ret = sigqueue(0, SIGUSR1, value);
			if(ret) 
				perror("sigqueue");
		}
	}
	else pull_spin = 1;

	while(pull_spin) {
		pause();
	}

	__arrived_syncpoint2();
	__debug_print("tid %d , sync3\n", __selftid);	

	__mvspace_pull();
}

int sync_mutex_lock(sthread_mutex_t *mutex)
{
	if(mutex->mutex) {
		int ret;
		union sigval value;

		if(__selftid == 0) commit_spin = 0;
		else commit_spin = 1;
		while(commit_spin) {
			pause();
		}
		/* it is a synchronization point, so here should be a commit */
		__mvspace_commit();
		/*notify the next task to commit */
		value.sival_int = SIG_COMMIT;
		/* chances are that a thread signals next, but the next thread hasn't reach pause, how to fix it ?? */
		ret = sigqueue(__nexttask(__currenttask())->pid, SIGUSR1, value);
		if(ret) 
			perror("sigqueue");

		/* if the lock hasn't been release by a previous thread, wait here */
		
		while(!(__sync_val_compare_and_swap(&(mutex->mutex->locked), 0, 1) == 0)) {
			/* this pause will answer to the sigusr1 sent by previous mutex_unlock, the sighanlder doesn't matter */
			pause();
		}
		/*after successfully getting the lock, pull the data and go on */
		if(__currenttask() == __lasttask) {
			pull_spin = 0;
			/* notify all process in the group to pull */
			value.sival_int = SIG_PULL;
			/* same problem, even probably, because all threads in the group are notified, how to fix it ?? */
			ret = sigqueue(0, SIGUSR1, value);
			if(ret) 
				perror("sigqueue");
		}
		else pull_spin = 1;

		while(pull_spin) {
			pause();
		}

		__mvspace_pull();

		return 0;

	}
	return -1;
}

int sync_mutex_unlock(sthread_mutex_t *mutex)
{
	if(mutex->mutex) {
		int ret;
		union sigval value;
	
		if(__selftid == 0) commit_spin = 0;
		else commit_spin = 1;
		while(commit_spin) {
			pause();
		}
		/* it is a synchronization point, so here should be a commit */
		__mvspace_commit();

		/* release the lock and notify other, due to the sequential signal sending, the lock will be acquired in the same order */
		mutex->mutex->locked = 0;
		value.sival_int = SIG_COMMIT;
		/* chances are that a thread signals next, but the next thread hasn't reach pause, how to fix it ?? */
		ret = sigqueue(__nexttask(__currenttask())->pid, SIGUSR1, value);
		if(ret) 
			perror("sigqueue");
		
		if(__currenttask() == __lasttask) {
			pull_spin = 0;
			/* notify all process in the group to pull */
			value.sival_int = SIG_PULL;
			/* same problem, even probably, because all threads in the group are notified, how to fix it ?? */
			ret = sigqueue(0, SIGUSR1, value);
			if(ret) 
				perror("sigqueue");
		}
		else pull_spin = 1;

		while(pull_spin) {
			pause();
		}
		__mvspace_pull();

		return 0;

	}

	return -1;	
}


int sync_barrier_wait(sthread_barrier_t *barrier)
{
	if(barrier->barrier) {
		int ret;
		union sigval value;

		barrier_spin = 1;

		if(__selftid == 0) commit_spin = 0;
		else commit_spin = 1;
		while(commit_spin) {
			pause();
		}
		/* it is a synchronization point, so here should be a commit */
		__mvspace_commit();

		/*notify the next task to commit */
		value.sival_int = SIG_COMMIT;
		/* chances are that a thread signals next, but the next thread hasn't reach pause, how to fix it ?? */
		ret = sigqueue(__nexttask(__currenttask())->pid, SIGUSR1, value);
		if(ret) 
			perror("sigqueue");
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
		
	/*pull stage */	
		if(__currenttask() == __lasttask) {
			pull_spin = 0;
			/* notify all process in the group to pull */
			value.sival_int = SIG_PULL;
			/* same problem, even probably, because all threads in the group are notified, how to fix it ?? */
			ret = sigqueue(0, SIGUSR1, value);
			if(ret) 
				perror("sigqueue");
		}
		else pull_spin = 1;

		while(pull_spin) {
			pause();
		}
		__mvspace_pull();

		return 0;
		
	}

	return -1;
}
