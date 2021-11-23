/******************************************************************************

 cxq.c - generic complex queue implementation

*******************************************************************************/

#include <string.h>
#include <stdlib.h>

#include "cxq.h"

#define TEST_CXQ


/*
  Description
    Init the queue.

  Parameters
    q          - Pointer to cxq_t struct.
    slots      - Number of queue positions. 
    data_size  - The size of each queue element.
    handlers   - Pointer to memfuns_t stuct that manages memory allocation
                 for queue elements. 

  Returns
    None
*/
void cxq_init(cxq_t *q, int slots, int data_size, memfuns_t *handlers)
{
    q->first = 0;
    q->count = 0;
    q->slots = slots;
    q->circular = false;
    q->data_size = data_size;
    q->handlers = malloc(sizeof(memfuns_t));
    if (handlers) {
        q->handlers->malloc_fn = handlers->malloc_fn;
        q->handlers->free_fn = handlers->free_fn;
        q->handlers->memcpy_fn = handlers->memcpy_fn;
    } else { /* Default handlers. */
        q->handlers->malloc_fn = malloc;
        q->handlers->free_fn = free;
        q->handlers->memcpy_fn = memcpy;
    }
    if (q->handlers->malloc_fn)
        q->data = q->handlers->malloc_fn(slots * data_size);
    MUTEX_INIT(q->lock, NULL);
}


/*
  Description
    De-init queue, free memory.

  Parameters
    q          - Pointer to cxq_t struct. 

  Returns
    None
*/
void cxq_finish(cxq_t *q)
{
    MUTEX_DESTROY(q->lock);
    if (q->handlers->free_fn)
        q->handlers->free_fn(q->data);
    free(q->handlers);
}


/*
  Description
    Retrieve an element from end of queue, optionally remove from queue.
    User code must not call this function directly.

  Parameters
    q          - Pointer to cxq_t struct.
    data       - Pointer to destination memory for dequeued element.
                 If data is NULL, a pointer to the actual data element
                 in the queue is returned.
                 Otherwise, memory for `data` must be allocated by
                 the caller.
    remove     - true to remove the element from the queue.
                 false to leave the element in the queue.

  Returns
    slot - A pointer to the dequeued data element.

  Note
    In the case when `data` is set to NULL by the caller and
    `remove` is set to true, `data` will be valid until
    it's overwritten.
*/
static void * _cxq_dequeue(cxq_t *q, void *data, bool remove)
{
    void * slot;
    if (q->count <= 0) {
        /* No data */
        slot = NULL;
    } else {
        slot = q->data + q->first * q->data_size;
        if (data)
            q->handlers->memcpy_fn(data, slot, q->data_size);
        if (remove) {
            q->first = (q->first + 1) % q->slots;
            q->count--;
        }
    }
    return slot;
}


/* Public wrapper for _cxq_dequeue. */
void * cxq_dequeue(cxq_t *q, void *data, bool remove)
{
  void * slot;
  LOCK(q->lock);
  slot = _cxq_dequeue(q, data, remove);
  UNLOCK(q->lock);
  return slot;
}


/*
  Description
    Add an element to end of queue.
    User code must not call this function directly.

  Parameters
    q          - Pointer to cxq_t struct.
    data       - Pointer to source memory for enqueued element.

  Returns
    slot - A pointer to the enqueued data element.  Returns NULL
    if there are no open slots available, i.e. queue is full.
*/
static void * _cxq_enqueue(cxq_t *q, const void *data)
{
    void * slot;
    if (q->count >= q->slots) {
        /* No open slots available. */
        slot = NULL;
    } else {
        int next = (q->first + q->count) % q->slots;
        slot = q->data + next * q->data_size;
        if (q->handlers->memcpy_fn)
            q->handlers->memcpy_fn(slot, data, q->data_size);
        q->count++;
    }
    return slot;
}


/* Public wrapper for _cxq_enqueue. */
void * cxq_enqueue(cxq_t *q, const void *data)
{
    void * slot;
    LOCK(q->lock);
    if (q->circular && cxq_isfull(q))
        _cxq_dequeue(q, NULL, true);
    slot = _cxq_enqueue(q, data);
    UNLOCK(q->lock);
    return slot;
}


/* Same as _cxq_enqueue, except element to front of queue
   User code must not call this function directly.
*/
static void * _cxq_enqueue_front(cxq_t *q, const void *data)
{
    void * slot;
    if (q->count >= q->slots) {
        /* No open slots available. */
        slot = NULL;
    } else {
        q->first = q->first ? q->first - 1 : q->slots -1;
        slot = q->data + q->first * q->data_size;
        q->handlers->memcpy_fn(slot, data, q->data_size);
        q->count++;
    }
    return slot;
}


