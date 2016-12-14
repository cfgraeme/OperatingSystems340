#include "mem.h"


/* minimum and maximum duration of use for an allocated block of memory */
#define MIN_DURATION      3
#define MAX_DURATION     25

/* minimum and maximum allocation request size */
#define MIN_REQUEST_SIZE    3
#define MAX_REQUEST_SIZE  100

/*
  The main program will accept four paramemters on the command line.
  The first parameter is the memory size.  The second parameter is the
  number of times to repeat each experiment (ie. number of runs).  The
  third parameter is the duration of the each simulation run. The
  forth parameter is a random number seed. Here is an example command
  line:

  ./hw7 10000 100 2000 1234

  This means that your program should initialize physical memory to
  10,000 units, perform 100 runs with each run taking 2000 units of
  time, and the random number generator should be seeded (one time)
  with the value 1234.
*/

int main(int argc, char** argv)
{
  if (argc != 5){
    printf("Function needs 4 arguments, as unsigned integers: num_memory_units num_runs run_time rand_seed");
  }
  else{
    unsigned int units = atoi(argv[1]);
    unsigned int runs = atoi(argv[2]);
    unsigned int dur = atoi(argv[3]);
    unsigned int seed = atoi(argv[4]);
    if (units == 0 || runs == 0 || dur == 0){
      printf("memory units, number of runs, and duration of run must be nonzero.\n");
      printf("input was: units: %u, runs: %u, duration: %u, random generator seed: %u", units, runs, dur, seed);
    }

    //printf("units: %u runs: %u dur: %u seed: %u\n", units, runs, dur, seed);

    mem_init(units);
    srand((unsigned)seed);
    int sizerange = MAX_REQUEST_SIZE - MIN_REQUEST_SIZE;
    int durationrange = MAX_DURATION - MIN_DURATION;
    char* strategy;
    int i;
    //0:FIRST 1:NEXT 2:BEST
    for (i = 0; i < 3; i++){
      if (i == 0){strategy = "FIRST:";}
      if (i == 1){strategy = "NEXT: ";}
      if (i == 2){strategy = "BEST: ";}

      int t_fragments = 0;
      int t_probes = 0;
      int t_alloc_fails = 0;
      double durxrun_d = (double)dur*runs;
      double runs_d = (double)runs;

      int j;
      for(j = 0; j < runs; j++){ //go through "runs" number of runs
        mem_clear();
        int k;
        for(k = 0; k < dur; k++){ //each run goes for "dur" time units
          //allocate new process with random size and duration within ranges
          int s = rand()%sizerange + MIN_REQUEST_SIZE;
          int d = rand()%durationrange + MIN_DURATION;
          int pr = mem_allocate(i,s,d);
          //data collection for probes and failures
          if (pr == -1){t_alloc_fails = t_alloc_fails + 1;}
          else{t_probes = t_probes + pr;}
          //decrement time
          mem_single_time_unit_transpired();
          //printf("probes: %d, fails: %d\n", t_probes, t_alloc_fails);
        } //done with this run
        int fr = mem_fragment_count(MAX_REQUEST_SIZE);
        t_fragments = t_fragments + fr;
        //printf("method %s, run %d,  probes: %d, fails: %d, frags: %d\n", strategy, j+1, t_probes, t_alloc_fails, t_fragments);
      } //done with all runs for this strategy
      printf("%s ", strategy);
      printf("Average probes: %.4f, %d total in %.0f allocations \n", t_probes/durxrun_d, t_probes, durxrun_d);
      printf("%s ", strategy);
      printf("Average fails:  %.4f, %d total out of %.0f allocations\n", t_alloc_fails/durxrun_d, t_alloc_fails, durxrun_d);
      printf("%s ", strategy);
      printf("Average frags:  %.4f, %d total in %d runs\n", t_fragments/runs_d, t_fragments, runs);

    }
    mem_free();
  }
  return 0;
}
