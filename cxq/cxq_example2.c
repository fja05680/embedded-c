#include "cxq.h"

//#define CXQ_EXAMPLE2

#ifdef CXQ_EXAMPLE2

/* This example uses the primative type int as the data, and creates
   the data array dynamically.  Here, we demonstrate how to supply
   our own memory functions; in this example, they are just wrappers for
   the built-in calls.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* custom memory functions */
void * my_malloc(size_t size) {return malloc(size);}
void   my_free(void *ptr) {free(ptr);}
void * my_memcpy(void *dest, const void *src, size_t n)
            {return memcpy(dest, src, n);}

int main()
{
    cxq_t q;
    int slots = 10;

    /* Assign custom memory functions. */
    memfuns_t handlers= {
        .malloc_fn = my_malloc,
        .free_fn = my_free,
        .memcpy_fn = my_memcpy,
    };

    /* Initalize the queue struct. */
    cxq_init(&q, slots, sizeof(int), &handlers);

    /* Populate the queue. */
    for (int i = 0; i < 8; i++) {
        if (!cxq_enqueue(&q, &i))
            printf("queue full!\n");
    }

    /* Put an item at the front of the queue. */
    int i = 101;
    if (!cxq_enqueue_front(&q, &i))
        printf("queue full!!\n");

    /* Check queue status. */
    printf("cxq_isempty = %d\n", cxq_isempty(&q));
    printf("cxq_isfull = %d\n", cxq_isfull(&q));
    printf("cxq_slots_filled = %d\n", cxq_slots_filled(&q));
    printf("cxq_slots_empty = %d\n", cxq_slots_empty(&q));

    /* Retrieve queue elements. */
    while (!cxq_isempty(&q)) {
        int data;
        if (cxq_dequeue(&q, &data, true))
            printf("data = %d\n", data);
    }

    /* Deinitialize the queue. */
    cxq_finish(&q);
}

#endif /*CQX_EXAMPLE2*/
