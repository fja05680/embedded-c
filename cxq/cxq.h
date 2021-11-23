/******************************************************************************

 cxq.h
 
*******************************************************************************/

#ifndef CXQ_H
#define CXQ_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define CXQ_VERSION "0.9.0"

//#define MULTI_THREAD

#ifndef NOP
#define NOP ((void) 0)
#endif

/* Mutex helpers. */
#ifdef MULTI_THREAD
#define LOCK(mutex_id)              osMutexAcquire((mutex_id), osWaitForever)
#define UNLOCK(mutex_id)            osMutexRelease((mutex_id))
#define MUTEX_INIT(mutex_id, attr)  ((mutex_id) = osMutexNew((attr)))
#define MUTEX_DESTROY(mutex_id)     osMutexRelease((mutex_id))
#else
#define LOCK(mutex_id)              NOP
#define UNLOCK(mutex_id)            NOP
#define MUTEX_INIT(mutex_id, attr)  NOP
#define MUTEX_DESTROY(mutex_id)     NOP
#endif /* MULTI_THREAD */

/* Semaphore helpers. */
#ifdef MULTI_THREAD
#define SEM_WAIT(sem_id, timeout)   osSemaphoreAcquire((sem_id), \
            (timeout) == osWaitForever ? osWaitForever: portTICK_PERIOD_MS * (timeout))
#define SEM_SIGNAL(sem_id)          osSemaphoreRelease((sem_id))
#define SEM_INIT(sem_id, max, init) ((sem_id) = osSemaphoreNew((max), (init), NULL))
#define SEM_DESTROY(sem_id)         osSemaphoreDelete((sem))
#else
#define SEM_WAIT(sem_id, timeout)   NOP
#define SEM_SIGNAL(sem_id)          NOP
#define SEM_INIT(sem_id, max, init) NOP
#define SEM_DESTROY(sem_id)         NOP
#endif /* MULTI_THREAD */


typedef struct _memfuns_t {
    void * (*malloc_fn)(size_t size);
    void   (*free_fn)(void *ptr);
    void * (*memcpy_fn)(void *dest, const void *src, size_t n);
} memfuns_t;

typedef struct _cxq_t {
    void *data;             /* Pointer to body of queue. */
    int first;              /* Position of first element. */
    int count;              /* Number of queue elements. */
    int slots;              /* Number of queue slots. */
    int data_size;          /* Size of each element. */
    bool circular;          /* This is a circular buffer. */
    memfuns_t *handlers;    /* Memory callback functions. */
#ifdef MULTI_THREAD
    osMutexId_t lock;       /* queue lock */
#endif
} cxq_t;

/* construction/destruction */
void cxq_init(cxq_t *q, int slots, int data_size, memfuns_t *handlers);
void cxq_finish(cxq_t *q);

/* enqueue/dequeue/flush */
void * cxq_enqueue(cxq_t *q, const void *data);
void * cxq_enqueue_front(cxq_t *q, const void *data);
void * cxq_dequeue(cxq_t *q, void *data, bool remove);
void   cxq_flush(cxq_t *q);

/* get/set queue options */
bool cxq_get_circular(const cxq_t *q);
void cxq_set_circular(cxq_t *q);
int cxq_get_first(const cxq_t *q);
void cxq_set_first(cxq_t *q, int first);
int cxq_get_last(const cxq_t *q);
int cxq_get_count(const cxq_t *q);
int cxq_get_slots(const cxq_t *q);
int cxq_get_data_size(const cxq_t *q);

/* isempty/isfull/slots_empty/slots_filled */
bool cxq_isempty(const cxq_t *q);
bool cxq_isfull(const cxq_t *q);
int cxq_slots_empty(const cxq_t *q);
int cxq_slots_filled(const cxq_t *q);

/* Dianostics */
typedef void (*cxq_callback_t)(const void *data);
void cxq_traverse(const cxq_t *q, cxq_callback_t peekfun);


#endif /*CXQ_H*/
