#pragma once

#include "type.h"
#include "string.h"

int getblock(int blk, char* buf);
u32 search(INODE* node, char* filename);
int load (char *filename, PROC *process);
int attemptLoadToBuffer(INODE *pINode, u16 INodeTableBlock, char *filename, byte *buffer);
int loadInodeContentsIntoPageTable(INODE * pINode, PROC * process);
int loadprogram(char *filename, PROC *process);

const char * bin = "bin";

byte buffer[BLOCK_SIZE];
byte gdBuffer[BLOCK_SIZE];
byte indirectBuffer[BLOCK_SIZE];
byte inodeBuffer[BLOCK_SIZE];

int load (char *filename, PROC *process)
{
    GD* pGroupDesc;
    INODE* pINode;
    u16 index;
    u16 loadedINode;
    u16 INodeTableBlock;

    // Get groupd descriptor
    getblock(GROUPDESCBLOCK, gdBuffer);
    pGroupDesc = gdBuffer;
    INodeTableBlock = (u16) pGroupDesc->bg_inode_table;

    // Get the inode table block
    printf("----------------1------------------\n");
    getblock(INodeTableBlock, buffer);
    printf("----------------2------------------\n");
    pINode = (INODE *) buffer + 1;

    char * next = strtok(filename, '/');
    printf("----------------3: %s --------------\n", next);
    loadedINode = attemptLoadToBuffer(pINode, INodeTableBlock, next, buffer);
    printf("----------------4: %d --------------\n", loadedINode);

    while(next != NULL && loadedINode >= 0)
    {
        pINode = (INODE *) buffer + loadedINode%8;
        next = strtok(NULL, '/');
        loadedINode = attemptLoadToBuffer(pINode, INodeTableBlock, next, buffer);
    }

    if (loadedINode >= 0)
    {
        return loadInodeContentsIntoPageTable(pINode, process);
    }

    return FALSE;
}

int attemptLoadToBuffer(INODE *pINode, u16 INodeTableBlock, char *filename, byte *buffer)
{
    u16 loadedINode = search(pINode, filename) - 1;
    if (loadedINode >= 0)
    {
        getblock((INodeTableBlock + loadedINode/8), buffer);
        return loadedINode;
    }

    return -1;
}

int loadInodeContentsIntoPageTable(INODE * pINode, PROC * process)
{
    kprintf("start program load\n");
    u32 index = 0, bytesLoaded = 0, pageIndex = 0;
    byte * procAddr = (byte *)(process->pgdir[2048] & ~0xC32); //&~0xc32 removes VA flag
    printf("%x\t\t%x\n", procAddr, process->pgdir[2048]);

    for (index = 0; index < 12; index++)
    {
        kprintf(
            "\tiblock: %d\taddr: %x\texists: %c\n",
            pINode->i_block[index],
            procAddr,
            ((pINode->i_block[index] == 0) ? 'F' : 'T')
        );
    
        if (pINode->i_block[index] == NULL) { break; }
        getblock(pINode->i_block[index], procAddr);
        procAddr += KB;
        bytesLoaded += KB;
    }
    if (pINode->i_block[12] != NULL)
    {
        printf("We'll deal with indirect blocks later\n");
    }

    printf("%d bytes loaded into page address\n", bytesLoaded);
    return bytesLoaded;
}

u32 search(INODE* node, char* filename)
{
    u32 index;
    byte heldByte;
    byte buffer[BLOCK_SIZE];
    byte* pBufferIndex;
    DIR* pDirEnt;

    for (index = 0; index < 12; index++)
    {
        if (node->i_block[index] != NULL)
        {
            getblock(node->i_block[index], buffer);
            pBufferIndex = buffer;

            while (pBufferIndex < buffer + BLOCK_SIZE)
            {
                pDirEnt = (DIR *) pBufferIndex;
                heldByte = pDirEnt->name[pDirEnt->name_len];
                pDirEnt->name[pDirEnt->name_len] = '\0';
                printf("\t%s ", pDirEnt->name);
                
                if (strcmp(filename, pDirEnt->name) == 0)
                {
                    printf("-- FOUND, at iblock: %d\n", pDirEnt->inode);
                    return pDirEnt->inode;
                }
                printf("\n");

                pDirEnt->name[pDirEnt->name_len] = heldByte;
                pBufferIndex += pDirEnt->rec_len;
                pDirEnt = (DIR *) pBufferIndex;
            }
        }
    }

    return NULL;
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