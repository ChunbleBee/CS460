#pragma once
#include "ucode.c"
#include "TypesAndDefs.h"

int main(int argc, char* argv[])
{
    char buffer[1024];
    FileDesc file;
    bool fromTerm, inTerm;

    gettty(tty);
    stat(tty, &myst);
    fstat(stdin, &st0);
    fstat(stdout, &st1);

    fromTerm = (st0.st_ino == myst.st_ino);
    inTerm = (st1.st_ino == myst.st_ino);

    if (argc < 2 && inTerm == true)
    {
        printf("Usage: grep PATTERNS [FILE]...");
        exit(0);
    }

    char* pattern = argv[1];

    if (argc > 2)
    {
        close(stdin);
        file = open(argv[2], O_RDONLY);
        dup(file);
    }
    else
    {
        file = stdin;
    }

    if (argc > 2 || inTerm == false)
    {
        int i = 0;
        char c;

        while(read(file, &c, 1) > 0)
        {
            if(i < 1024 && c != '\n')
            {
                buffer[i] = c;
                i++;
            }
            else
            {
                buffer[i] = '\0';
                char *ch = strstr(buffer, pattern);

                if (ch > 0)
                {
                    printf("%s\n", buffer);
                }

                i = 0;
            }
        }
    }
    else
    {
        while (gets(buffer) > 0)
        {
            char *ch = strstr(buffer, pattern);
            if (ch)
            {
                prints(buffer);
                printc('\n');
            }
        }
    }
    

    exit(0);
}