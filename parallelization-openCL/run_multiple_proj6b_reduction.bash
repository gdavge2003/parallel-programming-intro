#!/bin/bash



for NUM_ELEMENTS in 1024 4096 16384 131072 524288 1048576 2097152 4194304 6291456 8388608 10485760
do
	g++ -DNUM_ELEMENTS=$NUM_ELEMENTS -w -o project6b_reduction_run project6b_reduction.cpp /scratch/cuda-7.0/lib64/libOpenCL.so -lm -fopenmp
	./project6b_reduction_run
done
