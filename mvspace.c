#include "include/mvspace.h"
#include <sys/syscall.h>
#include <unistd.h>
#include "include/globals.h"
#include <string.h>


void sthread_shared(void *addr, size_t size)
{
	void *p = global_malloc(size);
	void *p2 = global_malloc(size);
	__vamap[__vamsize].addr = addr;
	__vamap[__vamsize].shared = p;
	__vamap[__vamsize].backup = p2;
	__vamap[__vamsize].size = size;
	memcpy(p2, addr, size);
	__vamsize++;
}

/* commit global data and bss */
int __mvspace_commit(void)
{	
//return syscall(316, __privatebase, __privatebase + MAXTHREADS*BLOCKSIZE);
	int i;
	for(i=0;i<MAPSIZE;i++) {
//		printf("%ld %ld\n", __vamap[i].addr, __vamap[i].paddr);
		if(__vamap[i].addr  && __vamap[i].shared && __vamap[i].backup) {
			int j;
			for(j=0;j<__vamap[i].size;j++) {
				char a = ((char *)(__vamap[i].addr))[j];
				char b = ((char *)(__vamap[i].backup))[j];
				if(a != b)
					((char *)(__vamap[i].shared))[j] = a;
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
		if(__vamap[i].addr  && __vamap[i].shared && __vamap[i].backup) {
//				printf("--------\n");
			memcpy(__vamap[i].addr, __vamap[i].shared, __vamap[i].size);
			memcpy(__vamap[i].backup, __vamap[i].shared, __vamap[i].size);
			
		}
	}
	return 0;
}

int __mvspace_clone(void)
{
	//return syscall(315);
	return 0;
}
