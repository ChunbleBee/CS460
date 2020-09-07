#include <string.h>
#include "types.h"
#include "ext2.h"
#include "globals.h"

main()
{
	char c;
    char tempBuffer[65];
    u32 i = 0;

    tempBuffer[0] = '\t';

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
    prints("Root Directory Contents:\n\r");

    for (i = 0; i < 12; i++)
    {
        if (pINode->i_block[i] != 0)
        {
            getDiskBlock(pINode->i_block[i], buffDirEnt);
            pDirEnt = (struct ext2_dir_entry_2 *) buffDirEnt;

            while((u8 *) pDirEnt < &buffDirEnt[BLOCKSIZE])
            {
                strncpy(tempBuffer, pDirEnt->name, pDirEnt->name_len);
                tempBuffer[pDirEnt->name_len] = 0;
                putc('\t'); prints(tempBuffer); prints("\n\r");

                if (strcmp(tempBuffer, "boot") == 0)
                {
                    prints("\t^---Boot directory found!");
                }

                getc();
                pDirEnt = (u8 *) pDirEnt + pDirEnt->rec_len;
            }
        }
    }
    prints("Henlo... I am completible!!!");
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