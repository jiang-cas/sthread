#include "include/heap.h"
#include "include/settings.h"
#include "include/globals.h"
#include <stdio.h>
#include <sys/mman.h>

void __init_global_heap() 
{
	__privatebase = mmap(NULL, BLOCKSIZE*MAXTHREADS, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	__sharedbase = mmap(NULL, BLOCKSIZE*MAXTHREADS, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
//	__globalbase = mmap(NULL, GLOBALSIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}


/* create mspace in the right place */
void __init_heap(unsigned int tid)
{
	/* the place is at an offset */
	void *privatebase = __privatebase + BLOCKSIZE * tid;
	void *sharedbase = __sharedbase + BLOCKSIZE * tid;
	SELF_HEAP.privatebase = privatebase;
	SELF_HEAP.privatemsp = create_mspace_with_base(privatebase, BLOCKSIZE, 0); 
	SELF_HEAP.sharedbase = sharedbase;
	SELF_HEAP.sharedmsp = create_mspace_with_base(sharedbase, BLOCKSIZE, 0); 
//	__globalmsp = create_mspace_with_base(__globalbase, BLOCKSIZE*MAXTHREADS, 0); 
}

void *mvprivate_malloc(size_t bytes)
{
	return mspace_malloc(SELF_HEAP.privatemsp, bytes);
}

void mvprivate_free(void *mem) 
{
	mspace_free(SELF_HEAP.privatemsp, mem);
}

void *mvshared_malloc(size_t bytes)
{
	return mspace_malloc(SELF_HEAP.sharedmsp, bytes);
}

void mvshared_free(void *mem) 
{
	mspace_free(SELF_HEAP.sharedmsp, mem);
}
/*
void *global_malloc(size_t bytes)
{
	return mspace_malloc(__globalmsp, bytes);
}

void global_free(void *mem)
{
	mspace_free(__globalmsp, mem);
}
*/
