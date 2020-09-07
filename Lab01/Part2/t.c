#include <string.h>
#include "types.h"
#include "ext2.h"
#include "globals.h"

main()
{
	char c;
    char tempBuffer[INPUTBUFFERSIZE];
    u32 i = 0;

    prints("Group Descriptor Block: "); putc(GROUPDESCBLOCK + '0'); prints("\n");
	getDiskBlock(GROUPDESCBLOCK, buffGroupDesc);
    pGroupDesc = (struct ext2_group_desc*) buffGroupDesc;
    groupINodeTable = (u16)pGroupDesc->bg_inode_table;

    prints("INode table Block: "); putc(groupINodeTable + '0'); prints("\n");
    getDiskBlock(blockINodeTable, buffINodeBlock);
    pINode = (struct ext2_inode*) buffINodeBlock;

	prints("Reading data block of Root Directory Entry...\n");
    prints("Press any key to continue...\n\n");
    getc();

    prints("Root Directory Contents:\n");
    for (i = 0; i < 12; i++)
    {
        prints("Reading i_block: "); putc(i + '0'); prints("\n");
        getc();

        if (pINode->i_block[i] != 0)
        {
            getDiskBlock(pINode->i_block[i], buffDirEnt);
            pDirEnt = (struct ext2_dir_entry_2 *) buffDirEnt;

            while(pDirEnt < &buffDirEnt[BLOCKSIZE])
            {
                strncpy(tempBuffer, pDirEnt->name, pDirEnt->name_len);
                tempBuffer[pDirEnt->name_len] = '\0';

                prints(tempBuffer);

                if (strcmp(tempBuffer, "boot") == 0)
                {
                    prints("\t---Boot directory found!");
                }
                prints("\n");

                pDirEnt = (u8 *) pDirEnt + pDirEnt->rec_len;
            }
        }
    }
    prints("Henlo... I am completible!!!");
    getc();
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

int getDiskBlock(u16 blk, u8 *buf)
{
    readfd(((2*blk)/CYLINDERS), (((2*blk)%CYLINDERS)/TRACKS), (((2*blk)%CYLINDERS)%TRACKS), buf);
}