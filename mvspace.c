#include "include/mvspace.h"
#include <sys/syscall.h>
#include <unistd.h>
#include "include/globals.h"
/* commit global data and bss */
int __mvspace_commit(void)
{	
//return syscall(316, __privatebase, __privatebase + MAXTHREADS*BLOCKSIZE);
	return 0;
}

int __mvspace_setflag(void)
{
	return syscall(320);
}

int __mvspace_pull(void)
{
//return syscall(318, __privatebase, __privatebase + MAXTHREADS*BLOCKSIZE);
	return 0;
}

int __mvspace_clone(void)
{
	return syscall(315);
}
