#!/bin/bash


for LOCAL_SIZE in 1 2 4 8 16 32 64 128 256 512 1024
do
	for NUM_ELEMENTS in 1024 4096 16384 131072 524288 1048576 2097152 4194304 6291456 8388608 10485760
	do
		g++ -DNUM_ELEMENTS=$NUM_ELEMENTS -DLOCAL_SIZE=$LOCAL_SIZE -w -o project6_run project6.cpp /scratch/cuda-7.0/lib64/libOpenCL.so -lm -fopenmp
		./project6_run
	done
done