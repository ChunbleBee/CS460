#pragma once
#include "ucode.c"
#include "TypesAndDefs.h"
#define FILL 24

// print 24 lines at a time with more()

int main(int argc, char* argv[])
{

    char pathbuff[4096];
    char *filename = argv[1];
    bool fromTerm, toTerm;

    gettty(tty);
    stat(tty, &myst);
    fstat(stdin, &st0);
    fstat(stdout, &st1);

    fromTerm = (st0.st_ino == myst.st_ino);
    toTerm = (st1.st_ino == myst.st_ino);
    
    if (argc < 2 && fromTerm == false)
    {
        printf("syntax: more [file]");
        exit(1);
    }

    if (filename[0] != '/')
    {
        getcwd(pathbuff);
    }

    strcat(pathbuff, filename);

    FileDesc file;
    if (argc > 1)
    {
        file = open(pathbuff, O_RDONLY);
    }
    else
    {
        file = stdin;
    }
    
    if (file < 0)
    {
        printf("Failed to open file!");
        exit(1);
    }

    char linebuff[1024];
    int toPrint = FILL;
    int bytesread = read(file, linebuff, 1024);
    int i = 0;

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
            char c = getc();

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

    getc();
}