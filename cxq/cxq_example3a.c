#include "cxq.h"

//#define CXQ_EXAMPLE3A

#ifdef CXQ_EXAMPLE3A

/* This example uses a simple struct type as the data, and creates
   the data array statically.  To do this, you must define the memory 
   functions as shown below.
*/

#include <stdio.h>
#include <string.h>

/* Data struct for the queue. */
typedef struct _person_t {
    char first_name[16];
    char last_name[16];
    int age;
} person_t;

/* Optional: to use cxq_traverse, you must define a callback. */
static void peekfun(const void *data)
{
    person_t *P = (person_t *)(data);
    printf("first_name: %s, last_name: %s, age %d\n",
           P->first_name, P->last_name, P->age);
}

int main()
{
    cxq_t q;
    int slots = 10;

    /* Declare a static data array for the queue. */
    person_t data[slots];

    /* Point q's data element to data array. */
    q.data = data;

    /* Assign custom memory functions. */
    memfuns_t handlers = {
        .malloc_fn = NULL,
        .free_fn = NULL,
        .memcpy_fn = memcpy,
    };

    /* Initialize the queue. */
    cxq_init(&q, slots, sizeof(person_t), &handlers);

    /* Populate the queue. */
    printf("enqueue:\n");
    person_t P;
    for (int i = 0; i < 8; i++) {
        P.age = i * 10;
        switch (i) {
        case 0: strcpy(P.first_name, "aaa"); strcpy(P.last_name, "AAA"); break;
        case 1: strcpy(P.first_name, "bbb"); strcpy(P.last_name, "BBB"); break;
        case 2: strcpy(P.first_name, "ccc"); strcpy(P.last_name, "CCC"); break;
        case 3: strcpy(P.first_name, "ddd"); strcpy(P.last_name, "DDD"); break;
        case 4: strcpy(P.first_name, "eee"); strcpy(P.last_name, "EEE"); break;
        case 5: strcpy(P.first_name, "fff"); strcpy(P.last_name, "FFF"); break;
        case 6: strcpy(P.first_name, "ggg"); strcpy(P.last_name, "GGG"); break;
        case 7: strcpy(P.first_name, "hhh"); strcpy(P.last_name, "HHH"); break;
        }

        if (!cxq_enqueue(&q, &P))
            printf("queue full!\n");
    }

    /* Put an item at the front of the queue. */
    P.age = 101;
    strcpy(P.first_name, "farrell");
    strcpy(P.last_name, "aultman");
    if (!cxq_enqueue_front(&q, &P))
        printf("queue full!!\n");
    
    /* Check queue status. */
    printf("cxq_isempty = %d\n", cxq_isempty(&q));
    printf("cxq_isfull = %d\n", cxq_isfull(&q));
    printf("cxq_slots_filled = %d\n", cxq_slots_filled(&q));
    printf("cxq_slots_empty = %d\n", cxq_slots_empty(&q));
    
    /* Traverse the queue. */
    printf("peek:\n");
    cxq_traverse(&q, peekfun);

    /* Retrieve queue elements. */
    printf("dequeue:\n");
    while (!cxq_isempty(&q))
    {
        if (cxq_dequeue(&q, (void *)&P, true)) {
            printf("first_name = %s, last_name = %s, age = %d\n",
                   P.first_name, P.last_name, P.age);
        }
    }

    /* Deinitialize the queue. */
    cxq_finish(&q);
}

#endif /*CXQ_EXAMPLE3A*/
