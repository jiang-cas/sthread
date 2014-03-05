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

void __mem_copy(void *dst, void *src, size_t size)
{
	int i;
	char *dest = (char *)dst;
	char *source = (char *)src;
	for(i=0;i<size;i++)
		dest[i] = source[i];
}

void sthread_shared(void *addr, size_t size)
{

		__setup_sync();
//		__DEBUG_PRINT(("tid %d shared 0 \n", __selftid));
//		__DEBUG_PRINT(("tid %d shared 0.1 barrier %d \n", __selftid, *(__global_barrier1.val)));
		wait_to_enter();	
//		__DEBUG_PRINT(("tid %d shared 0.2 barrier %d \n", __selftid, *(__global_barrier1.val)));
//		__DEBUG_PRINT(("tid %d shared 1 \n", __selftid));
		//		if(__selftid == 1) {
	//	__acquire_lock(__sharedlist->lock);
		if(!__in_sharedlist(addr)) {
		__DEBUG_PRINT(("tid %d shared 1.05 barrier %d \n", __selftid, *(__global_barrier1.val)));
				void *p = mvshared_malloc(size);
				void *p2 = mvshared_malloc(size);
				int __vamsize = __sharedlist->__vamsize;
				(__sharedlist->__vamap)[__vamsize].addr = addr;
//		__DEBUG_PRINT(("tid %d shared 1.1 barrier %d \n", __selftid, *(__global_barrier1.val)));
				(__sharedlist->__vamap)[__vamsize].addr = addr;
//		__DEBUG_PRINT(("tid %d shared 1.15 barrier %d \n", __selftid, *(__global_barrier1.val)));
				(__sharedlist->__vamap)[__vamsize].shared = p;
//		__DEBUG_PRINT(("tid %d shared 1.2 barrier %d \n", __selftid, *(__global_barrier1.val)));
				(__sharedlist->__vamap)[__vamsize].backup = p2;
//		__DEBUG_PRINT(("tid %d shared 1.3 barrier %d \n", __selftid, *(__global_barrier1.val)));
				(__sharedlist->__vamap)[__vamsize].size = size;
//		__DEBUG_PRINT(("tid %d shared 1.4 p2 %ld p %ld barrierpoint %ld \n", __selftid, p2, p, (__global_barrier1.val)));
//			memcpy(p2, addr, size);
//				memcpy(p, addr, size);
		__mem_copy(p2, addr, size);
		__mem_copy(p, addr, size);

//		__DEBUG_PRINT(("tid %d shared 1.5 barrier %d \n", __selftid, *(__global_barrier1.val)));
				(__sharedlist->__vamsize)++;
//		__DEBUG_PRINT(("tid %d shared 1.6 barrier %d \n", __selftid, *(__global_barrier1.val)));
		}
	//	__release_lock(__sharedlist->lock);
		v_next_and_wait();
//		__DEBUG_PRINT(("tid %d shared 2 \n", __selftid));
		leave_sync();
//		__DEBUG_PRINT(("tid %d shared 3 \n", __selftid));
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
//			printf("%ld, %ld, %ld \n", (__sharedlist->__vamap)[i].addr, (__sharedlist->__vamap)[i].shared, (__sharedlist->__vamap)[i].size);
			__mem_copy((__sharedlist->__vamap)[i].addr, (__sharedlist->__vamap)[i].shared, (__sharedlist->__vamap)[i].size);
//			printf("%ld, %ld, %ld \n", (__sharedlist->__vamap)[i].backup, (__sharedlist->__vamap)[i].shared, (__sharedlist->__vamap)[i].size);
			__mem_copy((__sharedlist->__vamap)[i].backup, (__sharedlist->__vamap)[i].shared, (__sharedlist->__vamap)[i].size);
			
		}
	}
	return 0;
}

int __mvspace_clone(void)
{
	//return syscall(315);
	return 0;
}
