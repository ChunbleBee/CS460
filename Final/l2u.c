#pragma once
#include "ucode.c"
#include "TypesAndDefs.h"

int main(int argc, char* argv[])
{
    FileDesc src, dest;
    bool pipeIn, pipeOut;

    gettty(tty);
    stat(tty, &myst);
    fstat(stdin, &st0);
    fstat(stdout, &st1);

    pipeIn = (st0.st_ino == myst.st_ino);
    pipeOut = (st1.st_ino == myst.st_ino);

    if (argc == 1 && pipeOut == false && pipeIn == false)
    {
        printf("Usage: l2u [SRC] [DEST]\n");
    }
    else
    {
        if (argc > 2)
        {
            src = open(argv[1], O_RDONLY);
        }
        else
        {
            src = stdin;
        }
        
        if (pipeOut == false)
        {
            dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC);
        }
        else
        {
            dest = stdout;
        }
        
        char c;
        int readbytes = read(src, &c, 1);

        while (readbytes > 0)
        {
            if (c >= 'a' && c <= 'z')
            {
                c = c - 'a' + 'A';
            }

            write(dest, &c, 1);
            readbytes = read(src, &c, 1);
        }
    }
    
    exit(0);
}