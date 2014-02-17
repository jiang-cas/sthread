#ifndef COND_H
#define COND_H

#include "mutex.h"
#include <time.h>

struct cond_struct {
	int locks;
};

typedef struct sthread_cond_t
{
	struct cond_struct *cond;
} sthread_cond_t;

typedef struct sthread_condattr_t
{
} sthread_condattr_t;

int sthread_cond_init(sthread_cond_t *cond, sthread_condattr_t *cond_attr);

int sthread_cond_signal(sthread_cond_t *cond);

int sthread_cond_broadcast(sthread_cond_t *cond);

int sthread_cond_wait(sthread_cond_t *cond, sthread_mutex_t *mutex);

int sthread_cond_timewait(sthread_cond_t *cond, sthread_mutex_t *mutex, const struct timespec *abstime);

int sthread_cond_destroy(sthread_cond_t *cond);


#endif
