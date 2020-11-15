#pragma once
#include "type.h"

extern int goUmode();
extern PROC proc[NPROC], *freeList, *readyQueue, *sleepList, *running;
extern int procsize;

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
  printf("new%d pgdir[2048]=%x, pgdir[2049]=%x\n", p->pid, p->pgdir[2048], p->pgdir[2049]); 
 
  // set kstack to resume to goUmode, then to Umode image at VA=0
  for (i=1; i<29; i++)  // all 28 cells = 0
    p->kstack[SSIZE-i] = 0;

  p->kstack[SSIZE-15] = (int)goUmode;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-28]);

  p->cpsr = (int *)0x10;
  
  r = load(filename, p);
  if (r == 0){
     printf("load %s failed\n", filename);
     getc();
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
  int index = 0;
  byte *ParentAddr01, *ParentAddr02, *ChildAddr01, *ChildAddr02;
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
  uPtable(child);
  ParentAddr01 = running->pgdir[2048] & 0xFFFF0000;
  ParentAddr02 = running->pgdir[2049] & 0xFFFF0000;
  ChildAddr01 = child->pgdir[2048] & 0xFFFF0000;
  ChildAddr02 = child->pgdir[2049] & 0xFFFF0000;
  memcpy(ChildAddr01, ParentAddr01, MB);
  memcpy(ChildAddr02, ParentAddr02, MB);
  for (i = 0; i <= 14; i++)
  {
    child->kstack[SSIZE - i] = running->kstack[SSIZE - i];
  }

  child->kstack[SSIZE-14] = 0; // Child PID == 0
  child->kstack[SSIZE-15] = (int)goUmode;
  child->ksp = &(child->kstack[SSIZE-28]);
  child->usp = running->usp;
  child->cpsr = running->cpsr;
  enqueue(&readyQueue, child);
  return child->pid;
}

int exec(char *cmdline)
{
  int i, upa1, upa2, usp;
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
// /bin/echo tokyo
  if (filename[0] != '/')
  {
    strcpy(file, "/bin/");
  }

  strcat(file, filename);

  upa1 = running->pgdir[2048] & 0xffff0000;
  upa2 = running->pgdir[2049] & 0xffff0000;

  if (loadprogram(file, running) == FALSE)
    return -1;

  usp = upa1 + 1*MB - 128;
  strcpy((char *)usp, kline);
  running->usp = (int *)VA(1*MB - 128);

  for (i = 2; i < 14; i++)
  {
    running->kstack[SSIZE - i] = 0;
  }

  running->kstack[SSIZE - 1] = (int)VA(0);

  return (int)running->usp;
}

int loadelf(char *file)
{
  kprintf("Error: loadelf() is not yet implemented.\n");
  return FALSE;
}

int loadprogram(char *filename, PROC *process)
{
  byte buffer[BLKSIZE];
  getblock(SUPERBLOCK, buffer);
  SUPER *superblock = (SUPER *)buffer;
  if (superblock->s_magic != EXT2FILESYSTEMTYPE) // I will not concede to magic numbers
  {
      return NULL;
  }

  return load(filename, process);
}