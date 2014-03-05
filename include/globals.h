#ifndef GLOABALS_H
#define GLOABALS_H

#include "types.h"
#include <semaphore.h>
#include <sys/types.h>


extern void *__privatebase;
extern void *__sharedbase;
extern void *__globalbase;
extern mspace __globalmsp;
extern unsigned int __localtid;
extern unsigned int __selftid;

extern sthread_t *__threadpool;
extern struct counter_struct __registered;
extern struct counter_struct __initsync;
extern struct counter_struct __synced;
extern struct counter_struct __global_barrier1;
extern struct counter_struct __semkey;

extern struct sharedlist_struct *__sharedlist;

#endif
