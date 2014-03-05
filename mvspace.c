#include "include/mvspace.h"
#include <sys/syscall.h>
#include <unistd.h>
#include "include/globals.h"
#include <string.h>
#include "include/semaphore.h"
#include "include/sync.h"


int __in_sharedlist(void *addr)
{
	int i;
	for(i=0;i<MAPSIZE;i++) {
		if((__sharedlist->__vamap)[i].addr == addr)
			return 1;
	}
	return 0;
}

void sthread_shared(void *addr, size_t size)
{

		__setup_sync();
		wait_to_enter();	
		//		if(__selftid == 1) {
	//	__acquire_lock(__sharedlist->lock);
		if(!__in_sharedlist(addr)) {
				void *p = global_malloc(size);
				void *p2 = global_malloc(size);
				int __vamsize = __sharedlist->__vamsize;
				(__sharedlist->__vamap)[__vamsize].addr = addr;
				(__sharedlist->__vamap)[__vamsize].shared = p;
				(__sharedlist->__vamap)[__vamsize].backup = p2;
				(__sharedlist->__vamap)[__vamsize].size = size;
				memcpy(p2, addr, size);
				(__sharedlist->__vamsize)++;
		}
	//	__release_lock(__sharedlist->lock);
		v_next_and_wait();
		leave_sync();
		//		}
}

/* commit global data and bss */
int __mvspace_commit(void)
{	
//return syscall(316, __privatebase, __privatebase + MAXTHREADS*BLOCKSIZE);
	int i;
	for(i=0;i<MAPSIZE;i++) {
		if((__sharedlist->__vamap)[i].addr  && (__sharedlist->__vamap)[i].shared && (__sharedlist->__vamap)[i].backup) {
			int j;
			for(j=0;j<(__sharedlist->__vamap)[i].size;j++) {
				char a = ((char *)((__sharedlist->__vamap)[i].addr))[j];
				char b = ((char *)((__sharedlist->__vamap)[i].backup))[j];
				if(a != b)
					((char *)((__sharedlist->__vamap)[i].shared))[j] = a;
			}

		}
	}
	return 0;
}

int __mvspace_setflag(void)
{
//	return syscall(320);
	return 0;
}

int __mvspace_pull(void)
{
//return syscall(318, __privatebase, __privatebase + MAXTHREADS*BLOCKSIZE);
	int i;
	for(i=0;i<MAPSIZE;i++) {
	//	printf("*****\n");
		if((__sharedlist->__vamap)[i].addr  && (__sharedlist->__vamap)[i].shared && (__sharedlist->__vamap)[i].backup) {
//				printf("--------\n");
			memcpy((__sharedlist->__vamap)[i].addr, (__sharedlist->__vamap)[i].shared, (__sharedlist->__vamap)[i].size);
			memcpy((__sharedlist->__vamap)[i].backup, (__sharedlist->__vamap)[i].shared, (__sharedlist->__vamap)[i].size);
			
		}
	}
	return 0;
}

int __mvspace_clone(void)
{
	//return syscall(315);
	return 0;
}
