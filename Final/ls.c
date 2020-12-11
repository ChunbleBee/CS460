#pragma once
#include "ucode.c"
#include "TypesAndDefs.h"

int main(int argc, char* argv[])
{
    STAT current;
    char dirnamebuff[4096];

    printf("<----- ls ----->\n");
    if (argc > 1)
    {
        // assume second argument is a file or directory
        if (argv[1][0] != '/')
        {
            getcwd(dirnamebuff);
        }

        strcat(dirnamebuff, argv[1]);
        stat(dirnamebuff, &current);
    
        if (S_ISDIR(current.st_mode) == true)
        {
            lsDir(dirnamebuff);
        }
        else
        {
            lsFile(dirnamebuff);
        }
    }
    else
    {
        getcwd(dirnamebuff);
        lsDir(dirnamebuff);
    }
}

void lsFile(char* filename)
{
    char permissions[10] = "wrxwrxwrx";
    char type = '-';
    char temp[256];
    STAT current;

    //printf("file to check: %s", filename);
    stat(filename, &current);
    u16 mode = current.st_mode;

    if (S_ISDIR(mode)) type = 'd';
    if (S_ISLNK(mode)) type = 'l';

    for (int i = 0; i < 9; i++)
    {
        if (mode & (1 << i) == 0)
        {
            permissions[8 - i] = '-';
        }
    }

    char *file = filename, *next = findNextDelim(filename, "/");

    while(*next != '\0')
    {
        next++;
        file = next;
        next = findNextDelim(next, "/");
    }
    
    printf(
        "%c%s\t%d\t%d\t%d\t%d\t%s",
        type,
        permissions,
        current.st_nlink,
        current.st_uid,
        current.st_gid,
        current.st_size,
        file
    );

    if (type == 'l')
    {
        readlink(filename, temp);
        printf("->%s", temp);
    }

    mputc('\n');
}

void lsDir(char * dirname)
{
    char dirnamebuff[4096];
    u8 direntbuff[1024];
    FileDesc directory = open(dirname, O_RDONLY);
    DIR *dirent;
    u8 *dirptr;

    int bytesread = read(directory, direntbuff, 1024);
    while(bytesread > 0)
    {
        dirent = (DIR *)direntbuff;
        dirptr = (u8 *)direntbuff;

        while (dirptr < direntbuff + 1024)
        {
            strcpy(dirnamebuff, dirname);

            if (dirnamebuff[strlen(dirname) - 1] != '/')
            {
                strcat(dirnamebuff, "/");
            }

            strncat(dirnamebuff, dirent->name, dirent->name_len);
            lsFile(dirnamebuff);

            dirptr += dirent->rec_len;
            dirent = (DIR *)dirptr;
        }

        bytesread = read(directory, direntbuff, 1024);
    }
}