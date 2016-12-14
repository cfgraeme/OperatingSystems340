
#include "mem.h"

/*
  Physical memory array. This is a static global array for all functions in this file.  
  An element in the array with a value of zero represents a free unit of memory.
*/
static unsigned int* memory;

/*
 The size (i.e. number of units) of the physical memory array. This is a static global 
 variable used by functions in this file.  

 */
static unsigned int mem_size;


/*
 The last_placement_position variable contains the end position of the last 
 allocated unit used by the next fit placement algorithm.  
 */
static unsigned int last_placement_position;

/*FIRST algorithm, returns number of probes and allocates memory */
int first_algorithm(unsigned int size, unsigned int duration){
  int index = 0;
  int alloced = 0;
  int count = 0;
  int probes = 0;
  int i = 0;
  while (alloced < 1 && i < mem_size){ //no blocks large enough and not through array
    if (count>= size){ //found
      //printf("found at %d\n",index);
      for(int j = 0; j < size; j++){
        memory[index + j] = duration;
      }
      probes++;
      alloced = 1;
    }
    else if (memory[i] == 0){ //empty unit?
      count++;
      if (i == mem_size -1){//last unit?
        if(count >= size){ //large enough?
          alloced = 1;
          for(int j = 0; j < size; j++){
            memory[index + j] = duration;
          }
        }
        probes++;
      }
      //printf("increment count %d at cell %d\n", count, i);
    }
    else if (count>0 && count < size){ //probed, not found
      probes++;
      count = 0;
      index = i+1;
      //printf("probed at %d, new index %d\n", i, index);
    }
    else{
      count = 0;
      index = i+1;
      //printf("full at %d, new index %d\n", i, index);
    }
    i++;
  }
  //printf("run through, index %d, count %d, probes %d, alloc %d, i %d\n", index, count, probes, alloced, i);

  if (alloced == 0){
    probes = -1;
    //printf("FAILED\n");
  }
  return probes;
}
/*NEXT algorithm, returns number of probes and allocates memory */
int next_algorithm(unsigned int size, unsigned int duration){
  int index = last_placement_position;
  int alloced = 0;
  int count = 0;
  int probes = 0;
  int i = 0;
  while (alloced < 1 && i < mem_size){ //no blocks large enough and not through array
     if((i+last_placement_position)%mem_size == 0 && count < size && i != 0){ //wrapped?
      //printf("wrapped when i is %d, count is %d", i, count);
      count = 0;
      index = 0;
      probes++;
    }

    if (count>= size){ //found
      //printf("found at %d\n",index);
      for(int j = 0; j < size; j++){
        memory[index + j] = duration;
      }
      last_placement_position = index + size - 1;
      //printf("last placement at %d\n", last_placement_position);
      alloced = 1;
      probes++;
    }
    else if (memory[(i+last_placement_position)%mem_size] == 0){ //empty unit?
      count++;
      if((i+last_placement_position)%mem_size == mem_size - 1){ //last unit?
        if(count>=size){ //large enough
          for(int j = 0; j < size; j++){
            memory[index + j] = duration;
          }
          alloced = 1;
          probes++;
        }
      }

      //printf("increment count %d at cell %d\n", count, (i+last_placement_position)%mem_size);
    }
    else if (count>0){ //probed, not found
      probes++;
      count = 0;
      index = (i+last_placement_position)%mem_size + 1;
      //printf("probed at %d, new index %d\n", (i+last_placement_position)%mem_size, index);
    }
    else{
      count = 0;
      index = (i+last_placement_position)%mem_size+1;
      //printf("full at %d, new index %d\n", (i+last_placement_position)%mem_size, index);
    }
    i++;
  }
  //printf("run through, index %d, count %d, probes %d, alloc %d, i modded %d\n", index, count, probes, alloced, (i+last_placement_position)%mem_size);

  if (i >= mem_size && alloced == 0){
    probes = -1;
    //printf("FAILED\n");
  }
  return probes;
}

