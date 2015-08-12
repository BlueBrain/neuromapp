/*
 * Neuromapp - writer.h, Copyright (c), 2015,
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
 * @file neuromapp/coreneuron_1.0/common/util/writer.h
 * Implements function for the input data reading
 */

#ifndef MAPP_WRITER_
#define MAPP_WRITER_

#include <stdio.h>

#include "coreneuron_1.0/common/memory/nrnthread.h"

/** \fn dump_darray_to_file(char *fname, double *d, int n)
    \brief wrapper preparing the dump of double array into a file
    \param fname name of the file
    \param d double pointer on the array of double
    \param n size of the array
 */
void dump_darray_to_file(char *fname, double *d, int n);

/** \fn dump_iarray_to_file(char *fname, int *d, int n)
    \brief wrapper preparing the dump of integer array into a file
    \param fname name of the file
    \param d integer pointer on the array of int
    \param n size of the array
 */
void dump_iarray_to_file(char *fname, int *d, int n);

/** \fn  write_darray_to_file(FILE *hFile, double *data, int n)
    \brief Dump a double array into a file
    \param fname name of the file
    \param d double pointer to the array of double
    \param n size of the array
 */
void write_darray_to_file(FILE *hFile, double *data, int n);

/** \fn  write_iarray_to_file(FILE *hFile, int *data, int n)
    \brief Dump an integer array into a file
    \param fname name of the file
    \param d integer pointer the array of int
    \param n size of the array to write
 */
void write_iarray_to_file(FILE *hFile, int *data, int n);

/** \fn  write_nt_to_file(char *filename, NrnThread *nt)
    \brief Dump an the nrnThread structure into a file
    \param fname name of the file
    \param nt nrnThread structure to write in a file
 */
void write_nt_to_file(char *filename, NrnThread *nt);

#endif
