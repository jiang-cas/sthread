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
#define _GNU_SOURCE
#include <linux/sched.h>
#include <sys/syscall.h>

unsigned int __alloc_tid()
{
	return __localtid++;
}

void *__alloc_stack(unsigned int size)
{
	return malloc(size);
}
void add_registered_count(struct counter_struct *cs)
{
	(*(cs->val))++;
}

void __init_localtid(void)
{
	__localtid = 1;
	__selftid = 0;
}

/* allocate space for thread structs */
void __init_threadpool(void)
{
	__threadpool = (sthread_t *)mmap(NULL, MAXTHREADS * sizeof(sthread_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	__vamap = (struct va_struct *)mmap(NULL, MAPSIZE * sizeof(struct va_struct), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

/* add a new task to the task list, the state is E_NORMAL at first */
void __addtask(sthread_t *task)
{
	task->state = E_NORMAL;
	add_registered_count(&__registered);
}

sthread_t *__currenttask(void)
{
	return &__threadpool[__selftid];
}


/* before doing the really work, the thread need to do some setup stuff */
void __setup_newtask(void)
{
	__selftid = __localtid;
	__init_heap(__localtid);
	setpgid(0, __threadpool[0].pid);
	__currenttask()->tid = __selftid;
	__currenttask()->pid = getpid();
	__currenttask()->retval = NULL;
	__addtask(__currenttask());
	__mvspace_clone();
}

void *__start_routine(void *arw)
{
	void *(*func)(void *) = ((struct __argwrapper *)arw)->func;
	void *args = ((struct __argwrapper *)arw)->args;
	
	__setup_newtask();

	return func(args);
}

/* at beginnig, the main thread's state is E_NORMAL and others are E_NONE */
void __init_threadlist(void)
{
	int i;
	for(i=0;i<MAXTHREADS;i++) {
		__threadpool[i].state = E_NONE;
		__threadpool[i].leaved = 1;
		__threadpool[i].lock1 = new_sem(1);
	//	__threadpool[i].lock2 = new_sem(1);
		__threadpool[i].joinlock = new_sem(1);
		init_sem(__threadpool[i].joinlock, 0, 0);
		__threadpool[i].mutex = 0;
	}
	__threadpool[0].state = E_NORMAL;
	__threadpool[0].tid = 0;
}

/* allocate the counter of threads_number and initialize it into 0 */
void __init_shared_globals(void)
{
	__registered.val = (int *)global_malloc(sizeof(int));
	*(__registered.val) = 0;

	__initsync.val = (int *)global_malloc(sizeof(int));
	*(__initsync.val) = 0;

	__synced.val = (int *)global_malloc(sizeof(int));
	*(__synced.val) = 0;

	__semkey.val = (int *)global_malloc(sizeof(int));
	*(__semkey.val) = SEM_KEY_START;

	__global_barrier1.val = (int *)global_malloc(sizeof(int));
	*(__global_barrier1.val) = new_sem(1);

}

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
	__init_shared_globals();
	__init_threadlist();
	
	__mvspace_setflag();
}

/*__attribute__((destructor)) void cleanup()
{
	int i;
	for(i=0;i<MAXTHREADS;i++) {
		del_sem(__threadpool[i].lock1, 0);
		del_sem(__threadpool[i].lock2, 0);
		del_sem(__threadpool[i].joinlock, 0);
	}
	del_sem(*(__global_barrier1.val), 0);
}*/


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
	setup_sync();
	__DEBUG_PRINT(("tid %d exit0 \n", __selftid));
	wait_to_enter();	

	__mvspace_commit();
	__threadpool[__selftid].retval = value;
	__threadpool[__selftid].state = E_NONE;
	__DEBUG_PRINT(("tid %d exit2 \n", __selftid));

	v_next_and_wait();
	__DEBUG_PRINT(("tid %d exit3 \n", __selftid));
	post_sem(__threadpool[__selftid].joinlock, 0);
	sleep(5);
	exit(0);
}

int sthread_join(sthread_t thread, void **thread_return)
{
	setup_sync();
	__DEBUG_PRINT(("tid %d join0 \n", __selftid));
	
	wait_to_enter();
		
	__mvspace_commit();
	__threadpool[__selftid].state = E_STOPPED;
	
	__DEBUG_PRINT(("tid %d join1 \n", __selftid));

	v_next_and_wait();
	__DEBUG_PRINT(("tid %d join2 \n", __selftid));

	wait_sem(__threadpool[thread.tid].joinlock, 0);
	__DEBUG_PRINT(("tid %d join3 \n", __selftid));

	__mvspace_pull();
	__threadpool[__selftid].state = E_NORMAL;

	if(thread_return)
		*thread_return = __threadpool[thread.tid].retval;

	leave_sync();
	return 0;
}

/* main thread wait for all threads have been created and setup the wait queue */
void sthread_main_wait(int n)
{
	while(*(__registered.val) != n);
	*(__synced.val) = 1;
}

void sthread_main_exit(int n)
{
	int i;
	for(i=0;i<MAXTHREADS;i++) {
		del_sem(__threadpool[i].lock1, 0);	
		del_sem(__threadpool[i].joinlock, 0);	
	}		
	del_sem(*(__global_barrier1.val), 0);
	exit(n);

}
