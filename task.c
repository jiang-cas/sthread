#include "include/task.h"
#include "include/globals.h"
#include <stdlib.h>
#include "include/settings.h"
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <string.h>
#include <errno.h>

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
}

void __init_threadpool(void)
{
	__threadpool = (sthread_t *)mmap(NULL, MAXTHREADS * sizeof(sthread_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

void *__start_routine(void *arw)
{
	void *(*func)(void *) = ((struct __argwrapper *)arw)->func;
	void *args = ((struct __argwrapper *)arw)->args;
	__selftid = __localtid;
	__init_heap(__localtid);
	return func(args);
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
	__jp_set_mvspace_flag();
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
			__error_log("clone failed %s\n", strerror(errno));
			return -2;
		} else {
			newthread->tid = __alloc_tid();
			newthread->pid = ret;
			newthread->retval = NULL;
			__threadpool[newthread->tid] = *newthread;
			return ret;
		}
	} else {
		__error_log("alloc stack failed %s\n", strerror(errno));
		return -1;
	}
}

sthread_t sthread_self(void)
{
	sthread_t self;
	self = __threadpool[__selftid];
	return self;	
}

void sthread_exit(void *value)
{
	__threadpool[__localtid].retval = value;
	exit(0);
}

int sthread_join(sthread_t thread, void **thread_return)
{
	waitpid(thread.pid, NULL, 0);
	if(thread_return)
		*thread_return = __threadpool[thread.tid].retval;
	return 0;
}
