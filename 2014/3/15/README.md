An example of a modified SENNA parser that can be used in a "piped" mode, where input is sent through a named input pipe and the output is retrieved through a named output pipe.

* Download the SENNA parser version 3.0: http://ml.nec-labs.com/senna/ 
* Replace the SENNA_main.c with the C-file from this repository.
* Compile the code: 
```
gcc -o senna -O3 -ffast-math *.c
````
* Create pipes:
```
mkfifo input_pipe
mkfifo output_pipe
```
* Start the SENNA parser:
/senna_piped.sh input_pipe output_pipe
* Test from another console:
```
./test_pipe.sh input-small
```


