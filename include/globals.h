#ifndef GLOABALS_H
#define GLOABALS_H

#include "types.h"


extern void *__privatebase;
extern void *__sharedbase;
extern unsigned int __localtid;
extern unsigned int __selftid;

extern sthread_t *__threadpool;
extern unsigned int *__registeredcount;



#endif
