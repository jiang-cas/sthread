#include "include/globals.h"

/* mvheap malloc base address*/
void *__privatebase;
void *__sharedbase;
//void *__globalbase;
//mspace __globalmsp; 

/* __localtid == __selftid, except in main thread  __localtid == number of total threads*/
unsigned int __localtid;
unsigned int __selftid;

/* all info about created threads*/
sthread_t *__threadpool;
struct counter_struct __registered;
struct counter_struct __initsync;
struct counter_struct __synced;
struct counter_struct __global_barrier1;
struct counter_struct __semkey;

struct sharedlist_struct *__sharedlist;

