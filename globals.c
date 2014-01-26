#include "include/globals.h"

/* mvheap malloc base address*/
void *__privatebase;
void *__sharedbase;

/* __localtid == __selftid, except in main thread  __localtid == number of total threads*/
unsigned int __localtid;
unsigned int __selftid;
unsigned int *__registeredcount;

/* all info about created threads*/
sthread_t *__threadpool;

sthread_t *__threadlist;

sthread_t **__lasttask;

sthread_t *__maintask;
