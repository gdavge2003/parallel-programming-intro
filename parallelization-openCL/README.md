OpenCL Array Multiply, Multiply-Add, and Multiply-Reduce

Project 6 explores parallelization using the GPU via OpenCL. We do the classic array multiply, multiply-add, and multiply-reduce using the GPU processing power. OpenCL takes advantage of the GPU processors and memory buffer, does all the calculations, then simply returns the final result(s) back to the CPU after all parallel calculations are completed.

Instructions to run:
For Mult and Mult-Add:
- Run the script "run_multiple_proj6.bash"
- To change between Multiply and Multiply-Add, comment out one or the other calculation in "first.cl".

For Mult-Reduction:
- Run the script "run_multiple_proj6b_reduction.bash"