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
#include "vid.c"

extern int goUmode();
int loadelf(char *file, PROC *process);
int fork();
int exec(char *cmdline);

PROC proc[NPROC], *freeList, *readyQueue, *sleepList, *running;
int procsize = sizeof(PROC);
char *pname[NPROC]={"sun", "mercury", "venus", "earth", "mars", "jupiter",
                     "saturn","uranus","neptune"};

int kernel_init()
{
  int i, j; 
  PROC *p; char *cp;
  int *MTABLE, *mtable;
  int paddr;

  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->status = FREE;
    p->priority = 0;
    p->ppid = 0;
    strcpy(p->name, pname[i]);
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0;
  freeList = &proc[0];
  readyQueue = 0;
  sleepList = 0;

  running = dequeue(&freeList);
  running->status = READY;
  running->pgdir = (int *)0x400000; // P0 pgdir at 4MB
  
  printList(freeList);
}

int scheduler()
{
  char line[8];
  int pid; PROC *old=running;
  char *cp;
  kprintf("proc %d in scheduler\n", running->pid);
  if (running->status==READY)
     enqueue(&readyQueue, running);
  printQ(readyQueue);
  running = dequeue(&readyQueue);

  kprintf("next running = %d\n", running->pid);
  pid = running->pid;
  if (pid==1) color=WHITE;
  if (pid==2) color=GREEN;
  if (pid==3) color=CYAN;
  if (pid==4) color=YELLOW;
  if (pid==5) color=BLUE;
  if (pid==6) color=PURPLE;   
  if (pid==7) color=RED;
  // must switch to new running's pgdir; possibly need also flush TLB

  if (running != old){
    printf("switch to proc %d pgdir at %x ", running->pid, running->pgdir);
    printf("pgdir[2048] = %x\n", running->pgdir[2048]);
    switchPgdir((u32)running->pgdir);
  }
}

PROC *kfork(char *filename)
{
  int i, r; 
  int pentry, *ptable;
  byte * CurrentAddress, * ParentAddress;
  char *cp, *cq;
  char *addr;
  char line[8];
  int usize1, usize;
  int *ustacktop, *usp;
  u32 BA, Btop, Busp;

  PROC *p = dequeue(&freeList);
  if (p==0){
    kprintf("kfork failed\n");
    return (PROC *)0;
  }

  printf("kfork %s\n", filename);
  
  p->ppid = running->pid;
  p->parent = running;
  p->status = READY;
  p->priority = 1;

  // build p's pgtable 
  uPtable(p);
  printf("new%d pgdir[2048]=%x\n", p->pid, p->pgdir[2048]); 
 
  // set kstack to resume to goUmode, then to Umode image at VA=0
  for (i=1; i<29; i++)  // all 28 cells = 0
    p->kstack[SSIZE-i] = 0;

  p->kstack[SSIZE-15] = (int)goUmode;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-28]);

  p->cpsr = (int *)0x10;
  
  r = load(filename, p);
  if (r==0){
     printf("load %s failed\n", filename);
     return 0;
  }
  p->usp = (int *)VA(0x100000);
  p->kstack[SSIZE-1] = VA(0);

  p->sibling = running->child;
  running->child = p;

  enqueue(&readyQueue, p);

  kprintf("proc %d kforked a child %d: ", running->pid, p->pid); 
  printQ(readyQueue);

  return p;
}

int fork()
{
  int i;
  byte *ParentAddr, *ChildAddr;
  PROC *child = dequeue(&freeList);

  if (child == NULL)
  {
    kprintf("fork() failed\n");
    return NULL;
  }

  child->ppid = running->ppid;
  child->parent = running;
  child->status = READY;
  child->priority = 1;
  ParentAddr = running->pgdir[2048] & 0xFFFF00000;
  ChildAddr = child->pgdir[2048] & 0xFFFF0000;
  memcpy(ChildAddr, ParentAddr, 1*MB);

  for (i = 0; i <= 14; i++)
  {
    child->kstack[SSIZE - i] = running->kstack[SSIZE - i];
  }

  child->kstack[SSIZE-14] = 0;
  child->kstack[SSIZE-15] = (int)goUmode;
  child->ksp = &(child->kstack[SSIZE-28]);
  child->usp = running->usp;
  child->cpsr = running->cpsr;
  enqueue(&readyQueue, child);
  return child->pid;
}

int exec(char *cmdline)
{
  int i, upa, usp;
  char *cp, kline[128], file[32], filename[32];

  strcpy(kline, cmdline);
  cp = kline;
  i = 0;

  while (*cp != ' ')
  {
    filename[i] = *cp;
    i++;
    cp++;
  }

  if (filename[0] != '/')
  {
    strcpy(file, "/bin/bash");
  }

  strcat(file, filename);

  upa = running->pgdir[2048] & 0xffff0000;

  if (!loadelf(file, running))
    return -1;
  
  usp = upa + 1*MB - 128;
  strcpy((char *)usp, kline);
  running->usp = (int *)VA(1*MB - 128);

  for (i = 2; i < 14; i++)
  {
    running->kstack[SSIZE - i] = 0;
  }

  running->kstack[SSIZE - 1] = (int)VA(0);

  return (int)running->usp;
}

int loadelf(char *file, PROC *process)
{
  kprintf("Error: loadelf() is not yet implemented.\n");
  return FALSE;
}