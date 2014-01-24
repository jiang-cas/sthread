#ifndef SYNC_H
#define SYNC_H


void __sigusr1_handler(int signo);
void __sigusr2_handler(int signo);

void sync(void);
int sync_mutex_unlock(sthread_mutex_t *mutex);
int sync_mutex_lock(sthread_mutex_t *mutex);


#endif
