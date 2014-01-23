#ifndef SYNC_H
#define SYNC_H


void __sigusr1_handler(int signo);
void __sigusr2_handler(int signo);

void sync(void);


#endif
