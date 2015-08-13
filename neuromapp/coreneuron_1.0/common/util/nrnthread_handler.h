/*
* Neuromapp - nrnthread_handler.h, Copyright (c), 2015,
* Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
* Pramod Kumbhar - Swiss Federal Institute of technology in Lausanne,
* Sam Yates - Swiss Federal Institute of technology in Lausanne,
* timothee.ewart@epfl.ch,
* paramod.kumbhar@epfl.ch
* sam.yates@epfl.ch
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
 * @file neuromapp/coreneuron_1.0/common/util/nrnthread_handler.h
 * Implements storage-API compatible functions for the allocation
 * and deallocation of NrnThread structures.
 */

#ifndef MAPP_NRNTHREAD_HANDLER_H
#define MAPP_NRNTHREAD_HANDLER_H

#include <stdlib.h>
#include "coreneuron_1.0/common/memory/nrnthread.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \fn void *make_nrnthread(void *filename)
    \brief Allocate NrnThread object and load data from file
    \param filename path (as void * context variable)
    \return Pointer to the constructed NrnThread object,
            or NULL on error.

    Allocated NrnThread objects should be freed with
    free_nrnthread().
*/
void *make_nrnthread(void *filename);

/** \fn void *clone_nrnthread(void *nrn)
    \brief Allocate a new NrnThread object on the heap and initialise
           with data from the NrnThread object pointed to by nrn.
           
    \param p pointer to existing NrnThread object (as void * context variable)
    \return Pointer to the allocated and constructed NrnThread object,
            or NULL on error.

    Allocated NrnThread objects should be freed with
    free_nrnthread().
*/
void *clone_nrnthread(void *p);


/** \fn void free_nrnthread(void * p);
    \brief Deallocate NrnThread data and free NrnThread object itself.
    \param p Pointer to heap-allocated NrnThread object.
*/
void free_nrnthread(void *p);

#ifdef __cplusplus
}
#endif

#endif
