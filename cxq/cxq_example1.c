#include "cxq.h"

//#define CXQ_EXAMPLE1

#ifdef CXQ_EXAMPLE1

/* This example uses the primative type int as the data, and creates
   the data array statically.  To do this, you must define the memory 
   functions as shown below
*/

#include <stdio.h>
#include <string.h>

int main()
{
    cxq_t q;
    int slots = 10;

    /* Declare a static data array for the queue. */
    int data[slots];

    /* Point q's data element to data array. */
    q.data = data;

    /* Assign custom memory functions. */
    memfuns_t handlers = {
        .malloc_fn = NULL,
        .free_fn = NULL,
        .memcpy_fn = memcpy,
    };

    /* Initialize the queue. */
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

#endif /*CXQ_EXAMPLE1*/
