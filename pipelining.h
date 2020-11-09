#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

enum { READ, WRITE }; //read = 0, write = 1
//These are used to read/write to the pipes(fd1&fd2)

/*	This function should be called to use the functionality 
 *	of one pipe (i.e. "ls -l | head -3").
 *	This was made for Major1 Assignment by Ramiro Ortiz
 *	November 6th, 2018
 *	Any changes that need to be made or updates should be listed here:
 *	1.00 - Nov8 - No changes made, working prototype.
 *	1.01 - Nov11 - update to fit format from input(parse.h)
 *  			--Also added file redirection options
 *	1.02 - Nov12 - Finalized input branching
 *
 *
 */

void pipe1_call(char *left[], char *right[], int al_size, int ar_size)
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

	char *exec_lines_l[al_size+1];
	char *exec_lines_r[ar_size+1];
	char *input_file;
	char *output_file = (char *)malloc(50*sizeof(char));
	int i = 0, stat, redir = 0; 
	pid_t pid1;
	int ifp, ofp; 
	int fd1[2]; // fd1 for 1 pipes
	
	if (pipe(fd1) < 0)
	{
		perror("Pipe 1");
		exit(1); //error checking for pipe
	}

	//This is where the parsing starts for branching left
	if (al_size == 1)
	{	
	  exec_lines_l[0] = left[0];
		exec_lines_l[1] = '\0';
	}
	else 
	{
	for(i = 0; i < al_size; i++)
	 { 
		if (!(strcmp(left[i], "<")))
		{	
		  redir = 1; 
		  break; 
		}		
		else if (i == al_size-1)
		{  
		  
		  break; 
		}
		else {}
	 } 
	 for (i = 0; i < al_size; i++)
			{ 
				exec_lines_l[i] = left[i];
			}
			exec_lines_l[al_size] = NULL;
	 if (redir == 1)
	 {	
		
		for (i = 0; i < al_size-1; i++)
		{
		  exec_lines_l[i] = left[i]; 
			if (i == al_size-2)
			{ exec_lines_l[i] = NULL; break; }

	  }		 
		input_file = left[al_size-1];	
	 }
	}	

	//This is where the parsing starts for branching right
	if (ar_size == 1)
	{
		exec_lines_r[0] = right[0];
		exec_lines_r[1] = '\0';
	}
	else
{
	for(i = 0; i < ar_size; i++)
	 { 
		if (!(strcmp(right[i], ">")))
		{	
		  
		  redir = 2; 
		  break; 
		}		
		else if (i == ar_size-1)
		{  
		  break; 
		} 
		else {}
	 } 
	 for (i = 0; i < ar_size; i++)
			{ 
				exec_lines_r[i] = right[i];
			}
			exec_lines_r[ar_size] = NULL;
	 
	 if (redir == 2)
	 {	
		
		for (i = 0; i < ar_size-1; i++)
		{
		  exec_lines_r[i] = right[i]; 
			if (i == ar_size-2)
			{ exec_lines_r[i] = NULL; break; }

	  }		
		exec_lines_r[ar_size] = NULL; 
		input_file = right[ar_size-1];	
	 }
}
	
	pid1 = fork(); 
	if(pid1 < 0)
	  {
		perror("Fork");
		exit(3); //exits if child fails
	  }
	else if (pid1 == 0)
		  {
				tcsetpgrp(fileno(stdin), getpgid(getpid()));
        act.sa_handler = SIG_DFL;
        assert(sigaction(SIGINT, &act, NULL) == 0);

			char *command = left[0];  //Initializes the first string for command
			// These are standard for the execvp() call

			if (redir == 0)
			{
				int status2;
 			  dup2(fd1[WRITE], fileno(stdout)); //stores command 1 output
			  close(fd1[READ]); //mandatory pipe close on both ends 
			  close(fd1[WRITE]); //stores stdout info to be used by second command
				//Setting shell back to foreground
				setpgid(getpid(), getpid());
				signal(SIGTTOU, SIG_IGN);
				tcsetpgrp(fileno(stdin), getpid());
				waitpid(-1, &status2, WUNTRACED);
			  execvp(command, exec_lines_l); 	//exec command for command 1
				exit(1);
				
			}

			else if (redir == 1)
			{
				int status2;
				ifp = open(input_file, O_RDONLY); 
				dup2(ifp, 0); 
				close(ifp);
				
				dup2(fd1[WRITE], fileno(stdout)); //stores command 1 output
			  close(fd1[READ]); //mandatory pipe close on both ends 
			  close(fd1[WRITE]); //stores stdout info to be used by second command
				//Setting shell back to foreground
				setpgid(getpid(), getpid());
				signal(SIGTTOU, SIG_IGN);
				tcsetpgrp(fileno(stdin), getpid());
				waitpid(-1, &status2, WUNTRACED);
			  execvp(command, exec_lines_l); 	//exec command for command 1
				exit(1);
			}		
		  }

	else if (pid1 > 0)
		{
			int status2;
			char *commands = right[0]; 
		  waitpid(pid1, &stat, 0); //parent of child 2 waits
			
			if (redir == 0)
			{
				dup2(fd1[READ], fileno(stdin));  //reads input from command 1 
				close(fd1[READ]);	       //for input for command 2
				close(fd1[WRITE]);
				//Setting shell back to foreground
				setpgid(getpid(), getpid());
				signal(SIGTTOU, SIG_IGN);
				tcsetpgrp(fileno(stdin), getpid());
				waitpid(-1, &status2, WUNTRACED);
			  execvp(commands, exec_lines_r); 	//exec command for command 2
				exit(1);
			}
			else if (redir == 2)
			{
				dup2(fd1[READ], fileno(stdin));  //reads input from command 1 
				close(fd1[READ]);	       //for input for command 2
				close(fd1[WRITE]);
				ofp = open(output_file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR); 
				dup2(ofp, 1); 
				close(ofp);
				//Setting shell back to foreground
				setpgid(getpid(), getpid());
				signal(SIGTTOU, SIG_IGN);
				tcsetpgrp(fileno(stdin), getpid());
				waitpid(-1, &status2, WUNTRACED);
				execvp(commands, exec_lines_r); 	//exec command for command 2
				exit(1);			
				
			}
		
		}
		
	else {wait(NULL); }

	return; 

}


