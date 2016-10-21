// ----------------------------------------------
// These are the only libraries that can be 
// used. Under no circumstances can additional 
// libraries be included
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "shell.h"


// -----------------------------------
// Main function 
// Arguments:	argc = number of arguments suppled by user
//				argv = array of argument values
//
//
int main( int argc, char** argv ) {

	int done = -1; //are we exiting yet?
	char line[60];

	while (done<0){
		command_t* cmd = malloc( sizeof( command_t ) );

		printf("SEASHELL$ ");
		gets(line);
		parse(line, cmd);

		if(cmd->path != NULL){
			if(is_builtin(cmd) == -1){ //not a builtin command
				if(execute(cmd) == 0){ //is executed, thus space is used
				}			
			}
			else{
				done = do_builtin(cmd);
			}
		}
		cleanup(cmd);

	}
	return 0;
		

} // end main function
