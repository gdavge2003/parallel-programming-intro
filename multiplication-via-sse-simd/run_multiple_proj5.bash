#!/bin/bash

# set first argument as number of tries otherwise default 1
if [[ $# -ne 1 ]]; then
	numtries=1
else
	numtries=$1
fi

for array_size in 1000 5000 10000 25000 50000 100000 250000 500000 750000 1000000 2000000 4000000 8000000 12000000 16000000 24000000 32000000
do
	g++ -DNUMTRIES=$numtries -DARRAY_SIZE=$array_size project5.cpp -o project5_run -lm -fopenmp -std=c++11
	./project5_run
done
