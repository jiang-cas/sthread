#ifndef HEAP_H
#define HEAP_H


#include "malloc.h"


struct __mvheap
{
	void *privatebase;
	mspace privatemsp;
};


void __init_global_heap();

void __init_heap(unsigned int tid);

void *mvmalloc(size_t bytes);

void mvfree(void *mem);

#endif
