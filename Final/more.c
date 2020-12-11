#pragma once
#include "ucode.c"
#include "TypesAndDefs.h"
#define FILL 24

// print 24 lines at a time with more()

int main(int argc, char* argv[])
{

    char pathbuff[4096];
    char *filename = argv[1];
    bool isTerm;
    
    if (argc < 2)
    {
        printf("syntax: more [file]");
        exit(0);
    }

    gettty(tty);
    stat(tty, &myst);
    fstat(stdout, &st1);

    isTerm = (st1.st_ino == myst.st_ino);
    if (filename[0] != '/')
    {
        getcwd(pathbuff);
    }

    strcat(pathbuff, filename);

    FileDesc file = open(pathbuff, O_RDONLY);
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
        prints("             \r");
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

    getc();
}