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


#define BOOST_TEST_MODULE ReadiTest
#include "readi/readi.h"
#include <boost/test/unit_test.hpp>

#include "readi/Model.h"

static constexpr double rel_tolerance = 1.e-12;

BOOST_AUTO_TEST_CASE(read_file_test){

    std::string file_model = std::string(FILEPATH_TO_DATASET) + "/data_model";

    readi::Model<int, double> model;

    model.read_from_file(file_model);

    constexpr int n_specs = 3;
    constexpr int n_reacs = 4;
    BOOST_CHECK_EQUAL(model.get_n_species(), n_specs);
    BOOST_CHECK_EQUAL(model.get_n_reactions(), n_reacs);

    std::vector<std::vector<int> > expected_reac_deps = {{0,1,2,3},{0,1,3},{1},{0,1,3}};
    model.compute_dependencies_of_reactions();
    for (int r=0; r<n_reacs; ++r) {
        std::vector<int> reac_deps = model.get_reaction_dependencies(r);
        BOOST_CHECK_EQUAL_COLLECTIONS(reac_deps.begin(), reac_deps.end(), expected_reac_deps[r].begin(), expected_reac_deps[r].end());
    }

    std::vector<std::vector<int> > expected_diff_deps = {{0,3},{0,2},{1}};
    model.compute_dependencies_of_diffusions();
    for (int s=0; s<n_specs; ++s) {
        std::vector<int> diff_deps = model.get_diffusion_dependencies(s);
        BOOST_CHECK_EQUAL_COLLECTIONS(diff_deps.begin(), diff_deps.end(), expected_diff_deps[s].begin(), expected_diff_deps[s].end());
    }


    model.get_max_diff();




}


