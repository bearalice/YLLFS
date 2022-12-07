#include "fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct superblock sb;

struct inode *inodes;

struct disk_block *dbs;

// create a new file system
void create_fs(char *filename, int size)
{
    sb.num_inodes = 10;
    sb.num_blocks = 100;
    sb.size_blocks = sizeof(struct disk_block);

    inodes = malloc(sizeof(struct inode) * sb.num_inodes);

    int i;

    for (i = 0; i < sb.num_inodes; i++)
    {
        inodes[i].size = -1;
        inodes[i].first_block_num = -1; // -1 means empty
        strcpy(inodes[i].name, "emptyfi");
        // inodes[i].name = "";
    }

    dbs = malloc(sizeof(struct disk_block) * sb.num_blocks);

    for (i = 0; i < sb.num_blocks; i++)
    {
        dbs[i].next_block_num = -1;
    }
};

// mount an existing file system
void mount_fs()
{
    FILE *file;
    file = fopen("fs_data", "r");

    // read superblock
    fread(&sb, sizeof(struct superblock), 1, file);

    // read inodes
    // int i;
    // for (i = 0; i < sb.num_inodes; i++)
    // {
    //     fread(&inodes[i], sizeof(struct inode), 1, file);
    // }
    inodes = malloc(sizeof(struct inode) * sb.num_inodes);
    dbs = malloc(sizeof(struct disk_block) * sb.num_blocks);

    fread(inodes, sizeof(struct inode), sb.num_inodes, file);
    fread(dbs, sizeof(struct disk_block), sb.num_blocks, file);

    fclose(file);

    // read disk blocks
    // for (i = 0; i < sb.num_blocks; i++)
    // {
    //     fread(&dbs[i], sizeof(struct disk_block), 1, file);
    // }

}; // how do i know it's working - add print_fs()

void print_fs()
{
    printf("superblock info: num of inodes: %d num of blocls: %d block size: %d\n", sb.num_inodes, sb.num_blocks, sb.size_blocks);

    printf("inode info: \n");
    int i;
    for (i = 0; i < sb.num_inodes; i++)
    {
        printf("inode %d: size: %d, first_block_num: %d, name: %s\n", i, inodes[i].size, inodes[i].first_block_num, inodes[i].name);
    }

    printf("disk block info: \n");
    for (i = 0; i < sb.num_blocks; i++)
    {
        printf("disk block %d: next block num: %d\n", i, dbs[i].next_block_num);
    }
}

// write all dirty blocks to disk
void sync_fs()
{
    FILE *file;
    file = fopen("fs_data", "w+");

    // write the superblock
    fwrite(&sb, sizeof(struct superblock), 1, file);

    fwrite(inodes, sizeof(struct inode), sb.num_inodes, file);
    fwrite(dbs, sizeof(struct disk_block), sb.num_blocks, file);

    // inodes
    // int i;
    // for (i = 0; i < sb.num_inodes; i++)
    // {
    //     fwrite(&inodes[i], sizeof(struct inode), 1, file);
    // }

    // disk blocks
    // for (i = 0; i < sb.num_blocks; i++)
    // {
    //     fwrite(&dbs[i], sizeof(struct disk_block), 1, file);
    // }

    fclose(file);
};

// helper functions
int find_empty_inode()
{
    int i;
    for (i = 0; i < sb.num_inodes; i++)
    {
        if (inodes[i].size == -1)
        {
            return i;
        }
    }
    return -1;
}

int find_empty_disk_block()
{
    int i;
    for (i = 0; i < sb.num_blocks; i++)
    {
        if (dbs[i].next_block_num == -1)
        {
            return i;
        }
    }
    return -1;
}

int allocate_file(char filename[12])
{
    // find an empty inode
    int inode_num = find_empty_inode();

    int block_num = find_empty_disk_block();

    // claim it
    inodes[inode_num].first_block_num = block_num;
    dbs[block_num].next_block_num = -2; // -2 means end of file

    // set the name
    strcpy(inodes[inode_num].name, filename);
    // find and claim a disk block
    // return file descriptor
    return inode_num;
};

// helper functions
void shorten_file(int block_num)
{
    int next_block_num = dbs[block_num].next_block_num;
    if (next_block_num >= 0)
    {
        shorten_file(next_block_num);
    }
    dbs[block_num].next_block_num = -1;
};

// add or delete blocks as needed
void set_file_size(int file_num, int size)
{
    // how many blocks do we need?
    int tmp = size + BLOCK_SIZE - 1;
    int num = tmp / BLOCK_SIZE;
    // if (size % BLOCK_SIZE != 0)
    // {
    //     num++;
    // }

    int begin_block_num = inodes[file_num].first_block_num;
    num--;

    // grow the file if needed
    while (num > 0)
    {
        int next_block_num = dbs[begin_block_num].next_block_num;

        if (next_block_num == -2)
        {
            // need to allocate a new block
            int new_block_num = find_empty_disk_block();
            dbs[begin_block_num].next_block_num = new_block_num;
            dbs[new_block_num].next_block_num = -2;
        }

        begin_block_num = dbs[begin_block_num].next_block_num;
        num--;
    }

    // check for shrinking
    shorten_file(begin_block_num);
    dbs[begin_block_num].next_block_num = -2;
};

int get_block_num(int file_num, int offset)
{
    int togo = offset;
    int block_num = inodes[file_num].first_block_num;
    while (togo > 0)
    {
        block_num = dbs[block_num].next_block_num;
        togo--;
    }
    return block_num;
};

void write_data_to_file(int file_num, int pos, char *data)
{
    // calculate the block and find the block number
    int relative_block_num = pos / BLOCK_SIZE;

    int block_num = get_block_num(file_num, relative_block_num);
    // calculate the offset in the block

    int offset = pos % BLOCK_SIZE;
    // write the data to the block
    //  memcpy(dbs[block_num].data + offset, data, strlen(data));
    dbs[block_num].data[offset] = *data;
}; // write data to a file

/*notes:
understand
mount as read
sync as write
*/