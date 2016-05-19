/*
 * Neuromapp - fileNb.h, Copyright (c), 2015,
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

#ifndef MAPP_FILENB_H
#define MAPP_FILENB_H

#include <string>
#include <sstream>
#include <fstream>

#include "replib/utils/config.h"
#include "replib/utils/fileview.h"
#include "replib/mpiio_dist/commonfile.h"


namespace replib {

/** \fun fileview * fileNb(config & c)
    \brief create the fileview object needed by MPI_File_set_view
    when each process writes multiple, non-contiguous blocks of data.
    It uses common code to read the write distribution from a file and
    then creates the fileview object accordingly
 */
inline fileview * fileNb(config & c) {

    // Find the corresponding line in the file
    std::vector<std::string> row;
    read_row(c, row);

    fileview * f = new fileview(c.numcells());

    unsigned int rankSize = 0, totalSize = 0;

    // Used by MPI
    f->add_length(1);
    //Lower bound
    f->add_disp(0);

    for (int i = 0; i < c.numcells(); i++) {
        int length = atoi(row[i+2].c_str());
        int disp = atoi(row[i+2+c.numcells()].c_str());
        f->add_length(length);
        f->add_disp(disp);
        rankSize += length;
    }

    MPI_Allreduce(&rankSize, &totalSize, 1, MPI_UNSIGNED, MPI_SUM, MPI_COMM_WORLD);
    c.elems_per_step() = totalSize / sizeof(float);

    // Imitating Report::prepareBuffer() behavior
    f->add_length(1);
    // Upper bound
    f->add_disp(totalSize);  //total bytes consumed by one reporting step

    f->add_dtype(MPI_LB);
    for (int i = 0; i < c.numcells(); i++) {
        f->add_dtype(MPI_BYTE);
    }
    f->add_dtype(MPI_UB);

    return f;
}

} // namespace replib

#endif // MAPP_FILENB_H
