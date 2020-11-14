#pragma once
#include "type.h"
extern PROC *freeList, *readyQueue, *running, proc[NPROC];

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