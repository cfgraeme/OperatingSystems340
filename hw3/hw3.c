
#include <stdio.h>	// IO functions
#include <stdlib.h> 	// std lib function
#include <unistd.h>	// close function
#include <pthread.h>	// creating, running, joining threads
#include <string.h>	// string processing 
#include "web.h"	// our code :)



/*

HW3: CSCI 340 Operating Systems

Developing a client that creates a thread for each
URL specified in the url.txt file. In each thread: 
1) a TCP/IP socket will be established, 2) read all
the characters in the webpage, and 3) count the number 
of anchor tags.

After all threads have been joined, the number of anchor 
tags will be written to a file named "url_a_num.txt".

*/

// ---------------------------------------------
// Global variable shared by all threads
// This is not thread safe.
web_t* web_array;


// ---------------------------------------------
// Function prototypes to read and write url files
int read_url_file( char* file_path, web_t* web_array );
int write_url_file( char* file_path, web_t* web_array, int num_urls );
void th_run( int* i );

int main( int argc, char *argv[] ) {

	int i = 0;

	// malloc web_t array with 100 elements
	web_array = malloc( sizeof( web_t )*100 );

	// -------------------------------------------------
	// Verify the correct number of arguments are provided
	// when the application is executed.

	if ( argc == 2 ) { 

		int num_urls = read_url_file( argv[1], web_array );


		if ( DEBUG )  { 

			printf("Number of urls in text file = %d\n", num_urls);

			for ( i=0; i<num_urls; i++ ) {

				printf("URL=%s\n", web_array[i].url ); 
				printf("CNT=%d\n", web_array[i].anchor_cnt );
				printf("FD=%d\n", web_array[i].socket_fd );
				
			}
		}
		
		pthread_t thread_array[num_urls];

		int cnt[num_urls];
		int x;
		cnt[0] = 0;
		for (x = 1; x < num_urls; x++){
			cnt[x] = cnt[x - 1] + 1; 
		}

		int i = 0;
		while (i < num_urls){

			if (pthread_create(&thread_array[i], NULL, (void *) th_run, &cnt[i]) == 0){
			}
			else {
				perror("thread creation: "), exit(1);
			}
			i++;
		}

		int j = 0;
		while (j < num_urls){
			if (pthread_join(thread_array[j], NULL) == 0) {
			}
			else{
				perror("thread join: "),exit(1);
			}
			j++;
		}

		write_url_file( "url_a_num.txt", web_array, num_urls );

	} else {
		printf( "URL file is not specified!\n" );
		printf( "Example usage: %s %s\n", argv[0], "url.txt" );
	}

	return OK;
     
} // end main function


void th_run( int* i ) {

	// -------------------------------------
	// Please do not modify this code
	// -------------------------------------

	if ( open_sock( &web_array[ (*i) ] ) == OK ) {

		parse( &web_array[ (*i) ] );
		close_sock( &web_array[ (*i) ] );

	} else printf( "[%s] network connection failed\n", web_array[ (*i) ].url );

	pthread_exit( NULL );

} // end th_run function


int write_url_file( char* file_path, web_t* web_array, int num_urls ) {

	int file_write = 0;

	FILE * fp;
	fp = fopen(file_path, "w");
	
	if (fp != NULL)
	{
		int i;
		for (i = 0; i < num_urls; i++){
			fprintf(fp, "%s, %d\n", web_array[i].url, web_array[i].anchor_cnt);
		}
	}
	else { 
		file_write = FAIL;
	}

	fclose(fp);
	
	return file_write;

} // end write_url_file function




int read_url_file( char* file_path, web_t* web_array ) {

	// -------------------------------------
	// Please do not modify this code
	// -------------------------------------

	int url_cnt = 0;

	FILE* fhnd;

	fhnd = fopen( file_path, "r" );

	char line[50];

	if ( fhnd != NULL ) {

    	while ( fgets( line, sizeof( line ), fhnd ) != NULL ) {
			line[strlen(line)-1] = '\0';
			web_array[url_cnt].url = malloc(sizeof(char)*100);
			sprintf( web_array[url_cnt].url, "www.%s", line );
			web_array[url_cnt].anchor_cnt = 0;
			web_array[url_cnt++].socket_fd = -1;
    	}

	} else url_cnt = FAIL;

	fclose( fhnd );

	return url_cnt;

} // end read_url_file function

