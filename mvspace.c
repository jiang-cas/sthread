#include "include/mvspace.h"

/* commit global data and bss */
int __mvspace_commit(void)
{
//	return syscall(320);
	return 0;
}

int __mvspace_setflag(void)
{
	return 0;
}

int __mvspace_pull(void)
{
	return 0;
}

