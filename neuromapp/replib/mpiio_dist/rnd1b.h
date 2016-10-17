/*
 * Neuromapp - rnd1b.h, Copyright (c), 2015,
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

#ifndef MAPP_RND1B_H
#define MAPP_RND1B_H

#include "replib/utils/config.h"
#include "replib/utils/fileview.h"
#include "replib/mpiio_dist/common1b.h"


namespace replib {

/** \fun fileview * file1b(config & c)
    \brief create the fileview object needed by MPI_File_set_view
    when each process writes a single, contiguous block of data.
    It randomly assigns a certain load to each process and
    then uses common code to create the fileview object
 */
fileview * rnd1b(config & c) {

    // Total number of compartments per process
    // (accounting all the GIDs assigned to the process)
    unsigned int compCount = 0;

    //generate cells -> use round robin for gid assignment
    for (int gid = 1+c.id(); gid < 1+c.numcells(); gid += c.procs()) {
        // Random number of compartments for this gid
        compCount += std::rand() % 512 + std::rand() % 128 + 200;
    }

    // Number of elements to skip for this process
    //int offsetElems;

    // Size (bytes) and number of elements that this process will write at every reporting cycle
    unsigned int elemsToWrite = compCount;

    fileview * f = common1b(c, elemsToWrite * sizeof(float));

    return f;
}

} // namespace replib

#endif // MAPP_RND1B_H
