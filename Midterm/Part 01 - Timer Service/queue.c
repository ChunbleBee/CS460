/************************  queue.c file ************************
1. Write your C code for the functions specified below
2. Compile queue.c into .o file by
      arm-none-eabi-gcc -c -mcpu=arm926ej-s queue.c -o queue.o
3. Use it in the LINK step by
      arm-none-eabi-ld -T t.ld ts.o t.o   queue.o   -o t.elf
****************************************************************/

//#include "type.h"
extern PROC *freeList, *readyQueue, *running;

// write YOUR C cdoe for the following functions

PROC *get_proc(PROC **list)  // called as get_proc(&freeList)
{
  if (freeList == NULL)
  {
    return NULL;
  }
  PROC *newFreeProcess = freeList;
  freeList = newFreeProcess->sibling;
}
   
int put_proc(PROC **list, PROC *p)   // called as put_proc(&feeList, p)
{
  // enter PROC p to end of freeList
}

int enqueue(PROC **queue, PROC *p)   // called as enqueue(&readyQueue, p)
{
  // enter p into readyQueue by priority; FIFO if same priority
}

PROC *dequeue(PROC **queue)         // called as dequeue(&readyQueue)
{
  // remove and return FIRST entry from queue
}

int printList(char *listname, PROC *list)
{
    kprintf(listname);
    kprintf(": ");

    while (list != NULL)
    {
        kprintf("[ %d, %d ] -> ", list->pid, list->ppid);
        list = list->sibling;
    }
    kprintf(" NULL\n");
}

