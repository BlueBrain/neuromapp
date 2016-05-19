/*
 * Neuromapp - commonfile.h, Copyright (c), 2015,
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

#ifndef MAPP_CMNFILE_H
#define MAPP_CMNFILE_H

#include <string>
#include <sstream>
#include <fstream>

#include "replib/utils/config.h"
#include "replib/utils/fileview.h"


namespace replib {

/** \fun void read_row(config & c, std::vector<std::string> & row)
    \brief common code to read miniapp input data from a file (write distribution).
    The file is expected to have all the information needed by
    MPI_File_set_view (mainly lengths of chunks and their displacements).
    Format is (all values are comma-separated):
    rankID,numBlocks,lengthOfEachBlock,displacementOfEachBlock
 */
inline void read_row(config & c, std::vector<std::string> & row) {
    // Read cell distribution and offsets from file
    int distRank = c.id();
    if (c.invert()) {
        distRank = c.procs() - c.id() - 1;
    }

    // Find line belonging to each rank
    std::string cell;
    std::stringstream keyword;
    keyword << distRank << ",";
    std::string line;
    std::ifstream in(c.input_dist().c_str());

    if(in.is_open()) {
        while( getline(in, line) ) {
            if (!line.compare(0, keyword.str().size(), keyword.str())) {
                std::stringstream lineStream(line);
                while (std::getline(lineStream, cell, ',')) {
                    row.push_back(cell);
                }
                break;
            }
        }
    } else {
        std::cout << "Error opening distribution file: " << c.input_dist() << std::endl;
        c.print(std::cout);
        MPI_Abort(MPI_COMM_WORLD, 920);

    }

    MPI_Barrier(MPI_COMM_WORLD);

    // Check we read the appropriate line
    int rank = atoi(row[0].c_str());
    if (rank != distRank) {
        std::cout << "[" << c.id() << "] Error!! I read the wrong line!!!" << std::endl;
    }

    c.numcells() = atoi(row[1].c_str()) - 1;
}

} // namespace replib

#endif // MAPP_CMNFILE_H
