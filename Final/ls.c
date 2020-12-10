#pragma once
#include "ucode.c"
#include "TypesAndDefs.h"

STAT current;
DIR  curdir;
char dirbuff[4096];

int main(int argc, char* argv[])
{
    printf("<----- ls ----->\n");
    if (argc > 1)
    {
        // assume second argument is a file or directory
        if (argv[1][0] != '/')
        {
            getcwd(dirbuff);
            printf("<----- new filename: %s ----->\n", dirbuff);
        }
        strcat(dirbuff, argv[1]);

        printf("<----- past recreate filename: %s ----->\n", dirbuff);
        stat(dirbuff, &current);
        
        printf("<----- past ls stat, isdir: %d ----->\n",
            S_ISDIR(current.st_mode));
        
        printf("dir: %d\nlink: %d\nreg: %d\n\n",
            S_ISDIR(current.st_mode),
            S_ISLNK(current.st_mode),
            S_ISREG(current.st_mode)
        );
    
        if (S_ISDIR(current.st_mode) == true)
        {
            lsDir(dirbuff);
        }
        else
        {
            printf("<----- call lsfile ----->\n");
            lsFile(dirbuff);
        }
    }
}

void lsFile(char * filename)
{
    char permissions[10] = "wrxwrxwrx";
    char type = '-';
    stat(filename, &current);
    u16 mode = current.st_mode;

    printf("<----- past lsfile stat ----->\n");
    if (S_ISDIR(mode)) type = 'd';
    if (S_ISLNK(mode)) type = 'l';

    for (int i = 0; i < 9; i++)
    {
        if (mode & (1 << i) == 0)
        {
            permissions[8 - i] = '-';
        }
    }

    printf("<----- past set permissions string ----->\n");
    char *file = filename, *next = findNextDelim(filename, "/");
    while(next != '\0')
    {
        next++;
        file = next;
        next = findNextDelim(next, "/");
    }
    printf("<----- file: %s ----->\n", file);

    u16 links = current.st_nlink;
    u16 owner = current.st_uid;
    u16 group = current.st_gid;
    long date = current.st_mtime; 
    u32 size = current.st_size;
    printf("<----- setup for printf ----->\n");
    
    printf("%c%s\t%d\t%d\t%d\t%d\t%s",
        type,
        permissions,
        links,
        owner,
        group,
        size,
        file);
    
    printf("<----- done ----->\n");
}

void lsDir(char * dirname)
{
    printf("<---------- LSDIR: NOT IMPLEMENTED YET!!! ---------->\n");
}