#pragma once
#include "ucode.c"
#include "TypesAndDefs.h"

main(int argc, char *argv[])
{
    for(int i = 1; i < argc; i++)
    {
        prints(argv[i]);
        printc(' ');
    }

    prints("\n");
    exit(0);
}