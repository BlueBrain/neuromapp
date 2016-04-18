/*
 * Neuromapp - file1b.h, Copyright (c), 2015,
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

#ifndef MAPP_FILE1B_H
#define MAPP_FILE1B_H

#include <string>
#include <sstream>
#include <fstream>

#include "replib/utils/config.h"
#include "replib/utils/fileview.h"
#include "replib/mpiio_dist/common1b.h"
#include "replib/mpiio_dist/commonfile.h"



namespace replib {

/** \fun fileview * file1b(config & c)
    \brief create the fileview object needed by MPI_File_set_view
    when each process writes a single, contiguous block of data.
    It uses common code to read the write distribution from a file and
    then create the fileview object
 */
inline fileview * file1b(config & c) {

    // Find the corresponding line in the file
    std::vector<std::string> row;
    read_row(c, row);

    // Compute total write size for this process
    unsigned int rankSize = 0;
    for (int i = 0; i < c.numcells(); i++) {
        rankSize += atoi(row[i+2].c_str());
    }
    
    // Create the vectors that MPI fileview will use
    fileview * f = common1b(c, rankSize);

    return f;
}

} // namespace replib

#endif // MAPP_FILE1B_H
