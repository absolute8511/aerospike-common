/*
 *  Citrusleaf Foundation
 *  src/queue.c - queue framework
 *
 *  Copyright 2008 by Citrusleaf.  All rights reserved.
 *  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE.  THE COPYRIGHT NOTICE
 *  ABOVE DOES NOT EVIDENCE ANY ACTUAL OR INTENDED PUBLICATION.
 */
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include "cf.h"



/* SYNOPSIS
 * Queue
 */


/* cf_queue_create
 * Initialize a queue */
cf_queue *
cf_queue_create()
{
	cf_queue *q = NULL;

	q = calloc(1, sizeof(cf_queue) + (CF_QUEUE_ALLOCSZ * sizeof(void *)));
	/* FIXME error msg */
	if (NULL == q)
		return(q);
	q->allocsz = CF_QUEUE_ALLOCSZ;

	if (0 != pthread_mutex_init(&q->LOCK, NULL)) {
		/* FIXME error msg */
		free(q);
		return(NULL);
	}
	/* FIXME error checking */
	pthread_cond_init(&q->CV, NULL);

	return(q);
}


/* cf_queue_push
 * */
int
cf_queue_push(cf_queue *q, void *ptr)
{
	/* FIXME arg check */

	/* FIXME error */
	if (0 != pthread_mutex_lock(&q->LOCK))
		return(-1);

	/* Check queue length */
	if (q->allocsz <= q->utilsz + 1) {
		/* FIXME freak out */
		if (NULL == (q = realloc(q, (sizeof(cf_queue) + ((q->allocsz + CF_QUEUE_ALLOCSZ) * sizeof(void *))))))
			return(-1);
		q->allocsz += CF_QUEUE_ALLOCSZ;
	}

	q->queue[q->utilsz++] = ptr;
	pthread_cond_signal(&q->CV);

	/* FIXME blow a gasket */
	if (0 != pthread_mutex_unlock(&q->LOCK))
		return(-1);

	return(0);
}


/* cf_queue_pop
 * */
void *
cf_queue_pop(cf_queue *q, int block)
{
	void *ptr = NULL;

	if (NULL == q)
		return(NULL);

	/* FIXME error checking */
	if (0 != pthread_mutex_lock(&q->LOCK))
		return(NULL);

	/* FIXME error checking */
	if (0 == q->utilsz) {
		if (TRUE == block)
			pthread_cond_wait(&q->CV, &q->LOCK);
		else {
			pthread_mutex_unlock(&q->LOCK);
			return(NULL);
		}
	}

	/* FIXME this is not the right behavior */
	ptr = q->queue[--q->utilsz];

	/* FIXME blow a gasket */
	if (0 != pthread_mutex_unlock(&q->LOCK)) {
		fprintf(stderr, "unlock failed\n");
		return(NULL);
	}

	return(ptr);
}
