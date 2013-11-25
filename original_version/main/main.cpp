//
//  main.cpp
//  mini_apps
//
//  Created by Ewart Timothée + alex on 13/11/13.
//  Copyright (c) 2013 Ewart Timothée. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include "include/section.h"
#include "include/membfunc.h"

#include <boost/chrono.hpp>

Memb_func* memb_func;

extern "C" void nrn_alloc(Prop *_prop, int iMech, int iNumVars);

extern void nrn_mk_prop_pools(int n);
extern void prop_free(Prop **pp);


void nrn_cur(double* p,int num_node);
void nrn_init(double* p,int num_node);
void nrn_state(double* p,int num_node);


/// Number of variables for the imaginary method
const int iTotNumMechs = 10;
int iArrVars[iTotNumMechs] = {18, 23, 17, 20, 14, 17, 10, 23, 15, 11};

int iAccessType, iNumMechs, iNumLoops, iLoop;
Prop *p = NULL, *pp, *head;


#define ERR_NO_NUM -1
#define ERR_NO_MEM -2

int myRandom (int size)
{
    int i, n;
    static int numNums = 0;
    static int *numArr = NULL;

    // Initialize with a specific size.

    if (size >= 0) {
        if (numArr != NULL)
            free (numArr);
        if ((numArr = (int*)malloc (sizeof(int) * size)) == NULL)
            return ERR_NO_MEM;
        for (i = 0; i  < size; i++)
            numArr[i] = i;
        numNums = size;
    }

    // Error if no numbers left in pool.

    if (numNums == 0)
        return ERR_NO_NUM;

    // Get random number from pool and remove it (rnd in this
    //   case returns a number between 0 and numNums-1 inclusive).

    n = rand() % numNums;
    i = numArr[n];
    numArr[n] = numArr[numNums-1];
    numNums--;
    if (numNums == 0) {
        free (numArr);
        numArr = 0;
    }

    return i;
}


/// The function to set the values of the double param mechanism property

void set_values(double *dParam, int iMech)
{
    srand (3);
    int iNumVars = iArrVars[iMech];
    for (int iVar = 0; iVar < iNumVars; ++iVar)
    {
        dParam[iVar] = 10*drand48();
    }
}


/// The function to get the values of the double param mechanism property
void get_values(double *dTestParam, double *dParam, int iMech)
{
    int iNumVars = iArrVars[iMech];
    for (int iVar = 0; iVar < iNumVars; ++iVar)
    {
        dTestParam[iVar] = dParam[iVar];
    }
}


/// The function to test the values of the double param mechanism property. It compares two provided arrays for them to be equal in values.
void test_values(double *dTestParam, double *dParam, int iMech)
{
    int iNumVars = iArrVars[iMech];
    for (int iVar = 0; iVar < iNumVars; ++iVar)
    {
        assert(dTestParam[iVar] == dParam[iVar]);
    }
}


/// Data access with the consequent loop over same property vars
void access_pattern1(void)
{

    /// Loop to allocate properties
    //  for (int iNum = iNumMechs-1; iNum >= 0; --iNum)

    p = (Prop *)emalloc(sizeof(Prop));
    p->next=NULL;
    p->type = 0;
    p->ob = (Object*)0;
    p->_alloc_seq = -1;
    p->dparam = (Datum *)0;
    p->param = (double *)0;
    p->param_size = 0;

    /// Allocate param and dparam of the property corresponding to the type of the mechanism
    nrn_alloc(p, p->type, iArrVars[p->type]);

    double *dParam = p->param;
    //            int iNumVars = iArrVars[p->type];

    /// Set the values of param doubles of the correspondent property
    set_values(dParam, p->type);

    head = p;

    for (int iNum = 0; iNum < iNumMechs; ++iNum)
    {
        for (iLoop = 1; iLoop < iNumLoops; ++iLoop)
        {

            pp = (Prop *)emalloc(sizeof(Prop));
            pp->next = NULL;
            p->next = pp;
            p = pp;
            p->type = iNum;
            p->ob = (Object*)0;
            p->_alloc_seq = -1;
            p->dparam = (Datum *)0;
            p->param = (double *)0;
            p->param_size = 0;

            /// Allocate param and dparam of the property corresponding to the type of the mechanism
            nrn_alloc(p, p->type, iArrVars[p->type]);

            double *dParam = p->param;
//            int iNumVars = iArrVars[p->type];

            /// Set the values of param doubles of the correspondent property
            set_values(dParam, p->type);
        }
    }
}


/// Data pattern to access the data with the loop of each property following another
void access_pattern2(void)
{
    /// Loop to allocate properties
    for (iLoop = 0; iLoop < iNumLoops; ++iLoop)
    {
        for (int iNum = 0; iNum < iNumMechs; ++iNum)
        {
            pp = (Prop *)emalloc(sizeof(Prop));
            pp->next = p;
            p = pp;
            p->type = iNum;
            p->ob = (Object*)0;
            p->_alloc_seq = -1;
            p->dparam = (Datum *)0;
            p->param = (double *)0;
            p->param_size = 0;

            /// Allocate param and dparam of the property corresponding to the type of the mechanism
            nrn_alloc(p, p->type, iArrVars[p->type]);

            double *dParam = p->param;
            int iNumVars = iArrVars[p->type];

            /// Set the values of param doubles of the correspondent property
            set_values(dParam, iNumVars);
        }
    }
}


