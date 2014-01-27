#include "include/task.h"
#include "include/globals.h"
#include "include/settings.h"
#include "include/sync.h"
#include "include/mvspace.h"
#include "include/semaphore.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sched.h>

unsigned int __alloc_tid()
{
	return __localtid++;
}

void *__alloc_stack(unsigned int size)
{
	return malloc(size);
}

void __init_localtid(void)
{
	__localtid = 1;
	__selftid = 0;
}

void __init_threadpool(void)
{
	__threadpool = (sthread_t *)mmap(NULL, MAXTHREADS * sizeof(sthread_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

void __addtask(sthread_t *task)
{
	task->inlist = 1;
	__sync_add_and_fetch(__registeredcount, 1);
}

void __setup_newtask(void)
{
	//__debug_print("enter setup newtask\n");
	__selftid = __localtid;
	//__debug_print("selftid %d \n", __selftid);
	__init_heap(__localtid);
	setpgid(0, __threadpool[0].pid);
	__currenttask()->tid = __selftid;
	__currenttask()->pid = getpid();
	__currenttask()->retval = NULL;
	__addtask(__currenttask());
}

void *__start_routine(void *arw)
{
	void *(*func)(void *) = ((struct __argwrapper *)arw)->func;
	void *args = ((struct __argwrapper *)arw)->args;
	
	__setup_newtask();

	return func(args);
}

void __init_threadlist(void)
{
	int i;
	__threadpool[0].inlist = 1;
	__threadpool[0].tid = 0;
	for(i=1;i<MAXTHREADS;i++) {
		__threadpool[i].inlist = 0;
	}	
}


sthread_t *__nexttask(sthread_t *task)
{
	int i;
	for(i=task->tid + 1;i<MAXTHREADS;i++) {
		if(__threadpool[i].inlist)
			return &__threadpool[i];	
	}
	return NULL;
}

sthread_t *__pretask(sthread_t *task)
{
	int i;
	for(i=task->tid - 1; i>=0; i--) {
		if(__threadpool[i].inlist)
			return &__threadpool[i];	
	}
	return NULL;
}

sthread_t *__currenttask(void)
{
	return &__threadpool[__selftid];
}

sthread_t *__lasttask(void)
{
	int i;
	for(i=MAXTHREADS;i>=0;i--) {
		if(__threadpool[i].inlist) return &__threadpool[i];
	}
	return &__threadpool[0];
}


void __init_sync(void)
{
	int i;
	setpgid(0, 0);
	for(i=0;i<MAXTHREADS;i++) {
		__threadpool[i].sync.commit_sem = new_sem();
		__threadpool[i].sync.pull_sem = new_sem();
	}
}

void __init_shared_variable(void)
{
	__registeredcount = (unsigned int *)mvshared_malloc(sizeof(unsigned int));
//	__registeredcount = (unsigned int *)mmap(NULL, sizeof(unsigned int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	*__registeredcount = 0;
}

/* initial sthreads */
__attribute__((constructor)) void init()
{
	/* prepared for next thread, and now __selftid is 0 */
	__init_localtid();
	/* allocate threadpool for threads info */
	__init_threadpool();
	/* allocate mvheap malloc space */
	__init_global_heap();
	/* init mvheap for main thread */
	__init_heap(0);
	/* init global data and bss for main thread */
	__mvspace_setflag();
	/* init register thread totalcount */	
	__init_shared_variable();
	__init_threadlist();
	/* setup signal handlers */
	__init_sync();
}


int sthread_create(sthread_t *newthread, sthread_attr_t *attr, void *(*func)(void *), void *args)
{
	unsigned int stack_size;
	void *pstack;
	
	if(attr)
		stack_size = attr->stack_size;
	else 
		stack_size = STACKSIZE;

	pstack = __alloc_stack(stack_size);

	if(NULL != pstack) {
		int ret;
		struct __argwrapper arw;
		arw.func = func;
		arw.args = args;
		ret = clone(__start_routine, (void *)(unsigned char *)pstack + stack_size, SIGCHLD | 0, &arw);
		if(-1 == ret) {
			perror("clone");
			return -2;
		} else {
			/* only returns the tid, and the rest are setup by the child process */
			newthread->tid = __alloc_tid();
			return ret;
		}
	}
	perror("stack alloc");
	return -1;
}

sthread_t sthread_self(void)
{
	sthread_t self;
	self = __threadpool[__selftid];
	return self;	
}

void sthread_exit(void *value)
{
	sthread_sync(SIG_NORMAL, NULL);
	__threadpool[__localtid].retval = value;
	exit(0);
}

int sthread_join(sthread_t thread, void **thread_return)
{
	sthread_sync(SIG_NORMAL, NULL);
	waitpid(thread.pid, NULL, 0);
	if(thread_return)
		*thread_return = __threadpool[thread.tid].retval;
	return 0;
}

void sthread_register(unsigned int n)
{
	while(*__registeredcount != n);
}
