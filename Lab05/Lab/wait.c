//#include "type.h"
extern PROC *freeList, *readyQueue, *running, proc[NPROC];

/************ wait.c file ****************/
// PROC *kfork(int func, int priority)
// {
//     int i;
//     PROC *p = get_proc(&freeList);;

//     if (p == NULL)
//     {
//         printf("no more PROC, kfork failed\n");
//         return 0;
//     }

//     p->status       = READY;
//     p->priority     = priority;
//     p->ppid         = running->pid;
//     p->parent       = running;
//     p->sibling      = running->child;
//     running->child  = p;

//     // set kstack to resume to body
//     // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
//     //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
//     for (i=1; i<15; i++)
//         p->kstack[SSIZE-i] = 0;

//     p->kstack[SSIZE-1]  = (int)func;  // in dec reg=address ORDER !!!
//     p->ksp              = &(p->kstack[SSIZE-14]);

//     enqueue(&readyQueue, p);
//     printf("proc %d kforked a child %d\n", running->pid, p->pid);

//     return p;
// }

int ksleep(int event)
{
    int CPSRRegValue = int_off();   // Turn interrupts off.
    running->event = event;         // Set the event to wake for the running process
    running->status = SLEEP;        // Set the status of running process to sleeping
    tswitch();                      // Switch running process
    int_on(CPSRRegValue);            // Turn interrupts back on.
}

int kwakeup(int event)
{
    int CPSRRegValue = int_off();   // Turn interrupts off
    for (u32 i = 0; i < NPROC; i++)
    {
        PROC * p = &proc[i];
        if (p->status == SLEEP && p->event == event)
        {
            p->status = READY;
            enqueue(&readyQueue, p);
        }
    }
    int_on(CPSRRegValue);
}

int kexit(int exitValue)
{
    // 1. Erase process user-mode context values, e.g., close descriptors, release resources, etc.
    // 2. Dispose of children processes, i.e., give them to the p1 process
    // 3. record exitValue in process exitcode
    // 4. Become a ZOMBIE process
    // 5. Wakeup parent process, and, if necessary, the init process in P1
    // 6. Switch processes
    PROC * p1           = &proc[1];
    PROC * p1Child      = p1->child;
    PROC * curOrphan    = running->child;
    PROC * prevOrphan   = NULL;

    int CPSRRegValue = int_off();

    if (running == &proc[1])
    {
        kprintf("Can't kill process 1... Kernel panic?\n");
        return -1;
    }

    if (curOrphan != NULL)
    {
        while (curOrphan != NULL)
        {
            curOrphan->parent   = p1;
            curOrphan->status   = ZOMBIE;
            prevOrphan          = curOrphan;
            curOrphan           = curOrphan->sibling;
        }

        prevOrphan->sibling = p1Child;
        p1->child           = running->child;
        running->child      = NULL;
    }

    running->exitCode   = exitValue;
    running->status     = ZOMBIE;
    running->priority   = 0;
    kwakeup(running->parent);
    tswitch();
    int_on(CPSRRegValue);
}

int kwait(int *status)
{
    PROC * curChild = running->child;
    PROC * prevChild = NULL;
    int CPSRRegValue = int_off();

    if (curChild == NULL)
    {
        return -1; // Error, no children.
    }

    while (curChild != NULL)
    {
        if (curChild->status == ZOMBIE)
        {
            int procID = curChild->pid;
            *status = curChild->exitCode;
            curChild->status = FREE;

            if (prevChild != NULL)
            {
                prevChild->sibling = curChild->sibling;
            }
            else
            {
                running->child = curChild->sibling;
            }
            
            enqueue(&freeList, curChild);
            return procID;
        }

        curChild = curChild->sibling;
        prevChild = curChild;
    }

    int_on(CPSRRegValue);
    ksleep(running);
}