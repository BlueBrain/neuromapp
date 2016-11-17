/*
 * Neuromapp - tools.h, Copyright (c), 2015,
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
 * GNU General Public License for more details. See
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/replib/utils/tools.cpp
 * \brief basic shell for mpi and report checking
 */

#include <iomanip>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>

#include <mpi.h>

#include "replib/utils/tools.h"



bool replib::check_report (char * report, int nwrites, int repCycleElems, int mpiSize) {

    // Vector of pairs: <rank, count> to illustrate the sequence of data
    std::vector< std::pair<int, int> > compsPerRank;
    compsPerRank.reserve(std::min(repCycleElems, 1024)); // Worse case, with some limit

    MPI_File fh;
    MPI_Status status;
    int result = MPI_File_open(MPI_COMM_SELF, report, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    if (result != MPI_SUCCESS) {
        std::cout << "Error opening file for reading" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 911);
    }

    int readSize = repCycleElems * sizeof(float);
    float * readValues = (float *) malloc(readSize);

    // Scan first dt to find out the number of compartments per rank and distributions
    memset(readValues, 0, readSize);
    MPI_Offset mpioff = 0;
    result = MPI_File_read_at(fh, mpioff, readValues, repCycleElems, MPI_FLOAT, &status);
    if (result != MPI_SUCCESS) {
        std::cout << "Error reading file" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 914);
    }

    int lastRank = -1;
    int count = 0;
    for (int s = 0; s < repCycleElems; s++) {
        int rank = std::floor(readValues[s]) / 1000;
        if (lastRank != rank) {
            count = 0;
            compsPerRank.push_back(std::make_pair(rank, count));

        }
        compsPerRank.back().second++;
        lastRank = rank;
    }

    //std::cout << "Found following sequence of comps: ";
    //for (int i = 0; i < compsPerRank.size(); i++) {
    //    std::cout << "  <" << compsPerRank[i].first << "," << compsPerRank[i].second << ">  ,";
    //}
    //std::cout << std::endl;

    // And now check the values
    // Keep track of the indices of each rank (decimal positions)
    std::vector<int> rankIdx(mpiSize, 0);
    // Index to the compsPerRank pairs
    unsigned int pairIdx = 0;
    // Number of values of the current rank, resets after we advance to a new pair of compsPerRank
    int currCount = 0;
    // Number of errors found so far
    int error = 0;

    for (int i = 0; i < nwrites; i++) {
        memset(readValues, 0, readSize);
        mpioff = i * readSize;
        MPI_File_read_at(fh, mpioff, readValues, repCycleElems, MPI_FLOAT, &status);
        //std::cout << "----------------------------- Reading values for time step " << i << " -----------------------------" << std::endl;
        for (int s = 0; s < repCycleElems; s++) {
            float expected = (float) compsPerRank[pairIdx].first * 1000.0 + (float) i + (float) ((rankIdx[compsPerRank[pairIdx].first]%1000) + 1.0) / 1000.0;
            // We're using 4 decimals, so no need to check for more precision
            if (fabs(readValues[s] - expected) > 0.0001) {
                error++;
                //std::cout << std::fixed << std::setprecision(4) << readValues[s] << "<--ERROR(" << expected << ")!!    ";
            }
            currCount++;
            rankIdx[compsPerRank[pairIdx].first]++;
            if (currCount == compsPerRank[pairIdx].second) {
                pairIdx++;
                currCount = 0;
                if (pairIdx == compsPerRank.size()) {
                    pairIdx = 0;
                    for (int j = 0; j < mpiSize; j++) {
                        rankIdx[j] = 0;
                    }
                }
            }
        }
        //std::cout << std::endl << "------------------------------------------------------------" << std::endl;
    }

    result = MPI_File_close(&fh);
    if (result != MPI_SUCCESS) {
        std::cout << "Error closing file" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 912);
    }

    free(readValues);

    //std::cout << "Report verification: " << ( (error == 0) ? "PASSED" : "FAILED") << std::endl;
    return (error == 0);
}
