#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>

int main(void)
{
  //input from user
  char prompt[256];

  //dir variables
  char command[256];
  char dir[256];
  int val;
  char currDir[512];

  //parsing variables
  char *tok;
  char *promptArr[256];
  int tokenCount = 0;

  //2 dimensional piping array
  char *finalPrompt[256][256];

  //piping
  char *pipeArr[256];

  while (1)
  {
    printf("assignment1-2shell:");
    getcwd(currDir,512);

    //Printing current directory
    printf("~%s:",currDir);

    //asking user input
    bzero((char *) &prompt, sizeof(prompt)); //Clear buffer
    fgets(prompt,256,stdin);
    int prompt_size = strlen(prompt);
    prompt[prompt_size-1] = '\0'; //removes newline

    //parsing prompt for spaces-------------------------------------------------
    bzero((char *) &promptArr, sizeof(promptArr)); //Clear buffer
    int y = 0;
    tok = strtok(prompt, " ");
    while(tok != NULL)
    {
      promptArr[y++] = tok;
      //printf("%s\n",tok );
      tok = strtok(NULL, " ");
    }

    /*
    //parsing prompt for tabsig
    y = 0;
    tok = strtok(prompt, "\t");
    while(tok != NULL)
    {
      promptArr[y++] = tok;
      //printf("%s\n",tok );
      tok = strtok(NULL, "\t");
    }
    */
    //--------------------------------------------------------------------------
    int k = 0;
    int check = 0;
    while (check == 0)
    {
      if(promptArr[k] == NULL)
      {
        check = 1;
      }
      else
      {
        k++;
      }
    }
    //printf("%d\n",k );
    //printf("%s\n",promptArr[k-1] );

    //--------------------------------------------------------------------------
    //counting number of pipes
    int pipeCount = 0;
    int increment = 0;
    while(promptArr[increment] != NULL)
    {
      if(strcmp(promptArr[increment++],"|") == 0)
      {
        pipeCount++;
      }
    }
    //printf("%d\n",pipecount);

    //piping array--------------------------------------------------------------
    /*
    int pInc = 0;
    for (y = 0; y < k; y++)
    {
      if(strcmp(promptArr[y],"|") == 0)
      {
        if(promptArr[y+1] == NULL) //makes sure that next index is not NULL
        {
          printf("ABORTING PROGRAM\n");
          return 0;
        }
        y++;
      }
      pipeArr[pInc] = promptArr[y];
      //printf("%s\n",pipeArr[pInc]);
      pInc++;
    }
    */
    bzero((char *) &finalPrompt, sizeof(finalPrompt)); //Clear buffer
    int pInc = 0;
    int aInc = 0;
    for (y = 0; y < k; y++)
    {
      if(strcmp(promptArr[y],"|") == 0)
      {
        if(promptArr[y+1] == NULL) //makes sure that next index is not NULL
        {
          printf("ABORTING PROGRAM\n");
          return 0;
        }
        y++;
        pInc++;
        aInc = 0;
        tokenCount++;
      }
      finalPrompt[pInc][aInc] = promptArr[y];
      //printf("%s\n",finalPrompt[pInc][aInc]);
      aInc++;
    }
    *finalPrompt[pInc+1] = NULL;

    //--------------------------------------------------------------------------
    if(promptArr[0] == NULL) //when no command is entered
    {
      //printf("NO COMMAND\n");
    }

    //Changing Directory--------------------------------------------------------
    else if (strcmp(promptArr[0],"cd") == 0)
    {

      val = chdir(promptArr[1]);

      if(val == -1)
      {
        printf("Unknown Directory\n");
      }
    }

    //Exit the shell------------------------------------------------------------
    else if(strcmp(promptArr[0],"exit") == 0 || strcmp(promptArr[0],"Exit") == 0)
    {
      exit(EXIT_SUCCESS);
    }

    //Piping/forking section----------------------------------------------------
    else
    {
      //printf("%d\t%d\n",pInc,aInc);
      int pipes[2];
      pid_t pid;

      //var
      int in = 0;
      int first = 0;
      int second = 0;

      //printf("%s\n",finalPrompt[pInc][aInc-1]);
      while (finalPrompt[first][0] != NULL) //stops when reaching end of commands
      {
        pipe(pipes);
        pid = fork();
        if(pid < 0) //failure
        {
          fprintf(stderr, "FORK FAILURE\n");
          return 0;
        }
        else if (pid == 0) // child
        {
          dup2(in, 0);
          close(pipes[0]);
          //removes & from finalPrompt
          if(strcmp(finalPrompt[pInc][aInc-1],"&") == 0)
          {
            finalPrompt[pInc][aInc-1] = NULL;
          }
          if(finalPrompt[first + 1][0] != NULL) //makes sure next element is not null when dup
          {

            dup2(pipes[1],1);
            close(pipes[1]);
          }

          //executes finalPrompts first, second ... commands
          execvp(finalPrompt[first][0], finalPrompt[first]);
          //reaches only when child fails
          exit(EXIT_FAILURE);
        }
        else //parent
        {
          if(!strcmp(finalPrompt[pInc][aInc-1],"&") == 0) //background process
          {
            waitpid(pid,0,0);
          }

          close(pipes[1]);
          in = pipes[0];
          //increments for next command
          first++;
        }
      }
      signal(SIGCHLD, SIG_IGN); //reap children/ no zombie children
      //closing file descriptors
      close(pipes[0]);
      close(pipes[1]);
    }
  }
}
