#include <stdio.h>
#include <stdlib.h>
#include "fs.h"

int main()
{
    // create_fs();
    // sync_fs();

    mount_fs();
    int file_num = allocate_file("another");

    // test set file size
    /*
    set_file_size(0, 1000);
    print_fs();

    set_file_size(0, 5000);
    print_fs();

    set_file_size(0, 100);
    print_fs();
    */

    // test write file
    set_file_size(file_num, 5000);

    char data = 'b';

    int i;
    for (i = 0; i < 10; i++)
    {
        write_data_to_file(file_num, i * 100, &data);
    }

    sync_fs();

    // after sync, the file system is written to disk,
    //  so even if we comment out the allocate_file("hello") and mount, we still have hello

    printf("done\n");

    return 0;
}