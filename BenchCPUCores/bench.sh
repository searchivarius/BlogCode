#!/bin/bash
N=$1

for ((i=1; i <= $N; ++i)) do
    export OMP_NUM_THREADS=$i
    echo "# of threads: $OMP_NUM_THREADS"
    time ./bench
done
