#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctime>
#include <sys/time.h>
#include <sys/resource.h>
#include <omp.h>

////Constant Definitions
//#define ARRAY_SIZE  1000000
//#define NUMTRIES    5
#define ARRAY_VALUE 2 //arbitrary value to multiply
#define SSE_WIDTH	4
#define ALIGNED		__attribute__((aligned(16)))

////SIMD Multiplication Functions
void
SimdMul( float *a, float *b,   float *c,   int len )
{
    int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
    __asm
    (
    ".att_syntax\n\t"
    "movq    -24(%rbp), %rbx\n\t"		// a
    "movq    -32(%rbp), %rcx\n\t"		// b
    "movq    -40(%rbp), %rdx\n\t"		// c
    );

    for( int i = 0; i < limit; i += SSE_WIDTH )
    {
        __asm
        (
        ".att_syntax\n\t"
        "movups	(%rbx), %xmm0\n\t"	// load the first sse register
        "movups	(%rcx), %xmm1\n\t"	// load the second sse register
        "mulps	%xmm1, %xmm0\n\t"	// do the multiply
        "movups	%xmm0, (%rdx)\n\t"	// store the result
        "addq $16, %rbx\n\t"
        "addq $16, %rcx\n\t"
        "addq $16, %rdx\n\t"
        );
    }

    for( int i = limit; i < len; i++ )
    {
        c[i] = a[i] * b[i];
    }
}

float
SimdMulSum( float *a, float *b, int len )
{
    float sum[4] = { 0., 0., 0., 0. };
    int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;

    __asm
    (
    ".att_syntax\n\t"
    "movq    -40(%rbp), %rbx\n\t"		// a
    "movq    -48(%rbp), %rcx\n\t"		// b
    "leaq    -32(%rbp), %rdx\n\t"		// &sum[0]
    "movups	 (%rdx), %xmm2\n\t"		// 4 copies of 0. in xmm2
    );

    for( int i = 0; i < limit; i += SSE_WIDTH )
    {
        __asm
        (
        ".att_syntax\n\t"
        "movups	(%rbx), %xmm0\n\t"	// load the first sse register
        "movups	(%rcx), %xmm1\n\t"	// load the second sse register
        "mulps	%xmm1, %xmm0\n\t"	// do the multiply
        "addps	%xmm0, %xmm2\n\t"	// do the add
        "addq $16, %rbx\n\t"
        "addq $16, %rcx\n\t"
        );
    }

    __asm
    (
    ".att_syntax\n\t"
    "movups	 %xmm2, (%rdx)\n\t"	// copy the sums back to sum[ ]
    );

    for( int i = limit; i < len; i++ )
    {
        sum[i-limit] += a[i] * b[i];
    }

    return sum[0] + sum[1] + sum[2] + sum[3];
}

////Non-SIMD Multiplication Functions
//non-SIMD multiplication of float array
void NonSimdMul(float *a, float *b, float *c, int len) {
    for (int i = 0; i < len; i++)
        c[i] = a[i] * b[i];
}

//non-SIMD multiplication of float array and then reduction sum
float NonSimdMulSum(float *a, float *b, int len) {
    float sum = 0;

    for (int i = 0; i < len; i++)
        sum += (a[i] * b[i]);

    return sum;
}


////Main
int main() {
    //setup and initialize arrays and value storage
    float *a = (float*) malloc(sizeof(float)*ARRAY_SIZE);
    float *b = (float*) malloc(sizeof(float)*ARRAY_SIZE);
    float *c = (float*) malloc(sizeof(float)*ARRAY_SIZE);

    for (int i = 0; i < ARRAY_SIZE; i++) {
        a[i] = ARRAY_VALUE;
        b[i] = ARRAY_VALUE;
    }
    for (int i = 0; i < ARRAY_SIZE; i++)
        c[i] = 0;

    //getting peak baseline mult time #1
    int count = 0;
    double peakTime_nonSM = 0;
    double avgTime_nonSM = 0;
    while (count < NUMTRIES) {
        double time0 = omp_get_wtime();
        NonSimdMul(a, b, c, ARRAY_SIZE);
        double time1 = omp_get_wtime();

        //recording results
        double elapsedTime = (double)ARRAY_SIZE/(time1 - time0)/1000000.;
        if (elapsedTime > peakTime_nonSM)
            peakTime_nonSM = elapsedTime;
        avgTime_nonSM += elapsedTime;

        //resetting array c in case existing stored value impacts how registry speed of handling the add
        for (int i = 0; i < ARRAY_SIZE; i++)
            c[i] = 0;

        count++;
    }
    //calculate average and record
    avgTime_nonSM /= NUMTRIES;


    //getting peak baseline mult reduction time #2
    count = 0;
    double peakTime_nonSMR = 0;
    double avgTime_nonSMR = 0;
    while (count < NUMTRIES) {
        double time0 = omp_get_wtime();
        NonSimdMulSum(a, b, ARRAY_SIZE);
        double time1 = omp_get_wtime();

        //recording results
        double elapsedTime = (double)ARRAY_SIZE/(time1 - time0)/1000000.;
        if (elapsedTime > peakTime_nonSMR)
            peakTime_nonSMR = elapsedTime;
        avgTime_nonSMR += elapsedTime;

        count++;
    }
    //calculate average and record
    avgTime_nonSMR /= NUMTRIES;


    //getting peak SIMD mult time #3
    count = 0;
    double peakTime_SM = 0;
    double avgTime_SM = 0;
    while (count < NUMTRIES) {
        double time0 = omp_get_wtime();
        SimdMul(a, b, c, ARRAY_SIZE);
        double time1 = omp_get_wtime();

        //recording results
        double elapsedTime = (double)ARRAY_SIZE/(time1 - time0)/1000000.;
        if (elapsedTime > peakTime_SM)
            peakTime_SM = elapsedTime;
        avgTime_SM += elapsedTime;

        //resetting array c in case existing stored value impacts how registry speed of handling the add
        for (int i = 0; i < ARRAY_SIZE; i++)
            c[i] = 0;

        count++;
    }
    //calculate average and record
    avgTime_SM /= NUMTRIES;


    //getting peak SIMD mult reduction time #4
    count = 0;
    double peakTime_SMR = 0;
    double avgTime_SMR = 0;
    while (count < NUMTRIES) {
        double time0 = omp_get_wtime();
        SimdMulSum(a, b, ARRAY_SIZE);
        double time1 = omp_get_wtime();

        //recording results
        double elapsedTime = (double)ARRAY_SIZE/(time1 - time0)/1000000.;
        if (elapsedTime > peakTime_SMR)
            peakTime_SMR = elapsedTime;
        avgTime_SMR += elapsedTime;

        count++;
    }
    //calculate average and record
    avgTime_SMR /= NUMTRIES;


    //print RAW data, which will be refined later in excel to match assignment specs (speedup values, 20% dif check)
    //format: array_size, nonSM peak time, nonSM avg time, SM peak time, SM avg time,
                        //nonSMR peak time, nonSMR avg time, SMR peak time, SMR avg time
    //IMPORTANT NOTE: TIME IS ACTUALLY MEGAMULTS/S
    printf("%d,%f,%f,%f,%f,%f,%f,%f,%f\n", ARRAY_SIZE, peakTime_nonSM, avgTime_nonSM, peakTime_SM, avgTime_SM,
                                                            peakTime_nonSMR, avgTime_nonSMR, peakTime_SMR, avgTime_SMR);


    //free arrays
    free(a);
    free(b);
    free(c);


    return 0;
}