#include "pthread_helper.h"

/* Initialize the barrier reference by the supplied pointer to syncronize count threads */

void barrier_init(barrier_t* barrier, int count) {
	pthread_mutex_init(&barrier->mutex, 0);
	pthread_cond_init(&barrier->cond, 0);
	barrier->trip_count = count;
	barrier->count = 0;
}

/* Synchronize count threads by calling the following function */

void barrier_wait(barrier_t* barrier) {
	pthread_mutex_lock(&barrier->mutex);
	++(barrier->count);
	if (barrier->count >= barrier->trip_count) {
		barrier->count = 0;
		pthread_cond_broadcast(&barrier->cond);
		pthread_mutex_unlock(&barrier->mutex);
		return;
	} else {
		pthread_cond_wait(&barrier->cond, &barrier->mutex);
		pthread_mutex_unlock(&barrier->mutex);
		return;
	}
}