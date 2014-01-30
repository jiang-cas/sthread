#include "include/globals.h"

/* mvheap malloc base address*/
void *__privatebase;
void *__sharedbase;

/* __localtid == __selftid, except in main thread  __localtid == number of total threads*/
unsigned int __localtid;
unsigned int __selftid;

/* all info about created threads*/
sthread_t *__threadpool;
struct counter_struct __registered;
struct wait_queue_struct __common_waits;
struct synchronized_struct __synced;
