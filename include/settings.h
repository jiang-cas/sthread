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


#endif

