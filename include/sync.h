#ifndef SYNC_H
#define SYNC_H
#include <signal.h>
#include "mutex.h"
#include "barrier.h"

#define SIG_COMMIT 0
#define SIG_PULL 1
#define SIG_BARRIER 2
#define SIG_MUTEX_UNLOCK 3
#define SIG_MUTEX_LOCK 4
#define SIG_NORMAL 5

void __sigusr1_handler(int signo, siginfo_t *si, void *ucontext);
void __sigusr2_handler(int signo);

void sthread_sync(int type, void *item);
int sync_mutex_unlock(sthread_mutex_t *mutex);
int sync_mutex_lock(sthread_mutex_t *mutex);
int sync_barrier_wait(sthread_barrier_t *barrier);


#endif
