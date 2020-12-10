#pragma once
#include "TypesAndDefs.h"
#include "ucode.c"

char catline[1024];

int main(int argc, char* argv[])
{
    int readbytes;
    bool isTerm = false;

    gettty(tty);
    stat(tty, &myst);
    fstat(0, &st0);
    fstat(1, &st1);

    if (st0.st_ino == myst.st_ino && st1.st_ino == myst.st_ino)
    {
        // printf("===== we're in here =====\n");
        isTerm = true;
    }
    else
    {
        isTerm = false;
        // printf(";;;;; we didn't get in there ;;;;;");
    }
    

    // there's a file
    if (argc > 1)
    {
        FileDesc file = open(argv[1], O_RDONLY);
        readbytes = read(file, catline, 1024);
        while(readbytes > 0)
        {
            if (isTerm)
            {
                prints(catline);
            }
            else
            {
                write(file, catline, 1024);
            }

            readbytes = read(file, catline, 1024);
        }

        printf("\n");
    }
    else
    {
        int bytesread = read(stdin, catline, 1024);
        while(bytesread > 0)
        {
            if (isTerm)
            {
                printf("\n%s\n", catline);
            }
            else
            {
                write(stdout, catline, strlen(catline));
            }
            
            bytesread = read(stdin, catline, 1024);
        }
    }
}