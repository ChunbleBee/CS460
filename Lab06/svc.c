/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
#pragma once

#include "type.h"

extern PROC proc[], *running, *readyQueue, *sleepList, *freeList;
extern int printQ(PROC *p);
extern int printSleepList(PROC *p);
extern int printList(PROC *p);
extern int printSiblingList(PROC *p);
extern int tswitch();
extern PROC *kfork(char* process);

int ktswitch()
{
    tswitch();
}

int kgetpid()
{
    return running->pid;
}

int kgetppid()
{
    return running->ppid;
}

int kgetusp()
{
    return running->usp;
}

char *pstatus[]={"FREE   ","READY  ","SLEEP  ","BLOCK  ","ZOMBIE", " RUN  "};
int kps()
{
    kprintf("%s\n", pstatus[running->status]);
    printQ(readyQueue);
    printSleepList(sleepList);
    printList(freeList);
    printSiblingList(running);
}

int kchname(char *s)
{
    kstrcpy(running->name, s);
}

int kgetPA()
{
  return (running->pgdir[2048] & ~0xC32);
}

int svc_handler(int a, int b, int c, int d)
{
    int r;

    // printf("svc_handler: a=%d b=%d c=%d d=%d\n",a,b,c,d);

    switch(a) { // a is the call number

        case 0: r = kgetpid();          break;
        case 1: r = kgetppid();         break;
        case 2: r = kps();              break;
        case 3: r = kchname((char *)b); break;
        case 4: r = kfork((char *) b);  break;
        case 5: r = ktswitch();         break;
        case 6: r = kwakeup(b);         break;
        case 7: r = kexit(b);           break;
        case 8: r = kgetusp(b);         break;
        case 9: r = fork();             break;
        case 10: r = exec(b);           break;

        case 90: r = kgetc();           break;
        case 91: r = kputc(b);          break;
        case 92: r = kgetPA();          break;
        default: printf("invalid syscall %d\n", a);
    }

    return r; //return to goUmode in ts.s, which will replace r0 in Kstack with r
}

