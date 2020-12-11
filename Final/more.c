#pragma once
#include "ucode.c"
#include "TypesAndDefs.h"
#define FILL 24

// print 24 lines at a time with more()

int main(int argc, char* argv[])
{
    STAT sin, sout, myterm;
    FileDesc stdio = stdin;
    char pathbuff[4096];
    char *filename = argv[1];
    bool fromTerm, toTerm;

    gettty(tty);
    stat(tty, &myterm);
    fstat(stdin, &sin);
    fstat(stdout, &sout);

    fromTerm = (sin.st_ino == myterm.st_ino);
    toTerm = (sout.st_ino == myterm.st_ino);

    if (toTerm == true && fromTerm == false)
    {
        printf("line: We're in here! tty: %s\n", tty);
        stdio = open(tty, O_RDONLY);
    }

    int d = 0;
    printf("line: %d\n\n", ++d);
    
    if (argc < 2 && fromTerm == true)
    {
        printf("syntax: more [file]");
        exit(1);
    }
    printf("line: %d\n", ++d);

    if (fromTerm == true)
    {
        if (filename[0] != '/')
        {
            getcwd(pathbuff);
        }
        
        strcat(pathbuff, filename);
    }

    printf("line: %d\n", ++d);

    FileDesc file;
    if (argc > 1)
    {
        file = open(pathbuff, O_RDONLY);
    }
    else
    {
        file = stdin;
    }
    printf("line: %d\n", ++d);
    
    if (file < 0)
    {
        printf("Failed to open file!");
        exit(1);
    }
    printf("line: %d\n", ++d);

    char linebuff[1024];
    int toPrint = FILL;
    int bytesread = read(file, linebuff, 1024);
    int i = 0;
    printf("line: %d\n", ++d);

    pause(20);
    while(bytesread > 0)
    {
        while(toPrint > 0 && bytesread > 0)
        {
            for (; i < bytesread && toPrint > 0; i++)
            {
                char next = linebuff[i];
                mputc(next);
                if (next == '\n')
                {
                    toPrint--;
                }
            }

            if (toPrint > 0)
            {
                i = 0;
                bytesread = read(file, linebuff, 1024);
            }
        }

        if (toTerm == true)
        {
            prints("  <-- More -->\r");
            char c;
            read(stdio, &c, 1);

            if (c == '\r')
            {
                toPrint = 1;
            }
            else if (c == ' ' || c == 'a' || c == 'd')
            {
                toPrint = FILL;
            }

            // clear the more print
            prints("                      \r"); 
        }
    }
    printf("line: %d\n", ++d);

    getc();
}