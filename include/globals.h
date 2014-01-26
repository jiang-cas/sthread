#ifndef GLOABALS_H
#define GLOABALS_H

#include "types.h"


extern void *__privatebase;
extern void *__sharedbase;
extern unsigned int __localtid;
extern unsigned int __selftid;
extern unsigned int *__registeredcount;
extern void *__mutexbase;

extern sthread_t *__threadpool;
extern sthread_t *__threadlist;
extern sthread_t **__lasttask;
extern sthread_t *__maintask;

#endif
