#include "types.h"
#include "ext2.h"
#include "globals.h"

void strncpy(char* dest, char* source, u32 bytesToCopy)
{
    u32 index = 0;
    while(index < bytesToCopy && source[index] != '\0')
    {
        dest[index] = source[index];
        index++;
    }
    dest[index] = '\0';
}

int prints(char *s)
{
	while (*s != '\0')
	{
		putc(*s);
		s++;
	}
}

int gets(char *s)
{
	int index = 0;
	char next;

	while(index < 64)
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

int getDiskBlock(u16 blk, u8 *buf)
{
    readfd(((2*blk)/CYLINDERS), (((2*blk)%CYLINDERS)/TRACKS), (((2*blk)%CYLINDERS)%TRACKS), buf);
}

main()
{
	char    c, tempBuffer[65];

	prints("read block 2 to get group descriptor information...\n\r");
	getDiskBlock(GROUPDESCBLOCK, buffGroupDesc);
    pGroupDesc = (struct ext2_group_desc*) buffGroupDesc;
    groupINodeTable = (u16)pGroupDesc->bg_inode_table;

	prints("inode_block="); putc(groupINodeTable + '0'); prints("\n\r");
    prints("Press a key to continue...");
    getc();

	prints("Reading INode information block to obtain root INode...\n\r");
    getDiskBlock(groupINodeTable, buffINodeBlock);
    pINode = (struct ext2_inode*) buffINodeBlock;

	prints("Reading data block of Root Directory Entry...\n\r");
    prints("Root Directory Contents:\n");
    tempBuffer[0] = '\t';

    for (u32 i = 0; i < 12; i++)
    {
        if ((u16)pINode->i_block[i] != 0)
        {
            getDiskBlock((u16)pINode->i_block[i], buffDirEnt);
            u8* pInit = buffDirEnt;
            u8* pNext = buffDirEnt;

            while((pNext - pInit) < BLOCKSIZE)
            {
                pDirEnt = (struct ext2_dir_entry_2*) pNext;
                strncpy(&tempBuffer[1], pDirEnt->name, pDirEnt->name_len);
                prints(tempBuffer);
                pNext = pNext + pDirEnt->rec_len;
            }
        }
    }
}