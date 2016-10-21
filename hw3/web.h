// -----------------------------------
// CSCI 340 - Operating Systems
// Fall 2016
// web.h header file
// 
// Homework 3
//
// -----------------------------------

#define OK 0
#define FAIL -1
#define DEBUG 1
#define PORT_NUM 80

typedef struct {

	char* url; // must malloc
	int anchor_cnt;
	int socket_fd;
} web_t;

// ------------------------------------
// Function prototype that creates a network 
// socket and then connects to the web address 
// specified in the web_t url field.
// 
//
// Arguments:	web_t pointer
//
// Return:     	OK on success, FAIL on error
//

int open_sock( web_t* web_struct );


// ------------------------------------
// Function prototype that closes an open network 
// socket 
// 
//
// Arguments:	web_t pointer
//
// Return:     	OK on success, FAIL on error
//

int close_sock( web_t* web_struct );

// ------------------------------------
// Function prototype that parses webpage/URL 
// pointed at by client socket
// 
//
// Arguments:	web_t pointer
//
// Return:     	OK on success, FAIL on error
//

int parse( web_t* web_struct );
