#pragma once
#include "type.h"

int uPtable(PROC *process)
{
    u32 i;
    u32 * userPageTable = (u32 *)(4*MB + 16*KB*(process->pid)); // base address of table
    u32 flag            = 0xC32;
    u32 entry           = 0x0;

    for (i = 0; i < 2049; i++) { userPageTable[i] = NULL; }

    for (i = 0; i < 258; i++)
    {
        userPageTable[i] = entry + flag;
        entry += MB;
    }

    userPageTable[2048] = ((7*MB) + (process->pid)*MB) | flag;
    userPageTable[2049] = ((16*MB) + (process->pid)*MB) | flag;

    process->pgdir = userPageTable;
}