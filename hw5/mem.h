#include <stdlib.h>   // for malloc() and free()
#include <stdio.h> //test print statements


typedef enum { FIRST, NEXT, BEST } mem_strategy_t;

int mem_allocate(mem_strategy_t strategy, unsigned int size, unsigned int duration);

int mem_single_time_unit_transpired();

int mem_fragment_count(int frag_size);

void mem_clear();

void mem_init( unsigned int size );

void mem_free();

// added this for debugging purposes
void mem_print();