///This data access pattern is for the completely random list of properties which are supposedly allocated for different nodes having a unique integer for a random number
void access_pattern3(void)
{
    /// Loop with a randomizer to allocate properties
    srand (time (NULL));
    int iVar = myRandom (iNumMechs*iNumLoops);
    while (iVar >= 0)
    {
        //    printf ("Number = %3d\n", iVar);

        pp = (Prop *)emalloc(sizeof(Prop));
        pp->next = p;
        p = pp;
        p->type = (int)(floor(iVar / iNumLoops));
        p->ob = (Object*)0;
        p->_alloc_seq = -1;
        p->dparam = (Datum *)0;
        p->param = (double *)0;
        p->param_size = 0;

        /// Allocate param and dparam of the property corresponding to the type of the mechanism
        int iNumVars = iArrVars[p->type];
        nrn_alloc(p, p->type, iNumVars);

        double *dParam = p->param;

        /// Set the values of param doubles of the correspondent property
        set_values(dParam, iNumVars);

        iVar = myRandom (-1);
    }
}


///This data access pattern is for the completely random list of properties which are supposedly allocated for different nodes with repeated integers
void access_pattern4(void)
{
    /// Loop with a randomizer to allocate properties
    srand (time (NULL));
    for (int iLoop = 0; iLoop < iNumLoops; ++iLoop)
    {
        int iVar = rand() % iNumMechs;
        //      printf ("Number = %d\n", iVar);
        pp = (Prop *)emalloc(sizeof(Prop));
        pp->next = p;
        p = pp;
        p->type = iVar;
        p->ob = (Object*)0;
        p->_alloc_seq = -1;
        p->dparam = (Datum *)0;
        p->param = (double *)0;
        p->param_size = 0;

        /// Allocate param and dparam of the property corresponding to the type of the mechanism
        int iNumVars = iArrVars[p->type];
        nrn_alloc(p, p->type, iNumVars);

        double *dParam = p->param;

        /// Set the values of param doubles of the correspondent property
        set_values(dParam, iNumVars);
    }
}


/// Data access with the random access built with the randomization of list of properties allocated
void access_pattern5(void)
{
    Prop ***prop_arr = new Prop**[iNumMechs];
    for (int iNum = 0; iNum < iNumMechs; ++iNum)
        prop_arr[iNum] = new Prop*[iNumLoops];

    /// Loop to allocate properties in odrer
    for (int iNum = 0; iNum < iNumMechs; ++iNum)
    {
        for (iLoop = 0; iLoop < iNumLoops; ++iLoop)
        {
            p = (Prop *)emalloc(sizeof(Prop));
            p->next = NULL;
            p->type = iNum;
            p->ob = (Object*)0;
            p->_alloc_seq = -1;
            p->dparam = (Datum *)0;
            p->param = (double *)0;
            p->param_size = 0;

            /// Allocate param and dparam of the property corresponding to the type of the mechanism
            nrn_alloc(p, p->type, iArrVars[p->type]);

            double *dParam = p->param;
            int iNumVars = iArrVars[p->type];

            /// Set the values of param doubles of the correspondent property
            set_values(dParam, iNumVars);

            prop_arr[iNum][iLoop] = p;
        }
    }

    /// Create a list of properties from random locations
    int iCol, iRow;
    int iVar = myRandom (iNumMechs*iNumLoops);
    iCol = (int)(floor(iVar / iNumLoops));
    iRow = iVar % iNumLoops;

    //int iCount = 1;

    //printf("The column is: %d, the row is: %d\n", iCol, iRow);

    p = prop_arr[iCol][iRow];
    iVar = myRandom (-1);
    while (iVar >= 0)
    {
        iCol = (int)(floor(iVar / iNumLoops));
        iRow = iVar % iNumLoops;

        //iCount++;
        //printf("The column is: %d, the row is: %d\n", iCol, iRow);

        pp = prop_arr[iCol][iRow];
        pp->next = p;
        p = pp;

        iVar = myRandom (-1);
    }

    //printf("Created the property list of size of %d\n", iCount);

    for (int iNum = 0; iNum < iNumMechs; ++iNum)
        delete [] prop_arr[iNum];
    delete [] prop_arr;
}

int main(int argc, char **argv)
{
    boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();
    boost::chrono::duration<double> sec_alloc;

    /// Get the data access type to test: 1 is for consequent loop over same property vars, 2 is for the loop of each property one after another
    iAccessType = 1;//atoi(argv[1]);

    /// Get the number of mechanisms to test from the command line
    iNumMechs = 1;//atoi(argv[2]);

    /// Get the number to loop for from the comand line
    iNumLoops = 16384;//atoi(argv[3]);

    /// Allocate that many double array pools and datum array pools
    nrn_mk_prop_pools(30);

    sec_alloc = boost::chrono::system_clock::now() - start;


    // Apply the access type
    switch(iAccessType)
    {

        case 1:
            access_pattern1();
            break;
        case 2:
            access_pattern2();
            break;
        case 3:
            access_pattern3();
            break;
        case 4:
            access_pattern4();
            break;
        case 5:
            access_pattern5();
            break;
    }

    printf("Initialized %d properties %d times\n", iNumMechs, iNumLoops);
    printf("Using the %d data access pattern\n", iAccessType);

    boost::chrono::system_clock::time_point start_run = boost::chrono::system_clock::now();
    boost::chrono::duration<double> sec_run;

    // memory ready and init
    double *dParam = head->param;
    nrn_init(dParam,iNumLoops); // because Na channel test
    nrn_cur(dParam,iNumLoops); // because Na channel test
    nrn_state(dParam,iNumLoops); // because Na channel test
    prop_free(&p);
   
    sec_run = boost::chrono::system_clock::now() - start_run;


    std::cout << " time alloc, sec " << sec_alloc.count() << std::endl;
    std::cout << " time run, sec "   << sec_run.count() << std::endl;

}
