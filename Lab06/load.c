#pragma once

#include "type.h"

int getblock(int blk, char* buf);
u32 search(INODE* node, char* filename);
int load (char *filename, PROC *process);
char *kstrtok(char *string, char delim);
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
    printf("-------------- Attempting to load: %s --------------\n", filename);
    GD* pGroupDesc;
    INODE* pINode;
    u16 index;
    int loadedINode;
    u16 INodeTableBlock;

    // Get groupd descriptor
    getblock(GROUPDESCBLOCK, gdBuffer);
    pGroupDesc = gdBuffer;
    INodeTableBlock = (u16) pGroupDesc->bg_inode_table;

    // Get the inode table block
    getblock(INodeTableBlock, buffer);
    pINode = (INODE *) buffer + 1;
    kstrtok(filename, '/');
    char * next = kstrtok(NULL, '/');

    while(next != NULL && loadedINode >= 0)
    {
        printf("-------------- file:%s, inodenum: %d --------------\n", next, loadedINode);
        loadedINode = attemptLoadToBuffer(pINode, INodeTableBlock, next, buffer);
        pINode = (INODE *) buffer + loadedINode%8;
        next = kstrtok(NULL, '/');
        printf("\tNext: %s", next);
    }
    
    printf("-------------- Outside File Search  --------------\n", next, loadedINode);

    if (loadedINode >= 0)
    {
        return loadInodeContentsIntoPageTable(pINode, process);
    }

    return FALSE;
}

int attemptLoadToBuffer(INODE *pINode, u16 INodeTableBlock, char *filename, byte *buffer)
{
    int loadedINode = search(pINode, filename) - 1;
    printf("loaded inode: %d", loadedINode);
    if (loadedINode >= 0)
    {
        printf("Maybe here?\n");
        getblock((INodeTableBlock + loadedINode/8), buffer);
        printf("have we gotten blocks?\n");
        return loadedINode;
    }

    return -1;
}

int loadInodeContentsIntoPageTable(INODE * pINode, PROC * process)
{
    kprintf("start program load\n");
    u32 index = 0, bytesLoaded = 0, pageIndex = 0;
    byte * procAddr = (byte *)(process->pgdir[2048] & ~0xC32); //&~0xc32 removes VA flag
    printf("%x\t\t%x\t%x\n", procAddr, process->pgdir[2048], process->pgdir[2049]);

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
        printf("How about here?\n");
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
                printf("Are we hanging here?\n");
            }
        }
    }

    return NULL;
}