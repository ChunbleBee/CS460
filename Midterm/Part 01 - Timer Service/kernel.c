//#include "type.h"

PROC proc[NPROC], *running, *freeList, *readyQueue;
int procsize = sizeof(PROC);

int body();
int kwait(int *status);
int kexit(int exitValue);
int kwakeup(int event);
int ksleep(int event);
PROC * kfork(int func, int priority);

#include "wait.c"
#include "timer.c"

int printSiblingList(char *listname, PROC *list)
{
    kprintf(listname);
    kprintf(": ");

    while (list != NULL)
    {
        kprintf("[ %d, %d ] -> ", list->pid, list->ppid);
        list = list->sibling;
    }
    kprintf("NULL\n");
}

int init()
{
    int i, j; 
    PROC *p;
    kprintf("kernel_init()\n");
    for (i=0; i<NPROC; i++)
    {
        p = &proc[i];
        p->pid = i;
        p->priority = 0;
        p->status = FREE;
        p->next = p + 1;
    }

    proc[NPROC-1].next = 0; // circular proc list
    freeList = &proc[0];
    readyQueue = 0;

    // creat P0 as initial process
    p = get_proc(&freeList);

    p->priority = 0;      // P0 priority = 0
    p->ppid = 0;
    p->child = 0;
    p->sibling = 0;
    p->parent = p;        // P0 parent = P0
    p->status = READY;

    running = p;
    kprintf("running = %d\n", running->pid);
    printList("freeList", freeList);
}

int scheduler()
{
    kprintf("proc %d in scheduler ", running->pid);
    if (running->status == READY)
        enqueue(&readyQueue, running);
    running = dequeue(&readyQueue);
    kprintf("next running = %d\n", running->pid);
}  

int body()
{
    char cmd[64];
    int pid;
    kprintf("proc %d resume to body()\n", running->pid);

    while(1)
    {
        pid = running->pid;
        if (pid==0) color=BLUE;
        if (pid==1) color=GREEN;
        if (pid==2) color=CYAN;
        if (pid==3) color=YELLOW;
        if (pid==4) color=RED;
        if (pid==5) color=GREEN;
        if (pid==6) color=WHITE;   
        if (pid==7) color=YELLOW;
        if (pid==8) color=CYAN;

        printList("freeList  ", freeList);
        printList("readyQueue", readyQueue);
        kprintf("Running Process ID: %d\nParent Process ID: %d\n",
            running->pid,
            running->ppid);
        printSiblingList("Children: ", running->child);

        kprintf("[ switch | fork | exit | sleep | wakeup | wait ] : ", running->pid);
        kgets(cmd);
        printf("\n");

        if (strcmp(cmd, "switch") == 0)
        {
            tswitch();
        }

        if (strcmp(cmd, "fork") == 0)
            kfork((int)body, 1);

        if (strcmp(cmd, "exit") == 0)
        {
            kprintf("Exit code: ");
            kgets(cmd);
            kexit(atoi(cmd));
        }

        if (strcmp(cmd, "sleep") == 0)
        {
            kprintf("Numeric Event to sleep on: ");
            kgets(cmd);
            ksleep(atoi(cmd));
        }

        if (strcmp(cmd, "wakeup") == 0)
        {
            kprintf("Numeric Event to wake for: ");
            kgets(cmd);
            kwakeup(atoi(cmd));
        }

        if (strcmp(cmd, "wait") == 0)
        {
            int exitCode  = 0;
            int procID = kwait(&exitCode);
            kprintf("Process #%d exited with code %d\n", exitCode, procID);
        }
    }
}