/*BEST algorithm, returns number of probes and allocates memory */
int best_algorithm(unsigned int size, unsigned int duration){
  int index = 0;
  int best_index = 0;
  int best_size = mem_size;
  int alloced = 0;
  int perfect_fit = 0;
  int count = 0;
  int probes = 0;
  int i = 0;
  while (i < mem_size && perfect_fit == 0){ //no blocks large enough and not through array
    if (memory[i] == 0){ //empty unit?
      count++;
      //printf("increment count %d at cell %d\n", count, i);
      if(i == mem_size - 1){ //last cell
        if (count>= size){ //found
          //printf("found at %d, size %d\n",index, count);
          if (count < best_size){
            //printf("CURRENT BEST\n");
            best_size = count;
            best_index = index;
            probes++;
          }
          else{ //not our best size, so counts as a probe
            probes++;
          }
          count = 0;
          alloced = 1;
        }
      }
    }
    else if (count == size){
      best_size = count;
      best_index = index;
      count = 0;
      alloced = 1;
      perfect_fit = 1;
      probes++;
    }
    else if (count>= size){ //found a large enough block
      //printf("found at %d, size %d\n",index, count);
      if (count < best_size){ //is current best
        best_size = count;
        best_index = index;
      }
      count = 0;
      alloced = 1;
      probes++;
    }
    else if (count>0){ //probed, not found
      probes++;
      count = 0;
      index = i+1;
      //printf("probed at %d, new index %d\n", i, index);
    }
    else{
      count = 0;
      index = i+1;
      //printf("full at %d, new index %d\n", i, index);
    }
    i++;
  }
  //printf("run through, index %d, proc_size %d,count %d, probes %d, alloc %d, i %d\n", index, size, count, probes, alloced, i);

  if (alloced == 0){
    probes = -1;
    //printf("FAILED\n");
  }
  else{
    for (int j = 0; j < size; j++){
      memory[j+best_index] = duration;
    }
    //printf("best index at %d of size %d", best_index, best_size);
  }
  return probes;
}



/*
  Using the memory placement algorithm, strategy, allocate size
  units of memory that will reside in memory for duration time units.

  If successful, this function returns the number of contiguous blocks
  (a block is a contiguous "chuck" of units) of free memory probed while 
  searching for a suitable block of memory according to the placement 
  strategy specified.  If unsuccessful, return -1.

  If a suitable contiguous block of memory is found, the units of this 
  block must be set to the value, duration.
 */
int mem_allocate(mem_strategy_t strategy, unsigned int size, unsigned int duration)
{
  //printf("MEMALLOC: %d, %u, %u\n ", strategy, size, duration);
  
  int probes;
  if (strategy == 0){
    probes = first_algorithm(size, duration);
  }
  else if (strategy == 1){ //Next Fit Algorithm
    probes = next_algorithm(size, duration);
  }
  else if(strategy == BEST){ //Algorithm for Best Fit!
    probes = best_algorithm(size, duration);
  }
  else{
    printf("invalid strategy: %d", strategy);
    probes = -1;
  }
  //mem_print();
  return probes;
}

/*
  Go through all of memory and decrement all positive-valued entries.
  This simulates one unit of time having transpired.
 */
int mem_single_time_unit_transpired()
{
  int x;
  for (x =0; x < mem_size; x++){
    if (memory[x] > 0){
      memory[x] = memory[x] -1;
    }
  }
  return 1;
}

/*
  Return the number of fragments in memory.  A fragment is a
  contiguous free block of memory of size less than or equal to
  frag_size.
 */
int mem_fragment_count(int frag_size)
{
  int x;
  int blockcount = 0;
  int fragcount = 0;
  for (x =0; x < mem_size; x++){
    if (x == mem_size - 1 && memory[x] == 0){ //last block!
        fragcount++;
      }
    else if (memory[x] == 0){ //count free blocks
      blockcount++;
    }
    else if(blockcount <= frag_size && blockcount > 0) //end of free block, check size
    {
      fragcount++;
      blockcount = 0;
    }
    else{ //free block is not larger than frag_size
      blockcount = 0;
    }
  }
  return fragcount;

}

/*
  Set the value of zero to all entries of memory.
 */
void mem_clear()
{
  int x;
  for (x =0; x < mem_size; x++){
    memory[x] = 0;
  }
  last_placement_position = 0;
}

/*
 Allocate physical memory to size. This function should 
 only be called once near the beginning of your main function.
 */
void mem_init( unsigned int size )
{
  mem_size = size;
	memory = malloc( sizeof(unsigned int)*mem_size );
  mem_clear();
}

/*
 Deallocate physical memory. This function should 
 only be called once near the end of your main function.
 */
void mem_free()
{
	free( memory );
}

/*
 Use to debug your program.
 */
void mem_print()
{
  for(int j = 0; j< mem_size; j++){
    printf("mem cell: %d, time val: %u\n", j, memory[j]);
  } 
}

