#include <stdio.h>
#include "dpsim.h"

/**************************************************

Main Function:
	int main( int argc, char** argv )

------------------------------------------------
In this function perform the following steps:
------------------------------------------------
1. Create the following variables:
	- main_thread (pthread_t)
	- status (join status value)
2. Create a main_thread 
	- If the return value != 0, then display an error message and 
	  immediately exit program with status value 1.
3. Join the main_thread
	- If the return value != 0, then display an error message and
	  immediately exit the program with status value 2.
4. Display join status value.
5. Exit program.

*/

int main( int argc, char** argv ) {

	pthread_t main_thread;
	int status;

	if((pthread_create(&main_thread, NULL, (void *) th_main, NULL)) != 0){
		perror("main thread creation error ");
		exit(1);
	}
	status = pthread_join(main_thread, NULL);
	if (status != 0) {
		perror("main thread join error ");
	}
	printf("Join status: %d\n", status);
	exit(0);

	return 0;

} // end main function