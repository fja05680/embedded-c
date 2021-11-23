#include "cxq.h"

//#define CXQ_EXAMPLE5

#ifdef CXQ_EXAMPLE5

/* This example uses a complex struct type as the data, and creates
   the data array statically.  However, the memory of some of the struct
   elements are allocated dynamically.  It uses custom memory functions.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* The data struct for the queue. */
typedef struct _person_t {
    char *first_name;
    char *last_name;
    int age;
} person_t;

#define NUM_SLOTS 10

/* Make q global, and also the data array. */
static cxq_t q;
static person_t data[NUM_SLOTS];

/* `array_len` recorded by malloc and used later.  Could have also just
   made `slots` global.
*/
   
static int array_len;

/* Custom malloc function - required to create memory for
   dynamic struct elements.
*/
void * my_malloc(size_t size)
{
    person_t *data = (person_t *)q.data;
    array_len = size / sizeof(person_t);

    for (int i = 0; i < array_len; i++) {
        data[i].first_name = malloc(32*sizeof(char));
        data[i].last_name = malloc(32*sizeof(char));
    }
    return (void *)data;
}

/* Custom free function - first free memory for struct elements,
   then for array.
*/
void my_free(void *ptr)
{
    person_t *data = (person_t *)ptr;
    for (int i = 0; i < array_len; i++) {
        free(data[i].first_name);
        free(data[i].last_name);
    }
}

/* Custom memcpy functon - allows copying of each struct element
   individually, so that pointer values aren't overwritten.
*/
void * my_memcpy(void *dest, const void *src, size_t n)
{
    person_t *A = (person_t *)dest;
    person_t *B = (person_t *)src;

    strcpy(A->first_name, B->first_name);
    strcpy(A->last_name, B->last_name);
    A->age = B->age;
    return (void *)A;
}

/* Optional: to use cxq_traverse, you must define a callback. */
static void peekfun(const void *data)
{
    person_t *P = (person_t *)(data);
    printf("first_name: %s, last_name: %s, age %d\n",
           P->first_name, P->last_name, P->age);
}

int main()
{
    person_t P;

    /* Point q's data element to data array. */
    q.data = data;

    /* Assign custom memory functions. */
    memfuns_t handlers = {
        .malloc_fn = my_malloc,
        .free_fn = my_free,
        .memcpy_fn = my_memcpy,
    };

    /* Initialize the queue. */
    cxq_init(&q, NUM_SLOTS, sizeof(person_t), &handlers);

    /* Populate the queue. */
    printf("enqueue:\n");
    for (int i = 0; i < 8; i++) {
        P.age = i * 10;
        switch (i) {
        case 0: P.first_name = "aaa"; P.last_name = "AAA"; break;
        case 1: P.first_name = "bbb"; P.last_name = "BBB"; break;
        case 2: P.first_name = "ccc"; P.last_name = "CCC"; break;
        case 3: P.first_name = "ddd"; P.last_name = "DDD"; break;
        case 4: P.first_name = "eee"; P.last_name = "EEE"; break;
        case 5: P.first_name = "fff"; P.last_name = "FFF"; break;
        case 6: P.first_name = "ggg"; P.last_name = "GGG"; break;
        case 7: P.first_name = "hhh"; P.last_name = "HHH"; break;
        }

        if (!cxq_enqueue(&q, &P))
            printf("queue full!\n");
    }
  
    /* Put an item at the front of the queue. */
    P.age = 101;
    P.first_name = "farrell";
    P.last_name = "aultman";
    if (!cxq_enqueue_front(&q, &P))
        printf("queue full!!\n");

    /* Check queue status. */
    printf("queue_isempty = %d\n", cxq_isempty(&q));
    printf("queue_isfull = %d\n", cxq_isfull(&q));
    printf("queue_slots_filled = %d\n", cxq_slots_filled(&q));
    printf("queue_slots_empty = %d\n", cxq_slots_empty(&q));

    /* Traverse the queue. */
    printf("peek:\n");
    cxq_traverse(&q, peekfun);

    /* Allocate memory for elements in retrieve struct. */
    P.first_name = malloc(32 * sizeof(char));
    P.last_name = malloc(32 * sizeof(char));

    /* Retrieve queue elements. */
    printf("dequeue:\n");
    while (!cxq_isempty(&q)) {
        if (cxq_dequeue(&q, (void *)&P, true))
            printf("first_name = %s, last_name = %s, age = %d\n",
                   P.first_name, P.last_name, P.age);
    }

    /* Deinitialize the queue. */
    cxq_finish(&q);
}

#endif /*CXQ_EXAMPLE5*/