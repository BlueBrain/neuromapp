/*
* Neuromapp - reader.h, Copyright (c), 2015,
* Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
* Pramod Kumbhar - Swiss Federal Institute of technology in Lausanne,
* timothee.ewart@epfl.ch,
* paramod.kumbhar@epfl.ch
* All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 3.0 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library.
*/

/**
 * @file neuromapp/coreneuron_1.0/common/util/reader.h
 * Implements function for the input data reading
 */

#ifndef MAPP_INIT_
#define MAPP_INIT_

#include <stdio.h>
#include <stdlib.h>

#include "coreneuron_1.0/common/memory/nrnthread.h"

/** \fn int read_nt_from_file(const char *filename, NrnThread *nt)
    \brief read the input data and set up all fields of nrnthread
    \param filename path of the input data
    \param NrnThread Data structure of the memory
*/
int read_nt_from_file(const char *filename, NrnThread *nt);

/** \fn void print_iarray(int * data, int n)
    \brief print integer array
    \param pointer to the integer array
    \param n size of the integer array
*/
void print_iarray(int * data, int n);

/** \fn read_darray_from_file(FILE *hFile, double *data, int n);
    \brief read double array from a file
    \param hFile looking file
    \param data data to read
    \param n size of the array
*/
void read_darray_from_file(FILE *hFile, double *data, int n);

/** \fn read_iarray_from_file(FILE *hFile, int *data, int n);
    \brief read  int array from a file
    \param hFile looking file
    \param data data to read
    \param n size of the array
 */
void read_iarray_from_file(FILE *hFile, int *data, int n);

/** \fn double* get_rhs(const NrnThread * nt)
    \brief get the rhs data for post processing
    \param nt pointer to the data structure
*/
double* get_rhs(const NrnThread * nt);

/** \fn double* get_d(const NrnThread * nt)
    \brief  get the d data for post processing
    \param nt pointer to the data structure
*/
double* get_d(const NrnThread * nt);

/** \fn int get_end(const NrnThread * nt)
    \brief get the end data for post processing
    \param nt pointer to the data structure
*/
int get_end(const NrnThread * nt);

#endif
