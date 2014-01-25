#include "include/barrier.h"


int sthread_barrier_init(sthread_barrier_t *barrier, const sthread_barrierattr_t *attr, unsigned count)
{
	barrier->barrier = (struct barrier_struct *)mvshared_malloc(sizeof(struct barrier_struct));
	if(barrier->barrier) {
		barrier->barrier->count = count;
		barrier->barrier->creator = __currenttask();
		return 0;
	}
	return -1;
}

int sthread_barrier_destroy(sthread_barrier_t *barrier)
{

}

int sthread_barrier_wait(sthread_barrier_t *barrier);
