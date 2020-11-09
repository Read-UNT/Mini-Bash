/*
Name: Ashley Torres
Date Completed: November 10, 2018
Description: This function is called when either a <, >, or both are found in a command line
ex: cat -n > output1.txt or  ls -l < input1.txt or cat -n < input1.txt > output1.txt.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

void redirection(char *left[], char *right[], int value)
{
	int z = 0;
	struct sigaction act;
	act.sa_handler = SIG_IGN;
	for(z = 1; z < 65; z++) //???
	{
		if((z != 9) && (z != 19) && (z != 32) && (z != 33))
		{
			assert(sigaction(z, &act, NULL) == 0);
		}
	}

	int ifp, ofp;
	pid_t pid1;

	pid1 = fork();
	if(pid1 == -1){ //Error
		perror("Fork");
		exit(3);
	}
	else if(pid1 == 0){ //This is the child and handles the input symbol that will read info from a file and execute the command
		if(value == 1){ //If the value passed is 1, then the code will preform the input execution
			tcsetpgrp(fileno(stdin), getpgid(getpid()));
            act.sa_handler = SIG_DFL;
            assert(sigaction(SIGINT, &act, NULL) == 0);

			char *input = right[0];

			ifp = open(input, O_RDONLY); //This opens the input file and has it only be read in
			dup2(ifp, 0);
			close(ifp);

			execvp(left[0], left); //This executes the command given with the information from the input file
			perror("execvp");

			exit(1);
		}
		else if(value == 2){
			tcsetpgrp(fileno(stdin), getpgid(getpid()));
			act.sa_handler = SIG_DFL;
			assert(sigaction(SIGINT, &act, NULL) == 0);

                        char *output = right[0];

                        ofp = open(output, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                        dup2(ofp, 1);
                        close(ofp);

                        execvp(left[0], left);
                        perror("execvp");

                        exit(1);
                }
	}
	else if(pid1 > 0){ //This is the parent function that handles when the output symbol is present
		int status2;
                //Setting shell back to foreground
                setpgid(getpid(), getpid());
                signal(SIGTTOU, SIG_IGN);
                tcsetpgrp(fileno(stdin), getpid());
                waitpid(-1, &status2, WUNTRACED);

		//wait((int *)0); //This makes the code wait for the child function to exit before exiting the parent function
		//exit(1);
	}
}

void two_redirect(char *left[], char *middle[], char *right[], int value)
{
        int z = 0;
        struct sigaction act;
        act.sa_handler = SIG_IGN;
        for(z = 1; z < 65; z++) //???
        {
                if((z != 9) && (z != 19) && (z != 32) && (z != 33))
                {
                        assert(sigaction(z, &act, NULL) == 0);
                }
        }

	int ifp, ofp;
	pid_t pid1;

	pid1 = fork();
	if(pid1 == -1){
		perror("Fork");
		exit(3);
	}
	else if(pid1 == 0){
		if(value == 3){
			char *input = middle[0];
			char *output = right[0];

			ifp = open(input, O_RDONLY);
			ofp = open(output, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			dup2(ifp, 0);
			dup2(ofp, 1);
			close(ifp);
			close(ofp);

			execvp(left[0], left);
			perror("execvp");
		}
	}
	else if(pid1 > 0){
		int status2;
		//Setting shell back to foreground
		setpgid(getpid(), getpid());
		signal(SIGTTOU, SIG_IGN);
		tcsetpgrp(fileno(stdin), getpid());
		waitpid(-1, &status2, WUNTRACED);
	}
}
