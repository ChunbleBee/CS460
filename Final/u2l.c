#pragma once
#include "ucode.c"
#include "TypesAndDefs.h"

int main(int argc, char* argv[])
{
    FileDesc src, dest;

    if (argc == 1)
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
        
        dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC);

        char c;
        int readbytes = read(src, &c, 1);

        while (readbytes > 0)
        {
            if (c >= 'A' && c <= 'Z')
            {
                c = c - 'A' + 'a';
            }

            write(dest, &c, 1);
            readbytes = read(src, &c, 1);
        }
    }
    
    exit(0);
}