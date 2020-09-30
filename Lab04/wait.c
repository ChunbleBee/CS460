//#include "type.h"
extern PROC *freeList, *readyQueue, *running, *proc;

/************ wait.c file ****************/
int ksleep(int event)
{
    int CPSRRegValue = int_off();   // Turn interrupts off.
    running->event = event;         // Set the event to wake for the running process
    running->status = SLEEP;        // Set the status of running process to sleeping
    tswitch();                      // Switch running process
    in_on(CPSRRegValue);            // Turn interrupts back on.
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
    Proc * p1           = &proc[1];
    Proc * p1Child      = p1->child;
    Proc * curOrphan    = running->child;
    Proc * prevOrphan   = NULL;

    int CPSRRegValue = int_off();

    if (curOrphan != NULL)
    {
        while (curOrphan != NULL)
        {
            curOrphan->parent   = p1;
            prevOrphan          = curOrphan;
            curOrphan           = curOrphan->sibling;
        }
        prevOrphan->sibling = p1Child;
        p1->child           = running->child;
        running->child      = NULL;
    }

    running->exitCode   = exitValue;
    running->status     = ZOMBIE;
    kwakeup(running->parent);
    tswitch();
    int_on(CPSRRegValue);
}

int kwait(int *status)
{
    Proc * curChild = running->child;

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
            enqueue(&freelist, curChild);
            return procID;
        }

        curChild = curChild->sibling;
    }

    ksleep(running);
}