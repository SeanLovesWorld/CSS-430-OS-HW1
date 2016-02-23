/* Source.cpp - A small C++ project designed to make a program that mimics the behavior of the linux command chain 
				ps -A | grep argv | wc -l, using pipe, dup2, fork, and execlp.
				
	Program starts by creating variables to be used by process (file descriptor arrays, )

	Author: Jeremy Bobotek

*/


#include <stdio.h>
#include <iostream>
#include <unistd.h>    //for fork, pipe
#include <stdlib.h>    //for exit
#include <sys/wait.h>  //for wait
#include <string.h>

using namespace std;

// This program should emulate 'ps -A | grep argv | wc -l'
int main(int argc, char ** argv) {
	pid_t pid;
	int status;
	int pipefd1[2];
	int pipefd2[2];

	const int read = 0, write = 1; //used for stdin and stdout lines

	//check for correct number of arguments
	if (argc < 2) {
		perror("Error: Too Few Arguments");
		return 0;
	}
	if(pipe(pipefd1) < 0) {
		perror("Error: Failed Pipe1 Creation"); // create pipe1
	}
	if (pipe(pipefd2) < 0) {
		perror("Error: Failed Pipe2 Creation"); // create pipe2
	}


	//The Forkening: fork til great-grandchild, check for bad forks all the way down
	if ((pid = fork()) < 0) {// Create New Child Process
		perror("Error: Failed to Fork");
		return EXIT_FAILURE;
	}
	else if (pid == 0) {
		if ((pid = fork()) < 0) // Create New Grandchild Process
		{
			perror("Error: Failed to Fork");
			return EXIT_FAILURE;
		}
		else if (pid == 0)
		{
			if ((pid = fork()) < 0) { // Create New Great-Grandchild Process
				perror("Error: Failed to Fork");
				return EXIT_FAILURE;
			}
			else if (pid == 0) {
				//ggchild, only writes to pipe1
				close(pipefd2[read]);
				close(pipefd2[write]);
				close(pipefd1[read]);				//Close pipe1's read side
				dup2(pipefd1[write], write);		//Set up to write to pipe1
				execlp("ps", "ps", "-A", NULL);//Change process to execute process state check of All, 
											   //Note to self: exec statements must have last argument be NULL
			}
			else {
				//gchild, reads from pipe1, writes to pipe2

				close(pipefd1[write]);			//Close pipe1 write side
				dup2(pipefd1[read], read);		//Set up process to read from pipe1
				close(pipefd2[read]);			//Close pipe2 read side, as it only writes to pipe2
				dup2(pipefd2[write], write);	//Set up process to write to pipe2

				//Change process to execute a text grab/search of processes provided by pipe1 with the same name as the program in argv[1]
				//List of terms is then sent to pipe2
				execlp("grep", "grep", argv[1], NULL);
			}
		}
		else {
			//child - only reads from pipe2

			close(pipefd1[write]);
			close(pipefd1[read]);
			close(pipefd2[write]);
			dup2(pipefd2[read], read);

			execlp("wc", "wc", "-l", NULL);
		}
	}
	else {
		//close all pipes/file descriptors
		for (int x = pipefd1[0]; x <= pipefd2[1]; x++) {
			close(x);
		}

		//parent must wait for all children to finish
		wait(NULL);
	}
	return 0;
}