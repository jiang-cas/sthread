#ifndef SETTINGS_H
#define SETTINGS_H

#ifdef __STHREAD_DEBUG
#define __DEBUG_PRINT(x) printf x
#else
#define __DEBUG_PRINT(x) do {} while(0);
#endif

#define STACKSIZE 4096
#define MAXTHREADS 32
#define BLOCKSIZE 1024*1024

typedef unsigned char bool;
#define false 0
#define true 1

#define E_NORMAL 2
#define E_MUTEX 3
#define E_BARRIER 4
#define E_NONE 5
#define E_STOPPED 11
#define SIG_JOIN 6
#define SIG_EXIT 7
#define SIG_MUTEX_LOCK 8
#define SIG_MUTEX_UNLOCK 9
#define SIG_BARRIER 10

#define SEM_KEY_START 6666

#endif

