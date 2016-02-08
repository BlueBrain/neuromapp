/*
 * Neuromapp - nrn_thread.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/coreneuron_1.0/common/memory/nrnthread.h
 * \brief Implements data structure of coreneuron1.0
 */

#ifndef MAPP_NRNTHREAD_
#define MAPP_NRNTHREAD_

#include <stdio.h>


/** \struct Mechanism
 *  \brief Represents the neuron membrane channels (in a compartment)
 */
typedef struct Mechanism {
    /* Total number of mechanism */
    int nodecount;
    /* Total number of mechanism with pad*/
    int nodecount_pad;
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

/** \struct NrnThread
 *  \brief A dataset representing a group of cells, their compartments, mechanisms, etc,
 */
typedef struct NrnThread {
    int _ndata;
    int nmech;
    /** Total number of Cell */
    int ncell;
    /** Total number of compartment */
    int end;
    /** Total number of compartment with padding*/
    int end_pad;
    /** time */
    double _t;
    /** step time */
    double _dt;
    /** Total non-mechanism data allocated in one (contiguous) region */
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

/** \brief Construct NrnThread from file.
 *  \param fh File handle used for reading.
 *  \param nt NrnThread structure to write to.
 *  \return non-zero on error.
 *
 *  The NrnThread object resulting from a successful construction
 *  should be destroyed with the nrnthread_dealloc() function.
 */
int nrnthread_read(FILE *fh, NrnThread *nt);

/** \brief Serialise NrnThread to file.
 *  \param fh File handle used for writing.
 *  \param nt NrnThread structure to write.
 *  \return non-zero on error.
 *
 *  The serialized data can be read with nrnthread_read().
 */
int nrnthread_write(FILE *fh, const NrnThread *nt);

/** \brief Copy NrnThread data to new NrnThread.
 *  \param p The NenThread object to copy.
 *  \param nt The target NrnThread.
 *  \return non-zero on error.
 *
 *  Any data held in nt will be overwritten.
 *  Copied NrnThread data should be deallocated with
 *  nrnthread_dealloc().
 */
int nrnthread_copy(const NrnThread *p, NrnThread *nt);

/** \brief Deallocate NrnThread data constructed by nrnthread_read() or nrnthread_clone().
 *  \param nt The NenThread object to destroy.
 *  \return non-zero on error.
 */
int nrnthread_dealloc(NrnThread *nt);

#endif
