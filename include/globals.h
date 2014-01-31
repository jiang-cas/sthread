#ifndef GLOABALS_H
#define GLOABALS_H

#include "types.h"
#include <semaphore.h>
#include <sys/types.h>


extern void *__privatebase;
extern void *__sharedbase;
extern unsigned int __localtid;
extern unsigned int __selftid;

extern sthread_t *__threadpool;
extern struct counter_struct __registered;
extern struct wait_queue_struct __common_waits;
extern struct synchronized_struct __synced;



#endif
