#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "time.c"

//#define _RESTR_ restrict 
#define _RESTR_

#define CHANNEL_LEN 18
#define ION_LEN 3

int random_max = 20;

double *pdata_aos;

double *p_1, *p_2, *p_3, *p_4, *p_5, *p_6, *p_7, *p_8, *p_9;
double *p_10, *p_11, *p_12, *p_13, *p_14, *p_15, *p_16, *p_17, *p_18;

double * soa_arr[CHANNEL_LEN];

/* New Vectors */
double * _RESTR_ VEC_RHS;
double * _RESTR_ VEC_V;

double * _RESTR_ ppvar;
double * _RESTR_ ppvar0, * _RESTR_ ppvar1, * _RESTR_ ppvar2;




void alloc_mechanism(int num_mechs)
{
    int i, j;
    int cnt = 0;

    p_1 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[0] = p_1;  
    p_2 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[1] = p_2;    
    p_3 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[2] = p_3;
    p_4 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[3] = p_4;
    p_5 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[4] = p_5;
    p_6 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[5] = p_6;
    p_7 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[6] = p_7;
    p_8 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[7] = p_8;
    p_9 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[8] = p_9;
    p_10 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[9] = p_10;
    p_11 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[10] = p_11;
    p_12 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[11] = p_12;
    p_13 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[12] = p_13;
    p_14 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[13] = p_14;
    p_15 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[14] = p_15;
    p_16 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[15] = p_16;
    p_17 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[16] = p_17;
    p_18 = (double *) malloc( sizeof(double) * num_mechs);soa_arr[17] = p_18;

    pdata_aos = (double *) malloc( sizeof(double) * num_mechs * CHANNEL_LEN );


    VEC_RHS = (double *) malloc( sizeof(double) * num_mechs );

    VEC_V   = (double *) malloc( sizeof(double) * num_mechs );
    
    ppvar   = (double *) malloc( sizeof(double) * num_mechs * ION_LEN );
    ppvar0 =  (double *) ( ppvar + 0 * num_mechs);
    ppvar1 =  (double *) ( ppvar + 1 * num_mechs);    
    ppvar2 =  (double *) ( ppvar + 2 * num_mechs);

 
}

void init_mechanism(int num_mechs)
{
    int i, j;
    long int seed = 0;
    long int cnt=0;
    seed = time (NULL);
    srand (seed);

    #pragma omp parallel for
    for( i=0; i<num_mechs; i++)
    {
        for( j=0; j<CHANNEL_LEN; j++)
            pdata_aos[i*CHANNEL_LEN+j] = ( rand() % random_max) + 10.1;
            //pdata_aos[i*CHANNEL_LEN+j] = cnt++ + 0.1;

        VEC_RHS[i] = ( rand() % random_max) + 10.1;
        VEC_V[i] = ( rand() % random_max) + 10.1;

        ppvar0[i] = ( rand() % random_max) + 10.1;
        ppvar1[i] = ( rand() % random_max) + 10.1;
        ppvar2[i] = ( rand() % random_max) + 10.1;


        p_1[i] = pdata_aos[i*CHANNEL_LEN];
        p_2[i] = pdata_aos[i*CHANNEL_LEN+1];
        p_3[i] = pdata_aos[i*CHANNEL_LEN+2];
        p_4[i] = pdata_aos[i*CHANNEL_LEN+3];
        p_5[i] = pdata_aos[i*CHANNEL_LEN+4];
        p_6[i] = pdata_aos[i*CHANNEL_LEN+5];
        p_7[i] = pdata_aos[i*CHANNEL_LEN+6];
        p_8[i] = pdata_aos[i*CHANNEL_LEN+7];
        p_9[i] = pdata_aos[i*CHANNEL_LEN+8];
        p_10[i] = pdata_aos[i*CHANNEL_LEN+9];
        p_11[i] = pdata_aos[i*CHANNEL_LEN+10];
        p_12[i] = pdata_aos[i*CHANNEL_LEN+11];
        p_13[i] = pdata_aos[i*CHANNEL_LEN+12];
        p_14[i] = pdata_aos[i*CHANNEL_LEN+13];
        p_15[i] = pdata_aos[i*CHANNEL_LEN+14];
        p_16[i] = pdata_aos[i*CHANNEL_LEN+15];
        p_17[i] = pdata_aos[i*CHANNEL_LEN+16];
        p_18[i] = pdata_aos[i*CHANNEL_LEN+17];
    }
}

