### Description of Files

 - `cxq.{c,h}` - complex queue module.
 - `cxq_example1.c` - This example uses the **primitive type `int`** as the data, and creates the data array **statically**.  To do this, you must define the memory 
   functions.
 - `cxq_example2.c` - This example uses the **primitive type 'int'** as the data, and creates the data array **dynamically**.  Here, we demonstrate how to supply our own memory functions; in this example, they are just wrappers for
   the built-in calls.
 - `cxq_example3.c` - This example uses a **simple 'struct'** type as the data, and creates the data array **dynamically**.  It uses the built-in memory functions.
 - `cxq_example3a.c` - This example uses a **simple 'struct'** type as the data, and creates the data array **statically**.  To do this, you must define the memory 
   functions.
 - `cxq_example4.c` - This example uses a **complex `struct`** type as the data, and creates the data array **dynamically**, as well as the memory of some of the struct
   elements.  It uses custom memory functions.
 - `cxq_example5.c` - This example uses a **complex `struct`** type as the data, and creates the data array **statically**.  However, the memory of some of the struct
   elements are allocated dynamically.  It uses custom memory functions.
 - `cxq_example6.c` - This example uses a **complex `struct` type** as the data, and creates the data array **dynamically**, as well as the memory of some of the struct
   elements.  It uses custom memory functions.  Data is dequeued by pointer instead of by data copy.
 - `cxq_example7.c` - Demonstrates using the **circular queue option**, i.e. ring buffer.  This example uses the **primitive type `int`** as the data, and creates
   the data array **statically**.  To do this, you must define the memory functions.
