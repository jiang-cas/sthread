#include "include/task.h"
#include "include/globals.h"
#include "include/settings.h"
#include "include/sync.h"
#include "include/mvspace.h"
#include "include/semaphore.h"
#include "include/equeue.h"
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
void add_registered_count(struct counter_struct *cs)
{
	(*(cs->counts))++;
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
}

/* add a new task to the task list, the state is E_NORMAL at first */
void __addtask(sthread_t *task)
{
	task->state = E_NORMAL;
	add_registered_count(&__registered);
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
	__threadpool[0].state = E_NORMAL;
	__threadpool[0].tid = 0;
	for(i=1;i<MAXTHREADS;i++) {
		__threadpool[i].state = E_NONE;
	}
}

/* functions for iteration on the task list */
sthread_t *__nexttask(sthread_t *task)
{
	int i;
	for(i=task->tid + 1;i<MAXTHREADS;i++) {
		if(__threadpool[i].state != E_NONE)
			return &__threadpool[i];	
	}
	return NULL;
}

sthread_t *__pretask(sthread_t *task)
{
	int i;
	for(i=task->tid - 1; i>=0; i--) {
		if(__threadpool[i].state != E_NONE)
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
		if(__threadpool[i].state != E_NONE) return &__threadpool[i];
	}
	return &__threadpool[0];
}

/* constructor & destructor of thread_counts and wait queue */
void new_counter_struct(struct counter_struct *cs)
{
	cs->counts = mvshared_malloc(sizeof(int));
}
void free_counter_struct(struct counter_struct *cs)
{
	mvshared_free(cs->counts);
}
void new_wait_queue_struct(struct wait_queue_struct *wqs)
{
	int i;
	struct wait_queue *waits;
	wqs->wq = (struct wait_queue *)mvshared_malloc(sizeof(struct wait_queue));
	waits = wqs->wq;
	for(i=0;i<MAXTHREADS;i++) {
		waits->semaqueue[i] = new_sem();
	}
	waits->barrier = new_sem();
	waits->p_barrier = new_sem();
	waits->inited = 0;
}
void free_wait_queue_struct(struct wait_queue_struct *wqs)
{
	int i;
	struct wait_queue *waits = wqs->wq;
	for(i=0;i<MAXTHREADS;i++) {
		free_sem(waits->semaqueue[i]);
	}
	free_sem(waits->barrier);
	free_sem(waits->p_barrier);
	mvshared_free(wqs->wq);
}

/* allocate the counter of threads_number and initialize it into 0 */
void __init_shared_globals(void)
{
	new_counter_struct(&__registered);
	new_wait_queue_struct(&__common_waits);
	*(__registered.counts) = 0;
	__synced.synced = mvshared_malloc(sizeof(int));
	*(__synced.synced) = 0;
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
	sthread_sync_normal(SIG_EXIT);
	__threadpool[__localtid].retval = value;
	exit(0);
}

int sthread_join(sthread_t thread, void **thread_return)
{
	int ret;
	sthread_sync_normal(SIG_JOIN);
	
	thread = __threadpool[thread.tid];
	__DEBUG_PRINT(("tid %d wait to join thread %d \n", __selftid, thread.tid));	
	ret = waitpid(thread.pid, NULL, 0);
	__DEBUG_PRINT(("tid %d has joined thread %d ret %d\n", __selftid, thread.tid, ret));	

	__threadpool[__selftid].state = E_NORMAL;
	if(thread_return)
		*thread_return = __threadpool[thread.tid].retval;
	return 0;
}

/* main thread wait for all threads have been created and setup the wait queue */
void sthread_main_wait(int n)
{
	while(*(__registered.counts) != n);
	init_common_wait_queue(__common_waits.wq, E_NORMAL);
	*(__synced.synced) = 1;
}
