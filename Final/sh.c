#pragma once

#include "TypesAndDefs.h"
#include "ucode.c"

// typedef enum __ASSOCIATIVITY__ {left, right} Associativity;

// typedef struct __OPNODE__
// {
//     char* opstring;
//     OpNode *left;
//     OpNode *right;
//     Associativity assoc;
// }
// OpNode;

int main(int argc, char* argv[])
{
    signal(2,1);

    int pid, status;
    char line[1024], temp[1024];
    printf("\n<------------- Taiya's Terribly Tacky Terminal ------------>\n");
    printf(  "    Tacked Together In Time To Trounce The Troq Teacher!\n\n");

    while (true)
    {
        printf(":>");
        gets(line);
        if (strcmp(line, "exit") == 0)
        {
            exit(0);
        }

        ProcID p = fork();
        if (p == CHILDPROCESS)
        {
            InterpretLine(line);
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

        *next = '\0';
        next++;

        FileDesc pipes[2];
        pipe(pipes);
        ProcID slave = fork();

        if (slave == CHILDPROCESS)
        {
            close(stdout);
            dup(pipes[1]);
            close(pipes[0]);
        }
        else
        {
            close(stdin);
            dup(pipes[0]);
            close(pipes[1]);
            wait(&slave);
            InterpretLine(next);
            exit(0);
        }
    }

    char* redirect = findNextDelim(cmd, "<>");

    if (*redirect != '\0')
    {
        printf("<--- IORedirect --->\n");
        if (*redirect == '<')
        {
            *redirect = '\0';
            FileDesc file = open((redirect + 1), O_RDONLY);
            close(stdin);
            dup(file);
        }

        if (*redirect == '>')
        {
            bool append = *(redirect + 1) == '>';
            *redirect = '\0';
            redirect++;

            int flags = (O_WRONLY | O_CREAT);
            if (append == true)
            {
                printf("<--- Append mode --->\n");
                redirect++;
                flags |= O_APPEND;
            }
            else
            {
                printf("<--- Truncate mode --->\n");
                flags |= O_TRUNC;
            }
            printf("<--- File to open: %s --->\n", redirect);
            FileDesc file = open(redirect, flags);
            close(stdout);
            dup(file);
        }
    }
    printf("<--- EXECUTE --->\n");
    int output = exec(cmd);
}