void na_cur_soa(int num_mechs)
{
    int _cntml = num_mechs;
    int i;

    double rhs, v, dina;

    #pragma acc parallel loop 
    #pragma ivdep
    #pragma omp parallel for
    for (i = 0; i < _cntml; i++) 
    {
        v = VEC_V[i];
        p_5[i] = ppvar0[i];

         //nrn_current
         p_16[i] = v+0.001;
         p_2[i]  =  p_7[i] *  p_3[i] *  p_3[i] *  p_3[i] * p_4[i];
         p_1[i] =  p_2[i] * (  p_16[i] -  p_5[i] );
         p_17[i] =  p_1[i];

         dina = p_1[i];

         //nrn_current
         p_16[i] = v;
         p_2[i]  =  p_7[i] *  p_3[i] *  p_3[i] *  p_3[i] * p_4[i];
         p_1[i] =  p_2[i] * (  p_16[i] -  p_5[i] );
         rhs =  p_1[i];

         ppvar2[i] += (dina - p_1[i]) / 0.001;

        p_17[i] = ( p_17[i] - VEC_RHS[i]) / 0.001;
        ppvar1[i] += p_1[i];

        VEC_RHS[i] -= rhs;
    }

}

/* nrn_state kernel in SOA layout: original from neurodamus */
void compute_mechanism_soa(int num_mechs)
{
    int _cntml = num_mechs;
    int i;

    #pragma acc parallel loop 
    #pragma ivdep
    #pragma omp parallel for
    for (i = 0; i < _cntml; i++) 
    {
        p_8[i] = ( 0.182 * ( p_16[i] - - 35.0 ) )/ ( 1.0 - ( exp ( - ( p_16[i] - - 35.0 ) / 9.0 ) ) ) ;
        p_9[i] = ( 0.124 * ( - p_16[i] - 35.0 ) )  / ( 1.0 - ( exp ( - ( - p_16[i] - 35.0 ) / 9.0 ) ) ) ;

        p_6[i] = p_8[i] / ( p_8[i] + p_9[i] ) ;
        p_7[i] = 1.0 / ( p_8[i] + p_9[i] ) ;

        p_3[i] = p_3[i] + (1. - exp(0.01*(( ( ( -1.0 ) ) ) / p_7[i])))*(- ( ( ( p_6[i] ) ) / p_7[i] ) / ( ( ( ( -1.0) ) ) / p_7[i] ) - p_3[i]);
        p_12[i] = ( 0.024 * ( p_16[i] - - 50.0 ) ) / ( 1.0 - ( exp ( - ( p_16[i] - - 50.0 ) / 5.0 ) ) ) ;

        p_13[i] = ( 0.0091 * ( - p_16[i] - 75.0 ) ) / ( 1.0 - ( exp ( - ( - p_16[i] - 75.0 ) / 5.0 ) ) ) ;
        p_10[i] = 1.0 / ( 1.0 + exp ( ( p_16[i] - - 65.0 ) / 6.2 ) ) ;
        p_11[i] = 1.0 / ( p_12[i] + p_13[i] ) ;

        p_4[i] = p_4[i] + (1. - exp(0.01*(( ( ( -1.0 ) ) ) / p_11[i])))*(- ( ( ( p_10[i] ) ) / p_11[i] ) / ( ( ( ( -1.0) ) ) / p_11[i] ) - p_4[i]);
    }

}

int main(int argc, char *argv[])
{
    int num_mechs;
    char filename [100];

    if(argc < 2)
    {
        printf("\n Pass number of mechanisms as an argument \n");
        exit(1);
    }
    else
    {
        num_mechs = atoi(argv[1]);

        printf("\n Allocating total mechanisms: %d (%4.2lf GB)\n", num_mechs, (sizeof(double) * num_mechs * CHANNEL_LEN )/(1024*1024*1024.0) );

        alloc_mechanism(num_mechs);
        init_mechanism(num_mechs);

        /* Compute time for SOA layout*/
        #pragma acc data copy(p_8[0:num_mechs], p_16[0:num_mechs], p_9[0:num_mechs], p_6[0:num_mechs], p_7[0:num_mechs], p_3[0:num_mechs], \
        p_4[0:num_mechs], p_10[0:num_mechs], p_11[0:num_mechs], p_12[0:num_mechs], p_13[0:num_mechs], p_14[0:num_mechs])
        {
            gettimeofday(&tvBegin, NULL);
            compute_mechanism_soa(num_mechs);
            gettimeofday(&tvEnd, NULL);
        }

        timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
        printf("\n SOA State OpenACC Version : %ld.%06ld", tvDiff.tv_sec, tvDiff.tv_usec);


        #pragma acc data copy(p_1[0:num_mechs], p_2[0:num_mechs], p_3[0:num_mechs], p_4[0:num_mechs], p_16[0:num_mechs], p_7[0:num_mechs], \
        p_5[0:num_mechs], ppvar0[0:num_mechs], ppvar1[0:num_mechs], ppvar2[0:num_mechs], VEC_V[0:num_mechs], VEC_RHS[0:num_mechs])
        {
            gettimeofday(&tvBegin, NULL);
            na_cur_soa(num_mechs);
            gettimeofday(&tvEnd, NULL);
        }
        
        timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
        printf("\n SOA Curr Version : %ld.%06ld", tvDiff.tv_sec, tvDiff.tv_usec);



        printf("\n\n");        
    }

    return 0;
}
