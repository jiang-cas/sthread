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

#define E_NORMAL 0
#define E_MUTEX 1
#define E_BARRIER 2
#define E_NONE 3

typedef unsigned char bool;
#define true 1
#define false 0


#endif

