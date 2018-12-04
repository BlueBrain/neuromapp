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

#include "readi/CompRej.h"
#include "readi/RdSolver.h"

static constexpr double rel_tolerance = 1.e-12;

struct FakeModel {
    unsigned int get_n_reactions() const { return n_reacs; }
    unsigned int n_reacs;
};

BOOST_AUTO_TEST_CASE(composition_rejection_test) {

    constexpr int n_tets = 6;
    constexpr int n_reacs = 4;

    using mesh_type = readi::SSAMesh<unsigned int, double>;
    mesh_type mesh;
    readi::CompRej<mesh_type> comprej(mesh);
    comprej.set_size(n_reacs, n_tets);
    FakeModel model{n_reacs};

    comprej.update_propensity(model, 0, 1, 0.49);
    comprej.update_propensity(model, 1, 3, 0.51);
    comprej.update_propensity(model, 3, 2, 4.12);

    BOOST_CHECK_CLOSE(comprej.get_total_propensity(), 5.12, rel_tolerance);

    comprej.update_propensity(model, 0, 1, 0.0);
    comprej.update_propensity(model, 1, 3, 0.0);
    comprej.update_propensity(model, 3, 2, 0.0);

    BOOST_CHECK_CLOSE(comprej.get_total_propensity(), 0.0, rel_tolerance);
}
