#include <stdio.h>
#include <stdlib.h>
#include "fs.h"

int main()
{
    create_fs();
    sync_fs();

    // mount_fs();
    allocate_file("hello");

    // test set file size
    set_file_size(0, 1000);
    print_fs();

    set_file_size(0, 5000);
    print_fs();

    set_file_size(0, 100);
    print_fs();
    sync_fs();

    // after sync, the file system is written to disk,
    //  so even if we comment out the allocate_file("hello") and mount, we still have hello

    printf("done\n");

    return 0;
}