#include <cstdio>
#include <omp.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


////Global system states
//Units of grain growth are inches.
//Units of temperature are degrees Fahrenheit (°F).
//Units of precipitation are inches.
int	    NowYear;		// 2017 - 2022
int	    NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	    NowNumDeer;		// number of deer in the current population
int     hunterGatherers; // number of human hunter-gatherers in the area

////Global constants for state changes
const float GRAIN_GROWS_PER_MONTH =		10.0;
const float ONE_DEER_EATS_PER_MONTH =	0.25;
const float HS_EATS_GRAIN_PER_MONTH =   3;
const float HS_EATS_DEER_PER_MONTH =    1;

const float AVG_PRECIP_PER_MONTH =		6.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =			    2.0;	// plus or minus noise

const float AVG_TEMP =				    50.0;	// average
const float AMP_TEMP =				    20.0;	// plus or minus
const float RANDOM_TEMP =			    10.0;	// plus or minus noise

const float MIDTEMP =				    40.0;
const float MIDPRECIP =				    10.0;

////Helper functions
//returns the square value of a float
float SQR( float x ) {
    return x*x;
}

//generate randomness using a seed and rand_r
float Ranf( unsigned int *seedp,  float low, float high ) {
    float r = (float) rand_r( seedp );              // 0 - RAND_MAX

    return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

int Ranf( unsigned int *seedp, int ilow, int ihigh ) {
    float low = (float)ilow;
    float high = (float)ihigh + 0.9999f;

    return (int)(  Ranf(seedp, low,high) );
}

//generate random temperature and precipitation based loosely on current month
unsigned int seed = (unsigned int) time(NULL);
void TemperaturePrecipitationCalc() {
    float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

    float temp = AVG_TEMP - AMP_TEMP * cos( ang );

    NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

    float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
    NowPrecip = precip + Ranf(&seed,  -RANDOM_PRECIP, RANDOM_PRECIP);
    if( NowPrecip < 0. )
        NowPrecip = 0.;
}

void SetupStartingValues() {
    // starting date and time:
    NowMonth =    0;
    NowYear  = 2017;

    // starting state:
    NowNumDeer = 1;
    NowHeight =  1.;
    hunterGatherers = 0;
}

//print header for watcher's result values
void PrintHeader () {
    printf("Year;Month;Temperature (C);Precipitation;Grain Height (cm);Deer Population;Hunter-Gatherers\n");
}

////Simulation functions
//simulation for grain deer population
void GrainDeer() {
    while( NowYear < 2023 ) {
        int tempDeer;

        if (NowNumDeer > NowHeight)
            tempDeer = NowNumDeer -1;
        else if (NowNumDeer < NowHeight)
            tempDeer = NowNumDeer + 1;
        else
            tempDeer = NowNumDeer; // should be unlikely, given that the grain height is a float

        // DoneComputing barrier:
        #pragma omp barrier
        NowNumDeer = tempDeer;
        NowNumDeer -= (hunterGatherers * HS_EATS_DEER_PER_MONTH);

        if (NowNumDeer < 0)
            NowNumDeer = 0;

        // DoneAssigning barrier:
        #pragma omp barrier

        //does nothing; waits for Watcher

        // DonePrinting barrier:
        #pragma omp barrier
        //goes back to the top if year hasn't been reached
    }
}

//simulation for Grain growth
void Grain() {
    while( NowYear < 2023 ) {
        float tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );
        float precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );

        // DoneComputing barrier:
        #pragma omp barrier
        NowHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
        NowHeight -= (NowNumDeer * ONE_DEER_EATS_PER_MONTH + hunterGatherers * HS_EATS_GRAIN_PER_MONTH);

        //height can't go negative
        if (NowHeight < 0)
            NowHeight = 0;

        // DoneAssigning barrier:
        #pragma omp barrier

        //does nothing; waits for Watcher

        // DonePrinting barrier:
        #pragma omp barrier
        //goes back to the top if year hasn't been reached
    }
}

//simulation for hunter-gatherers growth, which is dependent on grain and deer abundance
void Humans() {
    while( NowYear < 2023 ) {
        int tempHS;
        int grainFactor = (int) NowHeight / 10;

        if (grainFactor > 3 && NowNumDeer > 5)
            tempHS = hunterGatherers + 2;
        else if (NowHeight == 0. || NowNumDeer == 0)
            tempHS = hunterGatherers - 2;
        else if (grainFactor > 2 || NowNumDeer > 3)
            tempHS = hunterGatherers + 1;
        else if (grainFactor <= 1 || NowNumDeer < 3)
            tempHS = hunterGatherers - 1;

        else
            tempHS = hunterGatherers;

        //can't go negative
        if (tempHS < 0)
            tempHS = 0;

        // DoneComputing barrier:
        #pragma omp barrier
        hunterGatherers = tempHS;

        // DoneAssigning barrier:
        #pragma omp barrier

        //does nothing; waits for Watcher

        // DonePrinting barrier:
        #pragma omp barrier
        //goes back to the top if year hasn't been reached
    }
}

void Watcher() {
    while( NowYear < 2023 ) {

        //does nothing as grain and deer are computing

        // DoneComputing barrier:
        #pragma omp barrier

        //does nothing as grain and deer are assigning to globals

        // DoneAssigning barrier:
        #pragma omp barrier
        //conversion to C and centimeters
        float tempCEL = (5./9.) * (NowTemp - 32.0);
        float precipCM = 2.54 * NowPrecip;
        float heightCM = 2.54 * NowHeight;

        //print results given current month/year
        //printf("Year;Month;Temperature (C);Precipitation;Grain Height (cm);Deer Population;My Stuff\n");
        printf("%d;%d;%8.2lf;%8.2lf;%8.2lf;%d;%d\n", NowYear, NowMonth, tempCEL, precipCM,heightCM, NowNumDeer, hunterGatherers);

        //increment month and year
        NowMonth += 1;
        if (NowMonth > 11) {
            NowYear += 1;
            NowMonth = 0;
        }

        //calculate new temp and precip
        TemperaturePrecipitationCalc();

        // DonePrinting barrier:
        #pragma omp barrier
        //goes back to the top if year hasn't been reached
    }
}


int main() {
    //setup
    omp_set_num_threads(4);	// same as # of sections
    SetupStartingValues();
    TemperaturePrecipitationCalc();
    PrintHeader();

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            GrainDeer( );
        }

        #pragma omp section
        {
            Grain( );
        }

        #pragma omp section
        {
            Humans( );
        }

        #pragma omp section
        {
            Watcher( );
        }
    } // implied barrier -- all functions must return in order to allow any of them to get past here

    return 0;
}