#ifndef MUTEX_H
#define MUTEX_H

#include "task.h"

struct mutex_struct
{
	char locked;
	sthread_t *creator;
};

typedef struct sthread_mutex_t
{
	struct mutex_struct *mutex;
} sthread_mutex_t;

typedef struct sthread_mutexattr_t
{} sthread_mutexattr_t;

int sthread_mutex_init(sthread_mutex_t *mutex, const sthread_mutexattr_t *attr);

int sthread_mutex_destroy(sthread_mutex_t *mutex);


int sthread_mutex_trylock(sthread_mutex_t *mutex);

int sthread_mutex_lock(sthread_mutex_t *mutex);

int sthread_mutex_unlock(sthread_mutex_t *mutex);

#endif
