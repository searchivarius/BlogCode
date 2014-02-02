A utility to measure the speed of random-access read.
Works only for a new linux kernel that supports clearing of caches via:

/proc/sys/vm/drop_caches

Example of use:

sudo ./test_disk 4096 128  [some directory to store test file]

The first parameter is the size of the file in MBs, the second parameter is the total amount of memory read in MBs. The third parameter is a directory where to place a (temporary) test file.



sudo is **necessary**, because regular users don't have access to drop_caches and, hence, can't clear caches.

