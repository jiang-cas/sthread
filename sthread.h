#ifndef STHREAD_H
#define STHREAD_H

#ifdef __cplusplus
extern "C" {
#endif

	typedef unsigned long size_t;

#define MAXTHREADS 32

#define mspace void *

	void *mvprivate_malloc(size_t bytes);

	void mvprivate_free(void *mem);

	void *mvshared_malloc(size_t bytes);

	void mvshared_free(void *mem);

	struct mutex_struct
	{
		int lock[MAXTHREADS];
		int inited;
	};

	typedef struct sthread_mutex_t
	{
		struct mutex_struct *mutex;
	} sthread_mutex_t;

	typedef struct sthread_mutexattr_t
	{} sthread_mutexattr_t;

	int sthread_mutex_init(sthread_mutex_t *mutex, const sthread_mutexattr_t *attr);

	int sthread_mutex_destroy(sthread_mutex_t *mutex);

	int sthread_mutex_lock(sthread_mutex_t *mutex);

	int sthread_mutex_unlock(sthread_mutex_t *mutex);

	struct barrier_struct 
	{
		int total;
		int num;
		int sema;
	};

	typedef struct sthread_barrier_t 
	{
		struct barrier_struct *barrier;
	} sthread_barrier_t;

	typedef struct sthread_barrierattr_t {} sthread_barrierattr_t;

	int sthread_barrier_init(sthread_barrier_t *barrier, const sthread_barrierattr_t *attr, unsigned int count);

	int sthread_barrier_wait(sthread_barrier_t *barrier);

	int sthread_barrier_destroy(sthread_barrier_t *barrier);


	struct cond_struct {
			int locks;
	};

	typedef struct sthread_cond_t
	{
			struct cond_struct *cond;
	} sthread_cond_t;

	typedef struct sthread_condattr_t
	{
	} sthread_condattr_t;

	int sthread_cond_init(sthread_cond_t *cond, sthread_condattr_t *cond_attr);

	int sthread_cond_signal(sthread_cond_t *cond);

	int sthread_cond_broadcast(sthread_cond_t *cond);

	int sthread_cond_wait(sthread_cond_t *cond, sthread_mutex_t *mutex);

	int sthread_cond_timewait(sthread_cond_t *cond, sthread_mutex_t *mutex, const struct timespec *abstime);

	int sthread_cond_destroy(sthread_cond_t *cond);

	struct heap_struct
	{
			void *privatebase;
			mspace privatemsp;
			void *sharedbase;
			mspace sharedmsp;
	};

	typedef struct sthread_t
	{
			unsigned int tid;
			void *retval;
			int pid;
			struct heap_struct heap;
			int state;
			int lock1;
			int lock2;
			int joinlock;
			struct mutex_struct *mutex;
			struct barrier_struct *barrier;
			int leaved;

	} sthread_t;

	typedef struct sthread_attr_t 
	{
			unsigned int stack_size;
	} sthread_attr_t;

	void sthread_init(void);
	int sthread_create(sthread_t *newthread, sthread_attr_t *attr, void *(*func)(void *), void *args);
	sthread_t sthread_self(void);
	void sthread_clear(void);
	void sthread_shared(void *addr, size_t size);
	int sthread_join(sthread_t thread, void **thread_return);
	void sthread_main_wait(int n);
	void sthread_exit(int n);
#ifdef __cplusplus
}
#endif
#endif
