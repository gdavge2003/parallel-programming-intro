#!/bin/bash

echo "Begin running."

g++ project4.cpp -o project4_run -lm -fopenmp -std=c++11
./project4_run


echo "Finished running."