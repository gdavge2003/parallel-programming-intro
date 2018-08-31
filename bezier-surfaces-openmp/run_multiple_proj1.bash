#!/bin/bash

for threads in 1 2 4 8
do
	for numnodes in 50 100 500 1000 1500 2500 4000 6000
	do
		echo "Running!"
        g++ -DNUMT=$threads -DNUMNODES=$numnodes project1.cpp -o project1_run -lm -fopenmp -std=c++11
        ./project1_run
    done
done
