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
 * Implements function for the allocation, initialisation and copy of the memory for coreneuron1.0
 */

#ifndef _MAPP_NRNTHREAD_HANDLER_H
#define _MAPP_NRNTHREAD_HANDLER_H

#include <stdlib.h>
#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/reader.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \fn void * make_nrnthread(void * filename)
    \brief allocate and load data from file
    \param filename path
*/
void * make_nrnthread(void * filename);

/** \fn void dealloc_nrnthread(void * p);
    \brief deallocat the memory
    \param p data to deallocate
*/
void dealloc_nrnthread(void * p);

/** \fn NrnThread* clone_nrnthread(NrnThread const* p);
    \brief clone the NrnThread data for the OMP benchmark
    \param NrnThread data of the master thread
*/
NrnThread* clone_nrnthread(NrnThread const* p);

#ifdef __cplusplus
}
#endif

#endif