/* Public wrapper for _enqueue_front. */
void * cxq_enqueue_front(cxq_t *q, const void *data)
{
    void * slot;
    LOCK(q->lock);
    if (q->circular && cxq_isfull(q))
        _cxq_dequeue(q, NULL, true);
    slot = _cxq_enqueue_front(q, data);
    UNLOCK(q->lock);
    return slot;
}


/* Remove all elements from queue. */
void cxq_flush(cxq_t *q)
{
    while (!cxq_isempty(q))
        cxq_dequeue(q, NULL, true);
}


/* Returns true if queue is a ring buffer, otherwise false. */
bool cxq_get_circular(const cxq_t *q) {return q->circular;}

/* Make queue a ring buffer. */
void cxq_set_circular(cxq_t *q) {q->circular = true;}

/* Get position of first element in queue. */
int cxq_get_first(const cxq_t *q) {return q->first;}

/* Set starting position for queue. */
void cxq_set_first(cxq_t *q, int first)
{
    if (q->first < q->slots)
        q->first = first;
}

/* Get position of last element in queue. */
int cxq_get_last(const cxq_t *q)
    {return (q->first + q->count - 1) % q->slots;}

/* Returns num of elements in queue. */
int cxq_get_count(const cxq_t *q) {return q->count;}

/* Returns data size of a queue element. */
int cxq_get_data_size(const cxq_t *q) {return q->data_size;}


/* Returns true if queue is empty. */
bool cxq_isempty(const cxq_t *q) {return q->count <= 0;}

/* Returns true if queue is full. */
bool cxq_isfull(const cxq_t *q) {return q->count >= q->slots;}

/* Returns number of data slots. */
int cxq_get_slots(const cxq_t *q) {return q->slots;}

/* Returns num of empty slots. */
int cxq_slots_empty(const cxq_t *q) {return q->slots - q->count;}

/* Returns num of filled slots, same as cxq_get_count(). */
int cxq_slots_filled(const cxq_t *q) {return q->count;}


/*
  Description
    Visit each element in queue, call peekfun for each element.

  Parameters
    q          - Pointer to cxq_t struct.
    peekfun    - Callback function to call for each element.

  Returns
    None
*/
void cxq_traverse(const cxq_t *q, cxq_callback_t peekfun)
{
    LOCK(q->lock);
    int end = cxq_get_last(q);
    for (int i = q->first; i != end; i = (i+1) % q->slots)
        peekfun(q->data + i * q->data_size);
    UNLOCK(q->lock);
}


#ifdef TEST_CXQ

/* This example uses the primitive type int as the data, and creates
   the data array dynamically.  It uses the built in memory functions.
*/

#include <stdio.h>

/* Optional: to use cxq_traverse, you must define a callback. */
static void peekfun(const void *data)
{
    int *i = (int *)data;
    printf("peek: %d\n", *i);
}

int main()
{
    cxq_t q;
    int slots = 10;

    /* Initialize the queue struct. */
    cxq_init(&q, slots, sizeof(int), NULL);

    /* Populate the queue. */
    for (int i = 0; i < 8; i++)
    {
        if (!cxq_enqueue(&q, &i))
            printf("queue full!\n");
        else
            printf("enqueue: %d\n", i);
    }

    /* Put an item at the front of the queue. */
    int i = 101;
    if (!cxq_enqueue_front(&q, &i))
        printf("queue full!!\n");
    else
        printf("enqueue_front: %d\n", i);

    /* Check queue status. */
    printf("cxq_isempty = %d\n", cxq_isempty(&q));
    printf("cxq_isfull = %d\n", cxq_isfull(&q));
    printf("cxq_slots_filled = %d\n", cxq_slots_filled(&q));
    printf("cxq_slots_empty = %d\n", cxq_slots_empty(&q));

    /* Traverse the queue. */
    cxq_traverse(&q, peekfun);

    /* Retrieve queue elements. */
    while (!cxq_isempty(&q))
    {
        int data;
        if (cxq_dequeue(&q, &data, true))
            printf("dequeue: %d\n", data);
    }

    /* Re-populate the queue. */
    for (int i = 0; i < 8; i++)
    {
        if (!cxq_enqueue(&q, &i))
            printf("queue full!\n");
        else
            printf("enqueue: %d\n", i);
    }

    /* Flush queue. */
    printf("before flush\n");
    printf("cxq_slots_filled = %d\n", cxq_slots_filled(&q));
    cxq_flush(&q);
    printf("after flush\n");
    printf("cxq_slots_filled = %d\n", cxq_slots_filled(&q));

    /* Deinitialize the queue. */
    cxq_finish(&q);
}

#endif /*TEST_CXQ*/
