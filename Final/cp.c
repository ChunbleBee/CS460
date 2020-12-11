#pragma once
#include "ucode.c"
#include "TypesAndDefs.h"

int main(int argc, char *argv[])
{
    u8 buff[1024];

    if (argc > 2)
    {
        FileDesc src, dest;
        src = open(argv[1], O_RDONLY);
        dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC);

        int readbytes = read(src, buff, 1024);
        while(readbytes > 0)
        {
            write(dest, buff, readbytes);
            readbytes = read(src, buff, 1024);
        }
    }
    else
    {
        printf("Usage: cp [SRC] [DEST]\n");
    }

    exit(0);
}