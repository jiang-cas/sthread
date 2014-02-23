#include "include/cond.h"
#include "include/semaphore.h"
#include "include/sync.h"
#include "include/mvspace.h"
#include "include/globals.h"


int sthread_cond_init(sthread_cond_t *cond, sthread_condattr_t *cond_attr)
{
	cond->cond = (struct cond_struct *)mvshared_malloc(sizeof(struct cond_struct));
	if(cond->cond) {
		int i;
		cond->cond->locks = new_sem(MAXTHREADS);
		for(i=0;i<MAXTHREADS;i++)
			init_sem(cond->cond->locks, i, 0);
		return 0;
	}
	return -1;
}

int sthread_cond_destroy(sthread_cond_t *cond)
{
	if(cond->cond) {
		del_sem(cond->cond->locks, 0);
		mvshared_free(cond->cond);
		return 0;
	}
	return -1;
}

int sthread_cond_wait(sthread_cond_t *cond, sthread_mutex_t *mutex)
{
	if(cond->cond) {
		setup_sync();
		wait_to_enter();
		__mvspace_commit();
		__threadpool[__selftid].state = E_STOPPED;
		__threadpool[__selftid].cond = cond->cond;
		
		v_next_and_wait();

		__mvspace_pull();

		wait_sem(cond->cond->locks, __selftid);
		__threadpool[__selftid].state = E_NORMAL;

		leave_sync();
		return 0;
	}
	return -1;
}

int sthread_cond_signal(sthread_cond_t *cond)
{
	if(cond->cond)
	{
		int i;
		setup_sync();
		wait_to_enter();
		__mvspace_commit();
		v_next_and_wait();
		__mvspace_pull();

		for(i=0;i<MAXTHREADS;i++)
			post_sem(cond->cond->locks, i);

		leave_sync();
			
		return 0;
	}
	return -1;
}

int sthread_cond_broadcast(sthread_cond_t *cond)
{
	return sthread_cond_signal(cond);
}
