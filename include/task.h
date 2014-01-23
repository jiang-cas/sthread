#ifndef TASK_H
#define TASK_H

#include <stdio.h>
#include "heap.h"



typedef struct sthread_t
{
	unsigned int tid;
	void *retval;
	int pid;
	struct __mvheap heap_struct;
	struct sthread_t *next;
	struct sthread_t *pre;
} sthread_t;

typedef struct sthread_attr_t 
{
	unsigned int stack_size;
} sthread_attr_t;

struct __argwrapper 
{
	void *(*func)(void *);
	void *args;
};


unsigned int __alloc_tid();
void *__alloc_stack(unsigned int size);
void __init_localtid(void);
void __init_threadpool(void);
void *__start_routine(void *arw);
void __init_threadlist(void);
void __addtask(sthread_t *task);
sthread_t *__nexttask(sthread_t *task);
sthread_t *__pretask(sthread_t *task);
sthread_t *__currenttask(void);


#endif
