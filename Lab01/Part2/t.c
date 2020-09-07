#include "types.h"
#include "ext2.h"
#include "globals.h"

main()
{
    char tempBuffer[INPUTBUFFERSIZE];
    u16 i = 0;

	getDiskBlock(GROUPDESCBLOCK, buffGroupDesc);
    pGroupDesc = (struct ext2_group_desc*) buffGroupDesc;
    blockINodeTable = (u16) pGroupDesc->bg_inode_table;
    
    getDiskBlock(blockINodeTable, buffINodeBlock);
    pINode = (struct ext2_inode*) buffINodeBlock + 1;
    for (i = 0; i < 12; i++)
    {
        if (pINode->i_block[i] != 0)
        {
            getDiskBlock((u16) pINode->i_block[i], buffDirEnt);
            curDirPointer = buffDirEnt;
            pDirEnt = (struct ext2_dir_entry_2 *) curDirPointer;
            while(curDirPointer < buffDirEnt + BLOCKSIZE)
            {
                strncpy(tempBuffer, pDirEnt->name, pDirEnt->name_len);
                tempBuffer[pDirEnt->name_len] = '\0';

                prints(tempBuffer);
                if (strcmp(tempBuffer, "boot") == 0)
                {
                    bootInode = pDirEnt->inode;
                    prints(" - boot found.");
                }
                prints("\n\r");
                
                curDirPointer += pDirEnt->rec_len;
                pDirEnt = (struct ext2_dir_entry_2 *) curDirPointer;
            }
        }
    }
    prints("completed.\n");
}

int prints(char *s)
{
	while (*s != '\0')
	{
		putc(*s);
        if (*s == '\n')
        {
            putc('\r');
        }
		s++;
	}
}

int gets(char *s)
{
	int index = 0;
	char next;

	while(index < INPUTBUFFERSIZE)
	{
		next = getc();
		putc(next);

		if (next == '\r')
		{
			index++;
			break;
		}
		s[index] = next;
		index++;
	}
	index--;
	s[index] = '\0';
}

int getDiskBlock(u16 blk, u8* buf)
{
    readfd(
        (2*blk)/CYLINDERS,
        ((2*blk)%CYLINDERS)/TRACKS,
        ((2*blk)%CYLINDERS)%TRACKS,
        buf);
}