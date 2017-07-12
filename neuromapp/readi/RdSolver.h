/*
 * Neuromapp - RdSolver.h, Copyright (c), 2015,
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

/**
 * @file neuromapp/readi/RdSolver.h
 * \brief Reac-Diff solver for Readi Miniapp
 */

#ifndef MAPP_READ_RDSOLVER_
#define MAPP_READ_RDSOLVER_

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>

#include "Model.h"
#include "Tets.h"
#include "CompRej.h"


namespace readi {


template <class IntType, class FloatType>
class RdSolver {
public:


    // read from file both model and mesh
    void read_mesh_and_model(std::string const& filename_mesh, std::string const& filename_model) {
        model_.read_from_file(filename_model);
        tets_.read_from_file(filename_mesh, filename_model);
    }


    // compute update period, a.k.a. tau
    FloatType get_update_period() {
        FloatType max_diffusion_coeff = model_.get_max_diff();
        FloatType max_shape = tets_.get_max_shape();
        printf("computed update period tau = %1.15e\n", 1.0 / (max_diffusion_coeff * max_shape));
        return 1.0 / (max_diffusion_coeff * max_shape) ;
    }


    // run simulation for a tau period
    void run_period_ssa(FloatType tau) {
        run_reactions(tau);
        run_diffusions(tau);
    }


    // run reactions
    void run_reactions(FloatType tau) {
        std::cout << "---> Running reactions.\n";
        // TODO: fill this function
        return;
    }


    // run diffusions
    void run_diffusions(FloatType tau) {
        std::cout << "---> Running diffusions.\n";
        for (IntType s=0; s<model_.get_n_species(); ++s) {
            diffuse(tau, s, model_.diffusion_coeff(s));  // diffuse molecules of species s
            tets_.empty_buckets(s);                     // empty buckets of species s
        }

    }


    // run diffusion of molecules of species s
    void diffuse(FloatType tau, IntType s, FloatType diff_cnst) {

        std::random_device rd;
        std::mt19937 g(rd());

        // diffuse molecule of s-th species from every tetrahedron
        for (IntType i=0; i<tets_.get_n_tets(); ++i) {

            FloatType zeta_k = diff_cnst * tets_.shape_sum(i) * tau;                // zeta_k = prob. of local diffusion
            IntType n_leaving_max = tets_.molecule_count(s, i);                     // compute max n. of molecules that may leave

            // TODO: n_leaving_max should be based on occupancy

            readi::binomial_distribution<IntType> binomial(n_leaving_max, zeta_k);
            IntType tot_leaving_mols = binomial(g);                                 // compute n. of molecules that will actually leave
            tets_.molecule_count(s, i) -= tot_leaving_mols;                         // remove from origin tet n. of molecules leaving

            FloatType shapes_partial = tets_.shape_sum(i);                          // select destinations with multinomial
            for (IntType j=0; j<3; ++j) {
                readi::binomial_distribution<IntType> binomial_destination(tot_leaving_mols, tets_.shape(i,j)/shapes_partial);
                IntType leaving_neighb = binomial_destination(g);
                tot_leaving_mols -= leaving_neighb;
                tets_.add_to_bucket(i, j, leaving_neighb);
                shapes_partial -= tets_.shape(i,j);
            }

            tets_.add_to_bucket(i, 3, tot_leaving_mols);                            // last remaining direction possible: all the rest
        }

    }




private:
    readi::Model<IntType, FloatType> model_;
    readi::Tets<IntType, FloatType> tets_;
    readi::CompRej<IntType, FloatType> comprej_;
};

} // namespace readi
#endif// MAPP_READ_RDSOLVER_
