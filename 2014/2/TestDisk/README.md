A utility to measure the speed of random-access read.
Works only for a new linux kernel that supports clearing of caches via:

/proc/sys/vm/drop_caches

Example of use (4GB test file, 128 MB to read randomly):

 ```
sudo ./test_disk 4096 128  [some directory to store test file]
 ```
 
or better (32GB test file): 

 ```
sudo ./test_disk 32768 128  [some directory to store test file]
 ```

The first parameter is the size of the file in MBs, the second parameter is the total amount of memory read in MBs. The third parameter is a directory where to place a (temporary) test file.


sudo is **necessary**, because regular users don't have access to drop_caches and, hence, can't clear caches.

**Note on bias**: some people noted that the test is slighlty biased, because (1) computation of FakeCheckSum can account for 10-20% of processing time (2) some blocks may repeat. First of all, if the size of the file is much larger than the amount of data we read randomly, a block is almost never selected two or more times. Second, it is ok to add some time **proportional** to the size of the read data. In real life, we don't just read data from disk, we somehow process it. And  processing time is often proportional to the size of the read data chunk.

