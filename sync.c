

/* the lock will be inited once */
void setup_locks1(void)
{
	if(__sync_val_compare_and_swap(__initlock.val, 0, 1) == 0){
		int i;
		init_sem(__global_barrier1, 0);
		init_sem(__global_barrier2, 0);
		for(i=0;i<MAXTHREADS;i++) {
			init_sem(__threadpool[i].lock1, 0);
		}
		for(i=0;i<MAXTHREADS;i++) {
			if(__threadpool[i].state != E_NONE && __threadpool[i].state != E_STOPPED) {
				v_sem(__threadpool[i].locks1);
				break;
		}
	}
}

void v_next_and_wait(void)
{
	int i;
	for(i=__selftid+1;i<MAXTHREADS;i++) {
		if(__threadpool[i].state != E_NONE && __threadpool[i].state != E_STOPPED) {
			v_sem(__threadpool[i].locks1);
			p_sem(__global_barrier1);
			v_sem(__global_barrier1);
			return;
		}
	}
	v_sem(__global_barrier1);
}

void all_leaved(void)
{
	if()
}


void normal_sync(void)
{
	setup_locks1();
	p_sem(__threadpool[__selftid].locks1);
	__mvspace_commit();
	v_next_and_wait();
	__mvspace_pull();
	all_leaved();
	
}
