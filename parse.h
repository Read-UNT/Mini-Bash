//Timothy Stern (timothystern@my.unt.edu)
//11043921
//Read Ballew (readballew@my.unt.edu)
//rlb0422 | 11202191
//10/22/2018

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include "redirection.h"
#include "pipelining.h"

int parse(char * input);

char * end_input;

int parse(char * input)
{
    //Signal Handling
    int z = 0;
    struct sigaction act;
    act.sa_handler = SIG_IGN;
    for(z = 1; z < 65; z++) 
    {
        if((z != 9) && (z != 19) && (z != 32) && (z != 33))
        {
            assert(sigaction(z, &act, NULL) == 0);
        }
    }

    char * commands[50][50]; //fits 25 command lines with 25 options in each 
    char * token; //temp input for individual string
    char * spaceTok; //we will parse the tok again before storing

    int cmd_count=0; //temp for command line
    int cmd_word=0; //temp for each input

    int quit = 0; //Quit flag for built in exit
    char *buffer; //Buffer for path
    char *workingDir; //Directory stuff for changeDir system call
    char *directory;

    token = strtok_r (input, ";", &end_input); //this will get every command line
    
    while (token != NULL) //as long as we are not at the end of the input
    {
        
        char * end_token;
        spaceTok = strtok_r (token, " ", &end_token); //parse the token for each command
	    cmd_word=0;
        while (spaceTok != NULL)
        {
            commands[cmd_count][cmd_word] = spaceTok;
            spaceTok = strtok_r(NULL, " ", &end_token); //getting next input
            cmd_word++; //augmenting input by 1
        }
        commands[cmd_count][cmd_word] = NULL;

        //_____________________
        //NESTED FOR LOOP
        //_____________________
        
        char * tempQueue[30]; //this stores all the commands between the operators
        int tempQueuePosition; //this stores the position of the TempQueue
        int tempCount; //this is used to iterate the array
        int prev_k = 0; //stores the previous position of the array
        int tempK; //temp variable for comparing
        int k; //position in the array
        int flag = 1; //flag for operator count

        for(k=0; k < cmd_word; k++)
        {
            tempCount=0;

            //inside here we need to check position by position to find out what we have then store that off
            if(!(strcmp(commands[cmd_count][k],">"))) 
            {
                char * output_redirect_left[25]; //this holds the left side of the output redirection
                char * output_redirect_right[25]; 

                while(tempCount != tempQueuePosition) //This while loop gets everything from the left we need already parsed by spaces
                {
                    output_redirect_left[tempCount] = tempQueue[tempCount]; //set all of the tokens up to the > char into the output redirect file
                    tempCount++;
                } 
                prev_k = k+1; //skipping the > char **DO NOT AUGMENT TILL NEW CMD FOUND**
                tempCount=0;
                int tempK=prev_k;
                while( (!(tempK == (cmd_word))) && ((strcmp(commands[cmd_count][tempK], "<"))) && ((strcmp(commands[cmd_count][tempK], ">"))) && ((strcmp(commands[cmd_count][tempK], "|"))) ) //this while loop gets everything to the left
                {
                    output_redirect_right[tempCount] = commands[cmd_count][tempK]; //this stores off the file name which should be the token right after the > char
                    tempK++;
                    tempCount++;
                }
                //clearing the tempQueue for more
                int counter;
                for(counter=0; counter <= tempQueuePosition; counter++)
                {
                    tempQueue[counter] = '\0';
                }
                tempQueuePosition = 0;
                flag = 0;
                //***NOTE: This is where the ouput redirection funciton should be called, the funciton will pass in two arrays and an int value
               redirection(output_redirect_left, output_redirect_right,2);
            }
            
            else if(!(strcmp(commands[cmd_count][k],"<")))
            {

                //NOTE: check for the < > option and pass that instead just to make others life easier


                char * input_redirect_left[25]; //this holds the left side of the inputredirection
                char * input_redirect_right[25]; //this holds the right side
                char * input_redirect_center[25];
                while(tempCount != tempQueuePosition) //This while loop gets everything from the left we need already parsed by spaces
                {
                    //printf("%s \n", tempQueue[0] );
                    input_redirect_left[tempCount] = tempQueue[tempCount]; //set all of the tokens up to the > char into the output redirect file
                    tempCount++;
                } 
                prev_k = k+1; //skipping the < char **DO NOT AUGMENT TILL NEW CMD FOUND**
                tempCount=0;
                tempK=prev_k;
                while( (!(tempK == (cmd_word)))) 
                {
                    if(!(strcmp(commands[cmd_count][tempK], ">")))
                    {
                        tempK++; //skipping the > operator
                        tempCount=0;
                        //If this is true we are in the in < func > out
                        while(!(tempK == (cmd_word)))
                        {
                            input_redirect_right[tempCount] = commands[cmd_count][tempK]; //this stores off the file name which should be the token right after the > char
                            tempK++;
                            tempCount++; 
                        }
                        two_redirect(input_redirect_left, input_redirect_center, input_redirect_right, 3);
                        break; 
                    }
                    //printf("Problem: %s\n", commands[cmd_count][tempK]);
                    input_redirect_center[tempCount] = commands[cmd_count][tempK]; //this stores off the file name which should be the token right after the > char
                    tempK++;
                    tempCount++;
                }

                redirection(input_redirect_left, input_redirect_center, 1);
                //clearing the tempQueue for more
                int counter;
                
                
                for(counter=0; counter <= tempQueuePosition; counter++)
                {
                    tempQueue[counter] = '\0';
                }
                tempQueuePosition = 0;
                flag = 0;
                
                
                //***NOTE: This is where the input redirection funciton should be called, the funciton will pass in two arrays and an int value
                //inputRedirect(input_redirect_right, input_redirect_left, 1);

            }

            else if(!(strcmp(commands[cmd_count][k],"|")))
            {
                //MUST CHECK for  | | becuase the input of 1 matters to 3 
                char * pipeline_one[25]; //this holds the left side of the inputredirection
                char * pipeline_two[25]; //this holds the right side
                char * pipeline_three[25];

                int one_count;
                int two_count;
                //int three_count;

                tempK = k;
                tempCount=0;
                while(tempCount != tempK) //This while loop gets everything from the left we need already parsed by spaces
                {
                    pipeline_one[tempCount] = commands[cmd_count][tempCount]; //set all of the tokens up to the > char into the output redirect file
                    tempCount++;
                } 
                one_count = (tempCount); //setting tempCounter equal to the one_count that is passed to the pipeline function
                prev_k = k+1; //skipping the | char **DO NOT AUGMENT TILL NEW CMD FOUND**
                tempCount=0;
                tempK=prev_k;
                while(!(tempK == (cmd_word))) 
                {
                    if(!(strcmp(commands[cmd_count][tempK], "|")))
                    {
                        two_count = tempCount;
                        tempK++; //skipping the | 
                        tempCount=0;
                        while(!(tempK == (cmd_word)))
                        {
                            pipeline_three[tempCount] = commands[cmd_count][tempK];
                            tempCount++;
                            tempK++;
                        }
                        //three_count = tempCount;
                        pipe2_call(pipeline_one, pipeline_two, pipeline_three, one_count, two_count);
                        break;
                    }
                    pipeline_two[tempCount] = commands[cmd_count][tempK]; //this stores off the file name which should be the token right after the > char
                    tempK++;
                    tempCount++;
                }

                pipe1_call(pipeline_one, pipeline_two, one_count, tempCount);                
                //clearing the tempQueue for more
                int counter;
                for(counter=0; counter <= tempQueuePosition; counter++)
                {
                    tempQueue[counter] = '\0';
                }
                tempQueuePosition = 0;
                flag = 0;
                
            }
            //if it is just a normal command 
            else if(tempK != cmd_word) //if nothing we build out tempQueue
            {
                tempQueue[tempQueuePosition] = commands[cmd_count][k];
                tempQueuePosition++;
            }
        }

        if (flag == 1) //if there are no special operators just call the normal exec funciton 
        {
            //built in commands check
            int a, size, tempflag=0;
            for(a = 0; a < tempQueuePosition; a++)
            {
                if(strcmp(tempQueue[a], "pwd") == 0 )
                {
                    tempflag = 1;
                    //prints current working directory
                    size = pathconf("/", _PC_PATH_MAX);
                    if ((buffer = (char *)malloc((size_t)size)) != NULL)
                    {
                        workingDir = getcwd(buffer, (size_t)size);
                        if(workingDir == NULL)
                        {
                            printf("Major1>  Failed to find working directory.\n");
                        }
                        else
                        {
                            printf("Major1>  %s \n", workingDir);
                        }
                    }
                }

                else if(strcmp(tempQueue[a], "cd") == 0)
                {
                    //two branches, cd with 0 arguments after and cd with 1 argument after
                    tempflag = 1;
                    if(tempQueuePosition == 1)
                    {
                        //for only cd without extra arguments
                        directory = "/home/";
                        if(chdir(directory) == -1)
                        {
                            printf("Major1>  Failed to change working directory to home.\n");
                        }
                    }
                    else
                    {
                        directory = tempQueue[a + 1];
                        if(chdir(directory) == -1)
                        {
                            printf("Major1>  Failed to change working directory to %s.\n", directory);
                        }
                    }
                }
                else if(strcmp(tempQueue[a], "exit") == 0)
                {
                    tempflag = 1;
                    quit = 1;
                }
                else if(strcmp(tempQueue[a], " exit") == 0)
                {
                    tempflag = 1;
                    quit = 1;
                }
                else if(strcmp(tempQueue[a], "exit ") == 0)
                {
                    tempflag = 1;
                    quit = 1;
                }
                
            }
            //regular command execution
            if((tempflag == 0) && (fork() == 0)) 
            {

                //setpgrp();
                tcsetpgrp(fileno(stdin), getpgid(getpid()));
                act.sa_handler = SIG_DFL;
                assert(sigaction(SIGINT, &act, NULL) == 0);

                //TIM --------------------------------------------------------------------------------------------- 
                execvp(tempQueue[0], tempQueue);
                perror("execvp");
                exit(1);
                
                //exit(1);
            }
            else
            {
                int status2;
                //Setting shell back to foreground
                setpgid(getpid(), getpid());
                signal(SIGTTOU, SIG_IGN);
                tcsetpgrp(fileno(stdin), getpid());
                waitpid(-1, &status2, WUNTRACED);
                //wait( (int *)0 );
            }
            //***NOTE: This is where the normal exec function should be called at the very end
        }

        //memset(tempQueue, 0 , sizeof(tempQueue));
        //clear temp queue
        int counter=0;
        for(counter=0; counter <= tempQueuePosition; counter++)
                {
                    tempQueue[counter] = '\0';
                }
                tempQueuePosition = 0;


        token = strtok_r (NULL, ";", &end_input);
        cmd_count++;
    }

    //BONUS -- File Output
        
  /*  int x, y;
    
    for (x=0;x < cmd_count; x++)
    {
        for(y=0; y < cmd_word; y++)
        {
            if(y != (cmd_word-1))
            {
            fprintf(fout, "%s ", commands[cmd_count][cmd_word]);
            }
            else
            {
            fprintf(fout, "%s", commands[cmd_count][cmd_word]);
            }      
        }
        fprintf(fout, ";");
    }
    */
    return quit;
}
