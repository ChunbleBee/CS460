#pragma once
#include "TypesAndDefs.h"
#include "ucode.c"

char catline[1024];

int main(int argc, char* argv[])
{
    int bytesread;
    bool fromTerm, inTerm;

    gettty(tty);
    stat(tty, &myst);
    fstat(stdin, &st0);
    fstat(stdout, &st1);

    fromTerm = (st0.st_ino == myst.st_ino);
    inTerm = (st1.st_ino == myst.st_ino);

    // there's a file
    if (inTerm == false || argc > 1)
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

        while (bytesread > 0)
        {
            for (int i = 0; i < bytesread; i++)
            {
                write(stdout, &catline[i], 1);

                if (catline[i] == '\n' &&
                    i > 1 &&
                    catline[i-1] != '\n' &&
                    catline[i-1] != '\r' &&
                    inTerm == true)
                {
                    write(stdout, "\r", 1);
                }
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