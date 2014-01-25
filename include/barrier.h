#ifndef BARRIER_H
#define BARRIER_H

#include "task.h"


struct barrier_struct
{
	int count;
	sthread_t *creator;
};

typedef struct sthread_barrier_t
{
	struct barrier_struct barrier;
} stread_barrier_t;

typedef struct sthread_barrierattr_t
{} sthread_barrierattr_t;

int sthread_barrier_init(sthread_barrier_t *barrier, const sthread_barrierattr_t *attr, unsigned count);

int sthread_barrier_destroy(sthread_barrier_t *barrier);

int sthread_barrier_wait(sthread_barrier_t *barrier);

#endif
