#ifndef HEAP_H
#define HEAP_H

#include "malloc.h"

struct heap_struct
{
	void *privatebase;
	mspace privatemsp;
	void *sharedbase;
	mspace sharedmsp;
};

void __init_global_heap();

void __init_heap(unsigned int tid);

void *mvprivate_malloc(size_t bytes);

void mvprivate_free(void *mem);

void *mvshared_malloc(size_t bytes);

void mvshared_free(void *mem);
/*
void *global_malloc(size_t bytes);

void global_free(void *mem);
*/
#endif
