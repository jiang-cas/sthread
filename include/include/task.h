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

/* alloc a new tid */
unsigned int __alloc_tid();

/* alloc a stack for the new thread */
void *__alloc_stack(unsigned int size);

void __init_localtid();

void __init_threadpool();

void *__start_routine(void *arw);



#endif
