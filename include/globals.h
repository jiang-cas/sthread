#ifndef GLOABALS_H
#define GLOABALS_H

#include "types.h"


extern void *__privatebase;
extern void *__sharedbase;
extern unsigned int __localtid;
extern unsigned int __selftid;

extern sthread_t *__threadpool;
extern struct counter_struct __registered;
extern struct wait_queue __common_waits;



#endif
