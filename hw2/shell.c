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
#include <string.h>
#include "shell.h"

// --------------------------------------------
// Currently only two builtin commands for this
// assignment exist, however in future, more could 
// be added to the builtin command array.
// --------------------------------------------
const char* valid_builtin_commands[] = {"cd", "exit", NULL};

// --------------------------------------------
// Implementation for each prototype below
// --------------------------------------------
void parse( char* line, command_t* p_cmd ){


	//is something entered in?
	int i = 0;
	char space = ' ';
	while (line[i] == space){ //remove leading space
		i++;
	}
	if(line[i] == NULL ){ //nothing entered in
		p_cmd->path = NULL;
		p_cmd->argc = 0;
		p_cmd->argv = NULL;
	}
	else{ //something entered in
		char* token = strtok(line, " ");
		char cmd[sizeof(token)];
		strcpy(cmd, token);
		int count = 0;
		p_cmd->argv = malloc(64 * sizeof(char*));

		while (token != NULL){ //get tokens to populate arguments
			p_cmd->argv[count] = token;
			count = count + 1;
			token = strtok(NULL, " ");
		}
		p_cmd->argv[ count + 1 ] = NULL; //last element should be null

		//is cmd in valid_builtin_commands[]?
		int j; //index
		int builtin = -1; //boolean 
		while(builtin == -1 && j<2){ 
			if(strcmp(cmd, valid_builtin_commands[j]) == 0){
				builtin = 0;
			}
			j++;	
		}

		//build the command structures
		if(builtin == 0){ //is a builtin command
			p_cmd->path = cmd; //path is command
			p_cmd->argc = count;
		}
		else if(find_fullpath(cmd, p_cmd) == 0){ //not a valid command
			p_cmd->path = cmd;
			p_cmd->argc = -1;
			p_cmd->argv = NULL;
		}
		else{ 
			find_fullpath(cmd, p_cmd);  //populate path
			p_cmd->argc = count;		// populate argument count
		}
	}	
}



int execute( command_t* p_cmd ){
	int executed = -1;
	if ( p_cmd->argc > 0){ //valid command?
		if ( fork() == 0 ) { //fork the process				
			execv( p_cmd->path, p_cmd->argv );		
			exit(0);
		}
		wait(NULL);
		executed = 0;
	}
	else if (p_cmd->argc == -1){
		printf("%s is not a valid command!! try again.\n", p_cmd->path);
	}
	else{ //no args, do nothing
	}
	return executed;
}

int find_fullpath( char* command_name, command_t* p_cmd ){

	struct stat buff;
	int exists = 0;

	char thepath[300]; //this works but char* thepath = getenv("PATH") doesn't! hmm..
	strcpy(thepath, getenv("PATH"));
	char* pathtoken = strtok(thepath, ":");
	char filedir[70];
	
	int found = 0; //boolean for if function has been found
	while (pathtoken != NULL && found == 0){
		strcpy(filedir, pathtoken);
		strcat(filedir, "/");
		strcat(filedir, command_name);
		exists = stat(filedir, &buff);
		if (exists == 0){
			p_cmd->path = strdup(filedir);
			found = 1;
		}
		pathtoken = strtok(NULL, ":");
	}
	return found;
}

int is_builtin( command_t* p_cmd ){
	int j = 0; //index
	int boole = -1; //boolean
	if(p_cmd->path != NULL){ //there is a command
		while(boole == -1 && j<2){ 
			if(strcmp(p_cmd->path, valid_builtin_commands[j]) == 0){
				boole = 0;
			}
			j++;	
		}
	}
	return boole;
}

int do_builtin( command_t* p_cmd ){
	int exit = -1; //is the command to exit?
	if(strcmp(p_cmd->path, valid_builtin_commands[1]) == 0){
		exit = 0;
	}
	else if(strcmp(p_cmd->path, valid_builtin_commands[0]) == 0){
 		struct stat buff;
		int exists;
		exists = stat( p_cmd->argv[1], &buff );
		if (strcmp(p_cmd->argv[1], "../") == 0){
			chdir(p_cmd->argv[1]);
		}
		else if ( exists == 0 && ( S_IFDIR & buff.st_mode ) ) {
			chdir(p_cmd->argv[1]);
		} 
		else if ( exists == 0 && ( S_IFREG & buff.st_mode ) ) {
			printf("%s points to a file not a directory\n", p_cmd->argv[1]);
		} 
		else {
			printf("%s is not a directory\n", p_cmd->argv[1]);	
		}
	}
	return exit;
}



void cleanup( command_t* p_cmd ){
	free(p_cmd);
	/*int i;

	for(i = 0; i < (p_cmd->argc); i++){
		free(p_cmd->argv[i]);
	}

	free(p_cmd->argv);

	p_cmd->argv = NULL;
	p_cmd->argc = 0;
	p_cmd->path = NULL;*/
}