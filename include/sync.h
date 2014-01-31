#ifndef SYNC_H
#define SYNC_H
#include <signal.h>
#include "mutex.h"
#include "barrier.h"



void sthread_sync_normal(int type);

int sync_mutex_unlock(sthread_mutex_t *mutex);
int sync_mutex_lock(sthread_mutex_t *mutex);
int sync_barrier_wait(sthread_barrier_t *barrier);


#endif
