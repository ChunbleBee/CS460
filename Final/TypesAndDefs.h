#pragma once
#define CHILDPROCESS 0
#define NULL ((void*) 0)
#define stdin 0
#define stdout 1
#define stderror 2

#define BUFFERLEN 4000
#define MAXPATH 4096

// From Linux kernel stdio.h
#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */

typedef enum __BOOL__ {false, true} bool;
typedef int ProcID;
typedef int FileDesc;