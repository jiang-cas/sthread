#include <unistd.h>
#include <signal.h>
#include "include/mvspace.h"
#include "include/globals.h"

int commit_spin;
int pull_spin;


void __sigusr1_handler(int signo)
{
	commit_spin = 0;	
}

void __sigusr2_handler(int signo)
{
	pull_spin = 0;	
}

void sync(void)
{
	int ret;
	/* if it is the main thread, go on committing */
	if(__selftid == 0) commit_spin = 0;
	else commit_spin = 1;
	while(commit_spin) {
		pause();
	}
	
	__mvspace_commit();
	/*notify the next task to commit */
	ret = kill(__nexttask(__currenttask())->pid, SIGUSR1);
	if(ret) 
		perror("kill");

	if(__currenttask() == __lasttask) {
		pull_spin = 0;
		/* notify all process in the group to pull */
		kill(0, SIGUSR2);
	}
	else pull_spin = 1;

	while(pull_spin) {
		pause();
	}

	__mvspace_pull();
}

int sync_mutex_lock(sthread_mutex_t *mutex)
{
	if(mutex->mutex) {
		int ret;
		if(__selftid == 0) commit_spin = 0;
		else commit_spin = 1;
		while(commit_spin) {
			pause();
		}
		/* it is a synchronization point, so here should be a commit */
		__mvspace_commit();
		/*notify the next task to commit */
		ret = kill(__nexttask(__currenttask())->pid, SIGUSR1);
		if(ret) 
			perror("kill");

		/* if the lock hasn't been release by a previous thread, wait here */
		while(!(__sync_val_compare_and_swap(&(mutex->mutex->locked), 0, 1) == 0)) {
			/* this pause will answer to the sigusr1 sent by previous mutex_unlock, the sighanlder doesn't matter */
			pause();
		}
		/*after successfully getting the lock, pull the data and go on */
		if(__currenttask() == __lasttask) {
			pull_spin = 0;
			/* notify all process in the group to pull */
			kill(0, SIGUSR2);
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
		if(__selftid == 0) commit_spin = 0;
		else commit_spin = 1;
		while(commit_spin) {
			pause();
		}
		/* it is a synchronization point, so here should be a commit */
		__mvspace_commit();

		/* release the lock and notify other, due to the sequential signal sending, the lock will be acquired in the same order */
		mutex->mutex->locked = 0;
		ret = kill(__nexttask(__currenttask())->pid, SIGUSR1);
		if(ret) 
			perror("kill");
		
		if(__currenttask() == __lasttask) {
			pull_spin = 0;
			/* notify all process in the group to pull */
			kill(0, SIGUSR2);
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
