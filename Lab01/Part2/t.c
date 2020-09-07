/*******************************************************
*                      t.c file                        *
*******************************************************/
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;
GD    *gp;
INODE *ip;
DIR   *dp;

char buf1[BLK], buf2[BLK];
int color = 0x0A;
u8 ino;

main()
{ 
	u16    i, iblk;
	char   c, temp[64];

	prints("read block# 2 (GD)\n\r");
	getblk(2, buf1);

	// 1. WRITE YOUR CODE to get iblk = bg_inode_table block number
	iblk = 0;
	prints("inode_block="); putc(iblk+'0'); prints("\n\r"); 

	// 2. WRITE YOUR CODE to get root inode
	prints("read inodes begin block to get root inode\n\r");

	// 3. WRITE YOUR CODE to step through the data block of root inode
	prints("read data block of root DIR\n\r");

	// 4. print file names in the root directory /

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

int getblk(u16 blk, char *buf)
{
	// readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
	readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}

void recursivePWD(MINODE * cur_minode) {
    if ((cur_minode->ino != root->ino) ||
        (cur_minode->dev != root->dev)
    ) {
        int cur_inum = cur_minode->ino;
        int parent_inum = getino("..");
        MINODE * parent_minode = iget(dev, parent_inum);
        char cur_name[256];
        if (parent_minode->dev != cur_minode->dev) {
            MTABLE * p_mtable;
            int i;
            for (i = 0; i < NMTABLE; i++) {
                p_mtable = &mtable[i];
                if (p_mtable->dev == cur_minode->dev) {
                    break;
                }
            }
            if (i == NMTABLE) {
                printf("ERROR IN pwd(): broken mount table!\n");
                exit(3);
            }
            findmyname(parent_minode, p_mtable->mptr->ino, cur_name);
        } else {
            findmyname(parent_minode, cur_inum, cur_name);
        }
        running->cwd = parent_minode;
        recursivePWD(parent_minode);
        iput(parent_minode);
        printf("/%s", cur_name);
    }
}

void pwd(MINODE *wd) {
    MINODE * cwd_ref = running->cwd;
    printf("CWD = ");
    if (wd == root) printf("/");
    recursivePWD(wd);
    printf("\n");
    running->cwd = cwd_ref;
}