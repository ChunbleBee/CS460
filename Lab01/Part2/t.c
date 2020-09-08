#include "types.h"
#include "ext2.h"
#include "globals.h"

u16 search(IndexNode* node, char* filename)
{
    u16 index;
    u8 heldByte;
    u8* pBufferIndex;
    DirectoryEntry* pDirEnt;

    for (index = 0; index < 12; index++)
    {
        if ((u16)node->i_block[index] != NULL)
        {
            getDiskBlock((u16)node->i_block[index], ioBuffer02);
            pBufferIndex = ioBuffer02;

            while (pBufferIndex < ioBuffer02 + BLOCKSIZE)
            {
                pDirEnt = (DirectoryEntry *) pBufferIndex;
                heldByte = pDirEnt->name[pDirEnt->name_len];
                pDirEnt->name[pDirEnt->name_len] = '\0';
                //prints(pDirEnt->name);
                
                if (strcmp(filename, pDirEnt->name) == 0)
                {
                    //prints(" -FOUND\n\r");
                    return pDirEnt->inode;
                }

                //prints("\n\r");
                pDirEnt->name[pDirEnt->name_len] = heldByte;
                pBufferIndex += pDirEnt->rec_len;
                pDirEnt = (DirectoryEntry *) pBufferIndex;
            }
        }
    }

    return NULL;
}

int main()
{
    GroupDescriptor* pGroupDesc;
    IndexNode* pINode;
    u16 INodeTableBlock;
    u16 index;
    u32* pIndirect;
    u16 BootINodes[2];
    char* BootNames[2];
    char* stepPassed = "+\n\r";
    BootINodes[0] = 0;
    BootINodes[1] = 0;
    BootNames[0] = "boot";
    BootNames[1] = "mtx";

    getDiskBlock(GROUPDESCBLOCK, ioBuffer01);
    pGroupDesc = ioBuffer01;
    INodeTableBlock = (u16) pGroupDesc->bg_inode_table;
    getDiskBlock(INodeTableBlock, ioBuffer01);
    prints(stepPassed);
    pINode = (IndexNode *) ioBuffer01 + 1;
    for (index = 0; index < 2; index++)
    {
        BootINodes[index] = search(pINode, BootNames[index]) - 1;
        prints(stepPassed);
        if (BootINodes[index] < 0)
        {
            error();
        }

        getDiskBlock(INodeTableBlock + BootINodes[index]/8, ioBuffer01);
        pINode = (IndexNode *) ioBuffer01 + BootINodes[index]%8;
    }
    if ((u16)pINode->i_block[12] != NULL)
    {
        getDiskBlock((u16) pINode->i_block[12], ioBuffer02);
        pIndirect = (u32 *) ioBuffer02;
        prints("-\n\r");
    }

    setes(0x1000);
    prints(stepPassed);
    for (index = 0; index < 12; index++)
    {
        getDiskBlock((u16)pINode->i_block[index], NULL);
        inces();
    }
    prints(stepPassed);
    if ((u16)pINode->i_block[12] != NULL)
    {
        while(*pIndirect != NULL)
        {
            getDiskBlock((u16)*pIndirect, NULL);
            inces();
            pIndirect++;
        }
    }
    prints(stepPassed);
}

int prints(char *s)
{
	while (*s != '\0')
	{
		putc(*s++);
	}
}

// int gets(char *s)
// {
//     char next = '\0';
//     while (TRUE)
//     {
//         next = getc();
//         if (next = '\r')
//         {
//             break;
//         }
//         *s = next;
//         s++;
//     }
//     *s = '\0';
// }

int getDiskBlock(u16 block, u8* buffer)
{
    readfd(
        (2*block)/CYLINDERS,
        ((2*block)%CYLINDERS)/TRACKS,
        ((2*block)%CYLINDERS)%TRACKS,
        buffer);
}