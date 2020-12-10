#pragma once
#include "TypesAndDefs.h"
#include "ucode.c"

char catline[1024];

int main(int argc, char* argv[])
{
    // there's a file name
    if (argc > 1)
    {
        FileDesc file = open(argv[1], O_RDONLY);
        printfile(file);
        close(file);
    }
    else
    {
        printStdIn();
    }
}

void printStdIn()
{
    strcpy(catline, "");
    int readbytes = read(stdin, catline, 1024);
    while (readbytes > 0)
    {
        for(int i = 0; i < readbytes; i++)
        {
            write(stdout, catline, readbytes);
        }
        strcpy(catline, "");
        readbytes = read(stdin, catline, 1024);
    }
}

void printfile(FileDesc file)
{
    int readbytes = read(file, catline, 1024);
    while (readbytes > 0)
    {
        for (int i = 0; i < readbytes; i++)
        {
            mputc(catline[i]);
        }
        readbytes = read(file, catline, 1024);
    }
}