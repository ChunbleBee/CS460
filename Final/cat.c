#pragma once
#include "TypesAndDefs.h"
#include "ucode.c"

char catline[1024];

int main(int argc, char* argv[])
{
    int bytesread;
    bool isTerm;

    gettty(tty);
    stat(tty, &myst);
    fstat(stdout, &st1);

    isTerm = (st1.st_ino == myst.st_ino);

    // there's a file
    if (isTerm == false || argc > 1)
    {
        FileDesc file;
        if (argc > 1)
        {
            file = open(argv[1], O_RDONLY);
        }
        else
        {
            file = stdin;
        }
        
        bytesread = read(file, catline, 1024);

        while(bytesread > 0)
        {
            if (isTerm)
            {
                prints(catline);
            }
            else
            {
                write(stdout, catline, bytesread);
            }

            bytesread = read(file, catline, 1024);
        }
    }
    else //Only Interactive mode. Hopefully.
    {
        char c;
        int i = 0;
        while (gets(catline) > 0)
        {
            prints(catline);
            printc('\n');
        }
    }
}