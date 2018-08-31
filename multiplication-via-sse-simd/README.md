Vectorized Array Multiplication

Project 5 takes a break from using any openMP pragmas for performance, and instead looks at the speedup achieved from using SSE SIMD vectors that can hold up to 4 floating points per vector array. The program compares the speedup achieved from taking advantage of SIMD at the assembly language level in array multiplication and array multiplication plus summing compared to doing it normally.
