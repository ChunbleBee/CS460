#pragma once

#include "TypesAndDefs.h"
#include "ucode.c"

int main(int argc, char* argv[])
{
    signal(2, 1);

    int pid, status;
    char line[1024], temp[1024];
    printf("\n<------------- Taiya's Terribly Tacky Terminal ------------>\n");
    printf(  "    Tacked Together In Time To Trounce The Troq Teacher!\n\n");

    while (true)
    {
        printf("<3: ");
        gets(line);
        if (strcmp(line, "exit") == 0)
        {
            exit(0);
        }

        ProcID p = fork();
        if (p == CHILDPROCESS)
        {
            InterpretLine(line);
            exit(getpid());
        }
        else
        {
            wait(&p);
        }
        
    }
}

void InterpretLine(char* line)
{
    char* cmd = line;
    char* next = findNextDelim(line, "|");
    printf("<--- InterpretLine(), cmd = %s --->\n", cmd);

    if (*next != '\0')
    {
        printf("<--- pipe opened! --->\n");
        char *end = next - 1;
        while(*end == ' ')
        {
            *end = '\0';
            end--;
        }
        printf("cmd = %s\n", cmd);

        *next = '\0';
        next++;
        while(*next == ' ')
        {
            next++;
        }

        FileDesc pipes[2];
        pipe(pipes);
        ProcID childID = fork();
    
        if (childID == CHILDPROCESS)
        {
            printf("process ID: %d\n", getpid());
            close(stdout);
            dup2(pipes[stdout], stdout);
            close(pipes[stdin]);
        }
        else
        {
            close(stdin);
            dup2(pipes[stdin], stdin);
            close(pipes[stdout]);
            InterpretLine(next);
            wait(&childID);
            exit(0);
        }
    }


    char* redirect = findNextDelim(cmd, "<>");

    if (*redirect != '\0')
    {
        char c = *redirect;
        *redirect = '\0';
        redirect++;
        FileDesc file = -1;

        if (c == '<')
        {
            while(*redirect == ' ')
            {
                redirect++;
            }

            file = open(redirect, O_RDONLY);

            close(stdin);
        }
        else if (c == '>')
        {
            bool append = (*redirect == '>');
            u32 flags = (O_WRONLY | O_CREAT);

            if (append == true)
            {
                redirect++;
                flags = (O_WRONLY | O_APPEND | O_CREAT);
            }

            while(*redirect == ' ')
            {
                redirect++;
            }

            file = open(redirect, flags);
            if (file < 0)
            {
                creat(redirect);
                file = open(redirect, flags);
            }

            close(stdout);
        }

        if (file >= 0)
        {
            dup(file);
        }
    }
    
    //printf("<--- EXECUTE --->\n");
    int output = exec(cmd);
}