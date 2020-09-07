struct ext2_group_desc  *pGroupDesc;
struct ext2_inode       *pINode;
struct ext2_dir_entry_2 *pDirEnt;

u8 buffGroupDesc[BLOCKSIZE];
u8 buffINodeBlock[BLOCKSIZE];
u8 buffDirEnt[BLOCKSIZE];

int color = 0x0A;
u16 curINodeBlock;
u16 groupINodeTable;