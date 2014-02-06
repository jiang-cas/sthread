#ifndef SYNC_H
#define SYNC_H

void setup_sync(void);

void setup_mutex_sync(struct mutex_struct *mutex);
void v_next_and_wait(void);
void v_next_mutex(struct mutex_struct *mutex);

void leave_sync(void);
void wait_to_enter(void);



#endif
