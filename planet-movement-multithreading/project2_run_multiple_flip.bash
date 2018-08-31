#!/bin/bash

if [[ $# -ne 1 ]]; then
	numtries=1
else
	numtries=$1
fi

for threads in 1 2 4 6 8 16 24
do
	for is_coarse in 0 1
	do
		if [[ is_coarse ]]; then
			is_fine=0
		else
			is_fine=1
		fi

		for allocation in static dynamic
		do
        	g++ -DNUMTRIES=$numtries -DNUMTHREADS=$threads -DIS_COARSE_GRAINED=$is_coarse -DIS_FINE_GRAINED=$is_fine -DALLOCATION=$allocation project2.cpp -o project2_run -lm -fopenmp -std=c++11
        	./project2_run

        done
    done
done
