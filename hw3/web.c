
// -----------------------------------
// CSCI 340 - Operating Systems
// Fall 2016
// web.c header file
// 
// Homework 3
//
// -----------------------------------

#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include "web.h"

// ------------------------------------
// Function prototype that creates a socket and 
// then connects to the web address specified in 
// the web_t url field.
// 
//
// Arguments:	web_t pointer
//
// Return:     	OK on success, FAIL on error
//

int open_sock( web_t* web_struct ) {

	// -------------------------------------
	// Please do not modify this code
	// -------------------------------------

	int socket_fd;
	
	char ip[100];
	struct hostent *server;
    	struct sockaddr_in server_address;
     
	// create TCP socket
    	socket_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    	if (socket_fd < 0) { 
		if ( DEBUG ) printf( "Failed to create socket file descriptor\n" );
		web_struct->socket_fd = -1;
		return FAIL;
	}

	if ( DEBUG ) printf("URL = %s\n", web_struct->url );

    	server = gethostbyname( web_struct->url );

    	if ( server == NULL ) { 
		if ( DEBUG ) printf( "Unable to resolve IP.\n" );
		return FAIL;
	}

	if ( inet_ntop( AF_INET, (void *)server->h_addr_list[0], ip, sizeof(ip) ) == NULL ) {
		if ( DEBUG ) printf( "IP does not exist.\n" );
		return FAIL;
	}

	bzero( (char *) &server_address, sizeof(server_address) );

    	server_address.sin_family = AF_INET;
	inet_pton( AF_INET, ip, (void *)(&(server_address.sin_addr.s_addr) ) );
    	server_address.sin_port = htons( PORT_NUM );

    	if ( connect( socket_fd, (struct sockaddr *) &server_address, sizeof(server_address) ) == 0 ) {
		web_struct->socket_fd = socket_fd;
    		return OK;
    	} else {
		if ( DEBUG ) printf( "Connection refused.\n" );
    		return FAIL;
    	}

} // end open_sock function

// ------------------------------------
// Function prototype that closes an open network 
// socket 
// 
//
// Arguments:	web_t pointer
//
// Return:     	OK on success, FAIL on error
//
int close_sock( web_t* web_struct ) {

	return ( close( web_struct->socket_fd ) == OK ) ? OK : FAIL;

} // end close_sock function

// ------------------------------------
// Function prototype that parses webpage/URL 
// pointed at by client socket
// 
//
// Arguments:	web_t pointer
//
// Return:     	OK on success, FAIL on error
//
int parse( web_t* web_struct ) {

	int total_anchor_cnt = 0;

	char request[100];
	char response[500];

	bzero( request, sizeof( request ) );

	sprintf( request, "GET / HTTP/1.1\r\nHost:%s\r\n\r\n", web_struct->url );

	// if ( DEBUG ) printf("[STARTED] FD [%d] = %s\n", web_struct->socket_fd, request );

	write( web_struct->socket_fd, request, sizeof( request ) );

	while ( read( web_struct->socket_fd, response, sizeof( response ) ) != 0 )  {

		// --------------------------------------------
		// TODO:
		// Add code to parse response and count the total 
		// number of anchor tags;

		char* responsetokenizer;
		responsetokenizer = strtok(response, "<>");
		while (responsetokenizer!=NULL){
			int compare = strcmp(responsetokenizer, "/a");
			if (compare == 0){
				total_anchor_cnt++;
			}
			responsetokenizer = strtok(NULL, "<>");
		}
	}
	web_struct->anchor_cnt = total_anchor_cnt;

	if ( DEBUG ) printf("[FINISHED] FD [%d] = %s\n", web_struct->socket_fd, request );

	return 0;

} // end parse function
