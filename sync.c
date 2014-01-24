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
