 /*
===============================================================================================================

Name               : major1.c
Author             : Timothy Stern, Ashley Torres, Ramiro Ortiz, and Read Ballew
Lecture Section    : CSCE 3600.001
Recitation Section : CSCE 3600.206
Date               : November 9th, 2018
Description        : The purpose of this program is to create a smaller version of the bash shell, with
                     two modes of input, interactive and batch. It will implement redirection, pipelining, 
                     a unique run schedule, built in commands, and support signal handling & terminal control.
===============================================================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include "parse.h"
int MAX_LINE = 512;

int main(int argc, char *argv[])
{
    //Setting foreground
    tcsetpgrp(fileno(stdin), getpgrp());

    //Terminal mode
    if(argc < 2)
    {
        printf("We're in terminal mode now!\n");
        int quitWhenImZero = 1; //For exit command
        char *custom = (char *)malloc(50*sizeof(char));
        
        FILE * fout;
        
        printf("Please enter your custom prompt: ");
        scanf("%s", custom);
        
        int z = 0, y = 0;

        fout = fopen("history.txt", "a");
        if (fout == NULL)
        {
            printf("Error opening file!\n");
            exit(1);
        }
    
        while(quitWhenImZero)
        {
            //Signal Handling
            
            struct sigaction act;
            act.sa_handler = SIG_IGN;
            for(z = 1; z < 65; z++)
            {
                if((z != 9) && (z != 19) && (z != 32) && (z != 33))
                {
                    assert(sigaction(z, &act, NULL) == 0);
                }
            }

            char input[MAX_LINE];
            if(y != 0)
            {
                printf("%s ", custom);    
            }
            int c=0;
            int i=0;

            for (i=0;( ((c = getchar()) != EOF) && (c != '\n')); i++)
            {
                input[i]=c;
            }
            //fprintf(fout, "%s", input);
            input[i] = '\0'; // adding the null terminator
            fprintf(fout, "%s \n", input);
            y++;


            if(parse(input) == 1)
            {
                //fprintf(fout, "%s \n", input);
                break;
            }
        }
        fclose(fout);
    }
    //batch mode
    else
    {
        printf("We're in batch mode now!\n");
        char input[MAX_LINE];
        int c, i;
        FILE *fp;
        fp = fopen(argv[1], "r");
        if(fp == NULL)
        {
            printf("File not able to be opened\n");
            exit(1);
        }
        
        while(!feof(fp))
        {
            for (i=0;( ((c = fgetc(fp)) != EOF) && (c != '\n')); i++)
            {
                    input[i]=c;
                }
                input[i] = '\0'; // adding the null terminator
                
                parse(input);
            }
    }

    return 0;
}