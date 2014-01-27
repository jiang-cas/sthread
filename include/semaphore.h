#ifndef SEM_H
#define SEM_H

#include <semaphore.h>
#include <sys/types.h>


sem_t *new_sem(void);

int init_sem(sem_t *sema, int init_value);

int del_sem(sem_t *sema);

int p_sem(sem_t *sema);

int v_sem(sem_t *sema);

int read_sem(sem_t *sema);


#endif
