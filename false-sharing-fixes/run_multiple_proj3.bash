#!/bin/bash

echo "Begin running."

for fix in 0 1
do

	for threads in 1 2 4 8
	do
		if [[ fix -eq 1 ]]; then
			for pad in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
			do
	        	g++ -DIS_FIX_ONE=$fix -DNUMT=$threads -DNUMPAD=$pad project3.cpp -o project3_run -lm -fopenmp -std=c++11
	        	./project3_run
	    	done
	    else
	        g++ -DIS_FIX_ONE=$fix -DNUMT=$threads -DNUMPAD=$fix project3.cpp -o project3_run -lm -fopenmp -std=c++11
	        ./project3_run
        fi
	done
done

echo "Finished running."