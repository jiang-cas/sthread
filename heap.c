#include "include/heap.h"
#include "include/settings.h"
#include "include/globals.h"
#include <stdio.h>
#include <sys/mman.h>

void __init_global_heap() 
{
	__privatebase = mmap(NULL, BLOCKSIZE*MAXTHREADS, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

#define SELF_THREAD (__threadpool[__selftid])
#define SELF_HEAP (SELF_THREAD.heap_struct)

/* create mspace in the right place */
void __init_heap(unsigned int tid)
{
	/* the place is at an offset */
	void *privatebase = __privatebase + BLOCKSIZE * tid;
	SELF_HEAP.privatebase = privatebase;
	SELF_HEAP.privatemsp = create_mspace_with_base(privatebase, BLOCKSIZE, 0); 
}

void *mvmalloc(size_t bytes)
{
	return mspace_malloc(SELF_HEAP.privatemsp, bytes);
}

void mvfree(void *mem) 
{
	mspace_free(SELF_HEAP.privatemsp, mem);
}
