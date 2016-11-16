/*
 * Neuromapp - common1b.h, Copyright (c), 2015,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * judit.planas@epfl.ch,
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
 * GNU General Public License for more details. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

#ifndef MAPP_CMN1B_H
#define MAPP_CMN1B_H

#include <mpi.h>

#include "replib/utils/config.h"
#include "replib/utils/fileview.h"


namespace replib {

/** \fun fileview * common1b(config & c, unsigned int elemsToWrite)
    \brief common code to create the fileview object needed by MPI_File_set_view
    when each process writes a single, contiguous block of data
 */
inline fileview * common1b(config & c, unsigned int elemsToWrite) {

    // Number of elements to skip for this process
    int offsetElems = 0;

    // Write buffer treated as a single block, so it's like we process a single cell
    c.numcells() = 1;
    fileview * f = new fileview(c.numcells());

    // Used by MPI
    f->add_length(1);
    // Number of elements this process will write
    f->add_length(elemsToWrite);
    // Imitating Report::prepareBuffer() behavior
    f->add_length(1);

    // Find out the offset for this process with respect its lower ranks
    int elemsPerStep = 0;
    elemsToWrite = elemsToWrite / sizeof(float);
    if (c.procs() > 1) {
        MPI_Status status;
        if (c.id() == 0) {
            offsetElems = 0;
            int lastElem = offsetElems + elemsToWrite;
            MPI_Send(&lastElem, 1, MPI_INT, c.id()+1, c.id(), MPI_COMM_WORLD);
        } else {
            MPI_Recv(&offsetElems, 1, MPI_INT, c.id()-1, c.id()-1, MPI_COMM_WORLD, &status);
            int lastElem = offsetElems + elemsToWrite;
            if (c.id() < c.procs()-1) {
                MPI_Send(&lastElem, 1, MPI_INT, c.id()+1, c.id(), MPI_COMM_WORLD);
            } else {
                elemsPerStep = lastElem;
            }
        }
        MPI_Bcast(&elemsPerStep, 1, MPI_INT, c.procs()-1, MPI_COMM_WORLD);
    } else {
        // Single process running, no need to coordinate with others
        elemsPerStep = elemsToWrite;
    }

    c.elems_per_step() = elemsPerStep;

    // Lower bound
    f->add_disp(0);
    // Displacement
    f->add_disp(offsetElems * sizeof(float));
    // Upper bound
    f->add_disp(elemsPerStep * sizeof(float));  //total bytes consumed by one reporting step

    f->add_dtype(MPI_LB);
    for (int i = 0; i < c.numcells(); i++) {
        f->add_dtype(MPI_BYTE);
    }
    f->add_dtype(MPI_UB);

    return f;
}

} // namespace replib

#endif // MAPP_CMN1B_H
