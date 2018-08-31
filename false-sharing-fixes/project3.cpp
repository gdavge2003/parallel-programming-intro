#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <iostream>
#include <string>

//#define NUMPAD
//#define NUMT
//#define IS_FIX_ONE

struct s {
    float value;
    int pad[NUMPAD];
} Array[NUMT];

struct t {
    float value;
} Array2[NUMT];

int main() {
    omp_set_num_threads( NUMT );
    long int someBigNumber = 1000000000; //billion
    //int someBigNumber = 1000;

    //explicitly set to 0 in case compiler causes issues
    for (int i = 0; i < NUMT; i++) {
        Array[i].value = 0;
        Array2[i].value = 0;
    }

    //run with fix #1 using padding to put on dif cache lines
    if(IS_FIX_ONE) {
        double time0 = omp_get_wtime();
#pragma omp parallel for
        for (int i = 0; i < NUMT; i++) {
            for (int j = 0; j < someBigNumber; j++) {
                Array[i].value = Array[i].value + 2.;
            }
        }
        double time1 = omp_get_wtime();

        int calcTime = (int) someBigNumber / 1000000;
        double megaElementsCalculatedPerSec = ((calcTime * NUMT) / (time1 - time0) / 1.);

        //fix;thread;padding;megaElementsCalculated/s
        printf("%d;%d;%d;%lg\n", 1, NUMT, NUMPAD, megaElementsCalculatedPerSec);
    }
    //run with fix #2 using a temp variable to avoid modifying shared cache line
    else {
        double time0 = omp_get_wtime();
#pragma omp parallel for
        for (int i = 0; i < NUMT; i++) {
            float tmp = Array2[i].value;

            for (int j = 0; j < someBigNumber; j++) {
                tmp = tmp + 2.;
            }

            Array2[i].value = tmp;
        }
        double time1 = omp_get_wtime();

        int calcTime = (int) someBigNumber / 1000000;
        double megaElementsCalculatedPerSec = ((calcTime * NUMT) / (time1 - time0) / 1.);

        //fix;thread;padding;megaElementsCalculated/s
        printf("%d;%d;-;%lg\n", 2, NUMT, megaElementsCalculatedPerSec);
    }

    return 0;
}