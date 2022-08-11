## Testing multi-GPU acceleration.

This code was created to test the effectiveness of the Huggingface library [Accelerate](https://github.com/huggingface/accelerate).
This was motivated by a somewhat anecdotal observation that synchronous SGD does not scale with the number of GPUs when these GPUs are "sitting" on a slow interconnect (PCI) express rather than being connected using a fast interconnect system such as NVLink.


It was intended to run as an end-to-end example (run the script [run_main.sh](run_main.sh), which does everything including installing the environment and running tests. Some manual intervention might be required, in particular, [by specifying a different CUDA version or changing otherwise an installation script](run_sub.sh).
Likewise, you may want to change the number of training examples, or run these experiments for several seeds.


## Basic requirements

1. Linux system with multiple GPUs and CUDA installed.
2. conda
