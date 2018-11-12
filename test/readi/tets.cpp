/*
 * Neuromapp - hello.cpp, Copyright (c), 2015,
 * Francesco Casalegno - Swiss Federal Institute of technology in Lausanne,
 * francesco.casalegno@epfl.ch,
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

#include <random>

#define BOOST_TEST_MODULE ReadiTest
#include "readi/readi.h"
#include <boost/test/unit_test.hpp>

#include "readi/Tets.h"

static constexpr double rel_tolerance = 1.e-12;

BOOST_AUTO_TEST_CASE(read_file_test){

    std::string file_mesh = std::string(FILEPATH_TO_DATASET) + "/data_mesh";
    std::string file_model = std::string(FILEPATH_TO_DATASET) + "/data_model";

    int random_seed = 123;
    std::mt19937 rand_engine(random_seed);

    readi::Tets<int, double> tets;

    tets.read_from_file(file_mesh, file_model, rand_engine);

    constexpr int n_tets = 6;
    BOOST_CHECK_EQUAL(tets.get_n_tets(), n_tets);
    std::vector<double> vols = {8.0e-19, 9.0e-19, 6.0e-19, 5.0e-19, 3.0e-19, 4.0e-19};
    for (int i=0; i<n_tets; ++i)
        BOOST_CHECK_CLOSE(tets.volume(i), vols[i], rel_tolerance);
    BOOST_CHECK_CLOSE(tets.get_tot_volume(), std::accumulate(vols.begin(), vols.end(), 0.), rel_tolerance);

    std::vector<std::vector<int> > neighbs = {{1,2,-1,-1},{0,4,-1,-1},{0,3,-1,-1}, {2,5,-1,-1},{1,5,-1,-1},{3,4,-1,-1}};
    for (int i=0; i<n_tets; ++i)
        for (int j=0; j<4; ++j)
            BOOST_CHECK_EQUAL(tets.neighbor(i,j),neighbs[i][j]);

    constexpr int n_specs = 3;
    std::vector<int> expected_counts = {100, 200, 300};
    for (int s=0; s<n_specs; ++s) {
        int tot_mols = 0;
        for (int i=0; i<n_tets; ++i)
            tot_mols += tets.molecule_count(s,i);
        BOOST_CHECK_CLOSE(1.0*tot_mols, expected_counts[s], 0.05);
    }

}


