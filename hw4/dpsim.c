#include "dpsim.h"

//this assignment was run on a virtualbox; typically there is a delay between picking up chopsticks, but the virtual machine only works without this delay.

static const unsigned int NUM_PHILOSOPHERS = 5;
static const unsigned int NUM_CHOPSTICKS = 5;

static int chopsticks[5];
static pthread_mutex_t mutex[5];
static pthread_t philosophers[5];;

void* th_main( void* th_main_args ) {

	int i;
	for (i = 0; i < NUM_CHOPSTICKS; i++){
		chopsticks[i] = -1;
	}
	int cnt[NUM_PHILOSOPHERS];
		int x;
		cnt[0] = 0;
		for (x = 1; x < NUM_PHILOSOPHERS; x++){
			cnt[x] = cnt[x - 1] + 1; 
		}

	int j;
	for (j = 0; j < NUM_PHILOSOPHERS; j++){
		if (pthread_create(&(philosophers[j]), NULL, (void *) th_phil, &cnt[j]) == 0){
			//printf("thread created for philosopher %d\n", j);
		}
		else{
			perror("philosopher thread creation ");
			exit(1);
		}
	}
	int loop = TRUE;
	while(loop){
		int chopsticks_temp[5];
		memcpy(chopsticks_temp, chopsticks, 5*sizeof(int));
		int diners = 0; //increments to 5 if no diners
		int nodeadlock = 0; //remains at 0 if there is deadlock
		//printf("nodeadlock value before for loop: %d\n", nodeadlock);

		int k;

		for (k = 0; k < NUM_CHOPSTICKS; k++){
			if (chopsticks_temp[k] == -1){
				nodeadlock++;
			}
			else if(chopsticks_temp[(k+1)%5] == chopsticks_temp[k]){
				nodeadlock++;
				diners++;
			}
		}
		//printf("nodeadlock value after for loop: %d\n", nodeadlock);
		if(nodeadlock == 0){
			printf("Deadlock condition: [%d, %d, %d, %d, %d].  Terminating.\n", chopsticks_temp[0],chopsticks_temp[1],chopsticks_temp[2],chopsticks_temp[3],chopsticks_temp[4]);
			loop = 0;
		}
		else if(diners>0){
			printf("Philosopher(s) ");
			int l;

			for (l = 0; l < NUM_CHOPSTICKS; l++){
				if ((chopsticks_temp[l] == chopsticks_temp[(l+1)%5]) && (chopsticks_temp[l] != -1)){
					//philosopher l is holding both chopsticks
					printf("%d ", l);
					if (diners > 1){
						printf(", ");
						diners--;
					}
				}
			}
			printf("are eating.\n");
		}
		delay(200000);
	}
	
	for (i = 0; i < NUM_PHILOSOPHERS; i++){
		if (pthread_kill(philosophers[i], 0) !=0 ) {
			perror("thread kill error ");
			// exit(1);
		}
	}
	pthread_exit(0);
}

void* th_phil( void* th_phil_args ) {
	unsigned int phil_id = ((unsigned int)th_phil_args)%5;
	//printf("th_phil is run for %d\n", phil_id);
	while (TRUE){
		delay(10000);
		eat(phil_id);
		//printf("th_phil loops for %d\n", phil_id);
	}
} // end th_phil function

void delay( long nanosec ) {

	struct timespec t_spec;

	t_spec.tv_sec = 0;
	t_spec.tv_nsec = nanosec;

	nanosleep( &t_spec, NULL );
} // end think function

void eat( int phil_id ) {
	int right = phil_id; //chopstick to philosopher's right
	int left = (phil_id + 1)%5; //chopstick to philosopher's left
	pthread_mutex_lock(&mutex[right]);
	chopsticks[right] = phil_id;

	pthread_mutex_lock(&mutex[left]);
	chopsticks[left] = phil_id;
	delay(500);
	//eating
	//printf("chopstick vals: 0:%d 1:%d 2:%d 3:%d 4:%d\n", chopsticks[0],chopsticks[1],chopsticks[2],chopsticks[3],chopsticks[4]);
	pthread_mutex_unlock(&mutex[left]);
	chopsticks[left] = -1;
	pthread_mutex_unlock(&mutex[right]);
	chopsticks[right] = -1;
} // end eat function
