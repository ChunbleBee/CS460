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
    fstat(0, &st0);
    fstat(1, &st1);

    isTerm = (st0.st_ino == myst.st_ino && st1.st_ino == myst.st_ino);

    // there's a file
    if (argc > 1)
    {
        FileDesc file = open(argv[1], O_RDONLY);
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

        printf("\n");
    }
    else
    {
        bytesread = read(stdin, catline, 1024);
        while(bytesread > 0)
        {
            if (isTerm)
            {
                printf("\n%s\n", catline);
            }
            else
            {
                write(stdout, catline, bytesread);
            }

            bytesread = read(stdin, catline, 1024);
        }
    }
}