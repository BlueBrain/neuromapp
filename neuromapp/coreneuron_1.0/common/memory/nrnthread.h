/*
* Neuromapp - nrn_thread.h, Copyright (c), 2015,
* Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
* timothee.ewart@epfl.ch,
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
 * @file neuromapp/coreneuron_1.0/common/memory/nrnthread.h
 * Implements data structure of coreneuron1.0
 */

#ifndef _MAPP_NRNTHREAD_
#define _MAPP_NRNTHREAD_

/** \struct Membrane
 * \brief Represents the neuron membrane channels (in a compartment)
 */
typedef struct Membrane {
    /* Total number of mechanism */
    int nodecount;
    /** Total number of variables in mechanism (e.g. 18 in Na) */
    int szp;
    /** Total number of ion variable in mechanism (usually 3/4 in mechanism) */
    int szdp;
    int type;
    /** Whether the given mechanism is artificial cell */
    int is_art;
    /** offset between the channels mechanism */
    long offset;

    int *pdata;
    /** Data of the channels */
    double *data;
    int *nodeindices;
} Mechanism;

/** \struct NrnTh
 *  \brief A dataset representing a group of cells, their compartments, mechanisms, etc,
 */
typedef struct NrnTh{
    int _ndata;
    int nmech;
    /** Total number of Cell */
    int ncell;
    /** Total number of compartment */
    int end;
    /** step time */
    double dt;
    /** Total data allocated in once (contiguous) with a memory allocator */
    double *_data;
    /** Righ hand side of the sparse matrix, alias in _data */
    double *_actual_rhs;
    /** Diagonal of the sparse matrix, alias in _data */
    double *_actual_d;
    /** Top right of the sparse matrix, alias in _data */
    double *_actual_a;
    /** Bottom left of the sparse matrix, alias in _data */
    double *_actual_b;
    /** Voltage calculate by the resolution of the sparse matrice, alias in _data*/
    double *_actual_v;
    /** Area of the compartment m, alias in _data*/
    double *_actual_area;
    /** Shadow vector to remove atomic operation in current function of the mechnisms */
    double *_shadow_rhs;
    /** Shadow vector to remove atomic operation in current function of the mechnisms */
    double *_shadow_d;
    /** Total number of nodes */
    long max_nodecount;
    /** Pointer to the mechanism structure */
    Mechanism *ml;
    /** indexing of neuroni for linear algebra */
    int* _v_parent_index;

} NrnThread;

#endif