/*	This function should be called to use the functionality 
 *	of two pipe (i.e. "ls -l | head -3 | tail -1")
 *	This was made for Major1 Assignment by Ramiro Ortiz
 *	November 8th, 2018
 *	Any changes that need to be made or updates should be listed here:
 *	1.00 - Nov8 - No changes made, working prototype.
 *  1.01 - Nov11 - update to fit format from input(parse.h)
 * 				--  Also added file redirection options
 *	1.02 - Nov12 - Finalized input branching
 */

void pipe2_call(char *left[], char *middle[], char *right[], int al_size, int ar_size)
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

	char *exec_lines_l[al_size+1];
	char *exec_lines_r[ar_size+1];
	char *input_file;
	char *output_file = (char *)malloc(50*sizeof(char));
	int i = 0, stat, redir = 0, marker;
	int ifp, ofp; 
	pid_t pid1, pid2;   
	int fd1[2], fd2[2]; // fd1&2 for 2 pipes
	
	if (pipe(fd1) < 0)
	{
		perror("Pipe 1");
		exit(1);
	}		//Both of these if statements are error
		   // checks for the respective pipe	
	if (pipe(fd2) < 0)
	{
		perror("Pipe 2");
		exit(1);
	}

	//This is where the parsing starts for branching left
	if (al_size == 1)
	{	
	  	exec_lines_l[0] = left[0];
		exec_lines_l[1] = NULL;
	}
	
	else 
{
	for(i = 0; i < al_size; i++)
	 { 
		if (!(strcmp(left[i],"<")))
		{	
		  marker = i; 
		  redir = 1; 
		  break; 
		}		
		else if (i == al_size-1)
		{  
		  
		  break; 
		}
		else {}
	 } 
	 for (i = 0; i < al_size; i++)
			{ 
				exec_lines_l[i] = left[i];
			}
			exec_lines_l[i] = NULL;
	 if (redir == 1)
	 {	
		
		for (i = 0; i < al_size-1; i++)
		{
		  exec_lines_l[i] = left[i]; 
			if (i == al_size-2)
			{ exec_lines_l[i] = NULL; break; }

		}	
		
		input_file = left[al_size-1];	
	 }
}	


	//This is where the parsing starts for branching right
	if (ar_size == 1)
	{
	  	exec_lines_r[0] = right[0];
		exec_lines_r[1] = NULL;
	}
	else
{
	for(i = 0; i < ar_size; i++)
	 { 
		if (!(strcmp(right[i],">")))
		{	
		  if (!(strcmp(left[marker],"<")))
		  { redir = 3; }
		  else
		  { redir = 2; }
		  break; 
		}		
		else if (i == ar_size-1)
		{  
		  
		  break; 
		}
		else {}
	 } for (i = 0; i < ar_size; i++)
			{ 
				exec_lines_r[i] = right[i];
			}
			exec_lines_r[ar_size] = NULL;
	 
	 if (redir == 2)
	 {	
		
		for (i = 0; i < ar_size-1; i++)
		{
		  exec_lines_r[i] = right[i]; 
			if (i == ar_size-2)
			{ exec_lines_r[i] = NULL; break; }

	  }		
		exec_lines_r[ar_size] = NULL; 
		input_file = right[ar_size-1];	
	 }
}



	
	pid1 = fork();  //first fork
	if(pid1 == -1)
	{
		perror("Fork");
		exit(1); //exits if child fails
	}
	 	  
	else if (pid1 == 0)
	{
		tcsetpgrp(fileno(stdin), getpgid(getpid()));
		act.sa_handler = SIG_DFL;
		assert(sigaction(SIGINT, &act, NULL) == 0);

		pid2 = fork(); //this is where second fork starts!!
		
		if(pid2 == -1)
		{
			perror("Fork");
			exit(1); //exits if child fails
		}
		  
		  //This is the first child, this executes the original (1st)
		  // command to be input to the second command
		else if (pid2 == 0)
		  {
				tcsetpgrp(fileno(stdin), getpgid(getpid()));
        act.sa_handler = SIG_DFL;
        assert(sigaction(SIGINT, &act, NULL) == 0);
			  char *command = left[0];

			if (redir == 0)
			{
 			   dup2(fd1[WRITE], fileno(stdout)); //stores command 1 output
			   close(fd1[READ]); //mandatory pipe close on both ends 
			   close(fd1[WRITE]); //stores stdout info to be used by second command
   
			    execvp(command, exec_lines_l); 	//exec command for command 1
				exit(1);
				
			}

			else if ((redir == 1) || (redir == 3))
			{
				
				
				ifp = open(input_file, O_RDONLY); 
				dup2(ifp, 0); 
				close(ifp);
				
				dup2(fd1[WRITE], fileno(stdout)); //stores command 1 output
			   close(fd1[READ]); //mandatory pipe close on both ends 
			   close(fd1[WRITE]); //stores stdout info to be used by second command
   
			    execvp(command, exec_lines_l); 	//exec command for command 1
				exit(1);

			}												
		  }
		  	  
		//this is the second parent, waiting on the first command to 
		//finish then execute the second command to send to 3rd
		else if (pid2 > 0)
		{
				int status2;
		   char *command = middle[0];
		   char *exec_lines[] = {command, NULL}; 
		   
				dup2(fd1[READ], fileno(stdin)); //reads in command 1 output from pipe 1
				close(fd1[READ]);	       //for input for command 2
				close(fd1[WRITE]);
				dup2(fd2[WRITE], fileno(stdout)); //stores command 2 output into pipe 2
				close(fd2[READ]);
				close(fd2[WRITE]);
				//Setting shell back to foreground
				setpgid(getpid(), getpid());
				signal(SIGTTOU, SIG_IGN);
				tcsetpgrp(fileno(stdin), getpid());
				waitpid(-1, &status2, WUNTRACED);
				execvp(command, exec_lines);
				waitpid(pid2, &stat, 0); //parent of child 2 waits
				exit(1);			
		}			
	  }
			//This is the parent of the original fork, waiting for commands 1&2
			// to complete to input for the 3rd command
	else if (pid1 > 0)
	  {	
			int status2;

			waitpid(pid1, &stat, 0); //parent waits
		
			char *commands = right[0]; 
			
			if (redir == 0)
			{	
				dup2(fd1[READ], fileno(stdin));  //reads input from command 1 
				close(fd1[READ]);	       //for input for command 2
				close(fd1[WRITE]);
				//Setting shell back to foreground
				setpgid(getpid(), getpid());
				signal(SIGTTOU, SIG_IGN);
				tcsetpgrp(fileno(stdin), getpid());
				waitpid(-1, &status2, WUNTRACED);
			  execvp(commands, exec_lines_r); 	//exec command for command 2
				exit(1);
			}
			else if ((redir == 2) || (redir == 3))
			{
				
				dup2(fd1[READ], fileno(stdin));  //reads input from command 1 
				close(fd1[READ]);	       //for input for command 2
				close(fd1[WRITE]);
				
				ofp = open(output_file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR); 
				dup2(ofp, 1); 
				close(ofp);
				//Setting shell back to foreground
				setpgid(getpid(), getpid());
				signal(SIGTTOU, SIG_IGN);
				tcsetpgrp(fileno(stdin), getpid());
				waitpid(-1, &status2, WUNTRACED);
				execvp(commands, exec_lines_r); 	//exec command for command 2
				exit(1);
				
				
			}			
	  }
		
	else { wait(NULL); } 
	
	return; 
}

