
// -----------------------------------
// CSCI 340 - Operating Systems
// Fall 2016
// server.h header file
// Homework 1
//
// -----------------------------------

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "server.h"

// ------------------------------------
// Function prototype that creates a socket and 
// then binds it to the specified port_number 
// for incoming client connections
// 
//
// Arguments:	port_number = port number the server 
//				socket will be bound to.
//
// Return:      Socket file descriptor (or -1 on failure)
//

int bind_port( unsigned int port_number ) {

	// -------------------------------------
	// NOTHING TODO HERE :)
	// -------------------------------------
	// Please do not modify

	int socket_fd;
	int set_option = 1;

    struct sockaddr_in server_address;
     
    socket_fd = socket( AF_INET, SOCK_STREAM, 0 );

    setsockopt( socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&set_option, sizeof( set_option ) );

    if (socket_fd < 0) return FAIL;

    bzero( (char *) &server_address, sizeof(server_address) );

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons( port_number );

    if ( bind( socket_fd, (struct sockaddr *) &server_address, sizeof(server_address) ) == 0 ) {

    	return socket_fd;

    } else {

    	return FAIL;

    }

} // end bind_port function

// ------------------------------------
// Function prototype that accepts a client
// socket connection
// 
//
// Arguments:	server file descriptor
//
// Return:      Termination status of client
//				( 0 = No Errors, -1 = Error )
//
int accept_client( int server_socket_fd ) {

	int exit_status = OK;

	int client_socket_fd = -1;

	socklen_t client_length; 

	struct sockaddr_in client_address;

	char request[512];

	client_length = sizeof( client_address );

    client_socket_fd = accept( server_socket_fd, (struct sockaddr *) &client_address, &client_length );
		
	// -------------------------------------
	// TODO:
	// -------------------------------------
	// Modify code to fork a child process
	// -------------------------------------
	
	
	
	if ( client_socket_fd >= 0 ) {
		pid_t child_pid = fork();
		if (child_pid == 0){
			//in child process
			bzero( request, 512 );
		
			read( client_socket_fd, request, 511 );
		
			if ( DEBUG ) printf("Here is the http message:\n%s\n\n", request );
		
			/*
			 ------------------------------------------------------
			 POST method key/value pairs are located in the entity body of the request message
			 ? - indicates the beginning of the key/value pairs
			 & - is used to delimit multiple key/value pairs 
			 = - is used to delimit key and value
			 
			 Example:
			 
			 first=brent&last=munsell
			 
			 two &'s indicated two key/value pairs (first=brent and last=munsell)
			 key = first, value = brent
			 key = last, value = munsell
			*/
	
			
			char method_type[5]; //should be "POST" or "GET "
			strncpy(method_type, request, 4);
			method_type[5] = '\0';
			char entity_body[400];
			//
			if (method_type[0]=='G'){
				char* keyvalues;
				keyvalues = strtok(request,"? "); //points to 'get'
				keyvalues = strtok(NULL, "? "); //points to '/'
				keyvalues = strtok(NULL, "? "); //points to the keyvalue pair block
				char* temp_keyvalues;
				temp_keyvalues = strtok(keyvalues, "=&");
				char formatted_keyvalues[350];
				strcpy(formatted_keyvalues,"<table cellpadding=5 cellspacing=5 border=1>");
				while (temp_keyvalues != NULL){
					
				
					strcat(formatted_keyvalues, "<tr><td><b>");
					strcat(formatted_keyvalues, temp_keyvalues);
					strcat(formatted_keyvalues, "</b></td><td>");
					temp_keyvalues = strtok(NULL, "=&");
					strcat(formatted_keyvalues, temp_keyvalues);
					strcat(formatted_keyvalues, "</td></tr>");
					temp_keyvalues = strtok(NULL, "=&");
					
				}
				strcat(formatted_keyvalues, "</table>");
				//have html table of key value pairs
			
				sprintf(entity_body, "<html><body><h1>GET Operation</h1>%s</body></html>", formatted_keyvalues);
			
			}
			else if (method_type[0]=='P'){
				char* keyvalues;
				keyvalues = strtok(request, "\n"); //POST / HTTP/1.1
				int compare = strncmp(keyvalues, "first", 5);
				while (compare!=0){
					keyvalues = strtok(NULL, "\n"); //Host:
					compare = strncmp(keyvalues, "first", 5);
				}
				
				char* temp_keyvalues;
				temp_keyvalues = strtok(keyvalues, "=&");
				char formatted_keyvalues[350];
				strcpy(formatted_keyvalues,"<table cellpadding=5 cellspacing=5 border=1>");
				while (temp_keyvalues != NULL){
					
				
					strcat(formatted_keyvalues, "<tr><td><b>");
					strcat(formatted_keyvalues, temp_keyvalues);
					strcat(formatted_keyvalues, "</b></td><td>");
					temp_keyvalues = strtok(NULL, "=&");
					strcat(formatted_keyvalues, temp_keyvalues);
					strcat(formatted_keyvalues, "</td></tr>");
					temp_keyvalues = strtok(NULL, "=&");
					
				}
				strcat(formatted_keyvalues, "</table>");
				//have html table of key value pairs
			
				sprintf(entity_body, "<html><body><h1>POST Operation</h1>%s</body></html>", formatted_keyvalues);
			}
	
			else{sprintf(entity_body, "error: method not found %s", request);}
		
		
		
			char response[512];
			sprintf( response, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n%s", (int)strlen( entity_body ), entity_body );
		
			if ( DEBUG ) printf( "%s\n", response );
		
    			write( client_socket_fd, response, strlen( response ) );
		
			close( client_socket_fd );
			exit_status = OK;
			exit(0);
		
		} //end child process
		else if (child_pid > 0){
			exit_status = OK;
		} //parent process
		else {
			printf("no forks");
			exit_status = FAIL;
		} //failure to fork
	} //end 'if ( client_socket_fd >= 0 )'
	else{exit_status = FAIL;}//client_socket_fd < 0

	close( client_socket_fd );

	if ( DEBUG ) printf("Exit status = %d\n", exit_status );
	
	return exit_status;
	
	
	
} // end accept_client function
