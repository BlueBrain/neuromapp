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

#include <algorithm>

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

    // Maximum number of compartments per neuron (biological parameter)
    unsigned int maxComps = 350;

    // Save the numcells value to restore it
    int nc = c.numcells();

    //generate cells -> use round robin for gid assignment
    for (int gid = 1 + c.id(); gid < 1 + c.numcells(); gid += c.procs()) {
        // Random number of compartments for this gid
        unsigned int comps = std::rand() % 512 + std::rand() % 128 + 100;

        // Keep the biological limitation of 350 compartments per neuron
        comps = std::min(comps, maxComps);

        compCount += comps;
    }

    // Number of elements that this process will write at every simulation step
    unsigned long elemsToWrite = (unsigned long) compCount;
    // Number of elements that this process will write at every reporting step
    elemsToWrite *= (unsigned long) c.sim_steps();

    fileview * f = common1b(c, elemsToWrite * sizeof(float));

    // common1b() overwrites c.numcells() for convenience, restore original value
    c.numcells() = nc;

    return f;
}

} // namespace replib

#endif // MAPP_RND1B_H
