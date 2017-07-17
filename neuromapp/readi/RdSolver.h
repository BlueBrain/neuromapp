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
#include <unordered_set>

#include "Model.h"
#include "Tets.h"
#include "CompRej.h"


namespace readi {


template <class IntType, class FloatType>
class RdSolver {
public:

    // c-tor
    RdSolver(IntType seed = 42) :
        rand_engine_(seed)
    {}



    // read from file both model and mesh
    void read_mesh_and_model(std::string const& filename_mesh, std::string const& filename_model) {

        // Initialize mesh structure
        tets_.read_from_file(filename_mesh, filename_model, rand_engine_);

        // Initialize model structure
        model_.read_from_file(filename_model);

        // Initialize composition-rejection structure (holding and handling propensity values)
        comprej_.set_size(model_.get_n_reactions(), tets_.get_n_tets());
        recompute_all_propensities();

    }

    // recompute value of each propensity to initialize the whole composition-rejection structure
    void recompute_all_propensities() {
        for (IntType i=0; i<tets_.get_n_tets(); ++i)
            for (IntType r=0; r<model_.get_n_reactions(); ++r) {
                FloatType new_prop_val = model_.compute_reaction_propensity(r, i, tets_);
                comprej_.update_propensity(r, i, new_prop_val);
            }
    }


    // recompute propensities affected by r-th reaction in i-th tetrahedron
    void recompute_propensities_after_reac(IntType r, IntType i) {
        std::vector<IntType>& dependencies_idxs = model_.get_reaction_dependencies(r);
        for (IntType r_idx=0; r_idx<dependencies_idxs.size(); ++r_idx) {
            FloatType new_prop_val = model_.compute_reaction_propensity(r_idx, i, tets_);
            comprej_.update_propensity(r_idx, i, new_prop_val);
        }
    }

    // recompute propensities affected by diffusion of s-th species in i-th tetrahedron
    void recompute_propensities_after_diff(IntType s, IntType i) {
        std::vector<IntType>& dependencies_idxs = model_.get_diffusion_dependencies(s);
        for (IntType r_idx=0; r_idx<dependencies_idxs.size(); ++r_idx) {
            FloatType new_prop_val = model_.compute_reaction_propensity(r_idx, i, tets_);
            comprej_.update_propensity(r_idx, i, new_prop_val);
        }
    }


    // compute update period, a.k.a. tau
    FloatType get_update_period() {
        FloatType max_diffusion_coeff = model_.get_max_diff();
        FloatType max_shape = tets_.get_max_shape();
        return 1.0 / (max_diffusion_coeff * max_shape) ;
    }


    // run simulation for a tau period
    void run_period_ssa(FloatType tau) {
        printf("----  REAC-DIFF info ----------------------------------------\n");
        printf("\t computed tau : %1.5e\n", tau);
        run_reactions(tau);
        run_diffusions(tau);
        printf("-------------------------------------------------------------\n");

    }


    // run reactions
    void run_reactions(FloatType tau) {
        FloatType elapsed_time = 0.;
        IntType n_reacs_run = 0;
        while (true) {
            // Exact SSA Algorithm:
            // dt ~ Exp(lambda = a_0)           [select time of next reaction]
            // j  ~ Categorical(p_i = a_i/a_0)  [select idx of next reaction]
            FloatType u = (rand_engine_() - rand_engine_.min())/double(rand_engine_.max() - rand_engine_.min());
            FloatType dt = - std::log(u) / comprej_.get_total_propensity();
            if (elapsed_time + dt > tau)
                break;
            elapsed_time += dt;
            IntType next_reac_i; // idx of tet where next reaction takes place
            IntType next_reac_r; // idx of next reaction that takes place
            comprej_.select_next_reaction(rand_engine_, &next_reac_r, &next_reac_i);
            model_.apply_reaction(next_reac_r, next_reac_i, tets_);
            recompute_propensities_after_reac(next_reac_r, next_reac_i);

            ++n_reacs_run;
        }
        printf("\t completed reactions (n. of events=%d)\n", n_reacs_run);
        // TODO: fill this function
        return;
    }


    // run diffusions
    void run_diffusions(FloatType tau) {
        for (IntType s=0; s<model_.get_n_species(); ++s) {
            run_diffusion_for_species(tau, s, model_.diffusion_coeff(s));  // diffuse molecules of species s
            tets_.empty_buckets(s);                     // empty buckets of species s
        }
        printf("\t completed diffusions\n");

    }


    // run diffusion of molecules of species s
    void run_diffusion_for_species(FloatType tau, IntType s, FloatType diff_cnst) {
        std::unordered_set<IntType> update_tet_idxs;                            // set where we insert all idxes of tets affected by diffusion

        // diffuse molecule of s-th species from every tetrahedron
        for (IntType i=0; i<tets_.get_n_tets(); ++i) {

            FloatType zeta_k = diff_cnst * tets_.shape_sum(i) * tau;                // zeta_k = prob. of local diffusion
            IntType n_leaving_max = tets_.molecule_count(s, i);                     // compute max n. of molecules that may leave

            // TODO: n_leaving_max should be based on occupancy

            readi::binomial_distribution<IntType> binomial(n_leaving_max, zeta_k);
            IntType tot_leaving_mols = binomial(rand_engine_);                      // compute n. of molecules that will actually leave
            tets_.molecule_count(s, i) -= tot_leaving_mols;                         // remove from origin tet n. of molecules leaving
            if (tot_leaving_mols)
                update_tet_idxs.insert(i);

            FloatType shapes_partial = tets_.shape_sum(i);                          // select destinations with multinomial
            for (IntType j=0; j<3; ++j) {
                readi::binomial_distribution<IntType> binomial_destination(tot_leaving_mols, tets_.shape(i,j)/shapes_partial);
                IntType leaving_neighb = binomial_destination(rand_engine_);
                tot_leaving_mols -= leaving_neighb;
                if (leaving_neighb)
                    update_tet_idxs.insert(tets_.neighbor(i, j));
                tets_.add_to_bucket(i, j, leaving_neighb);
                shapes_partial -= tets_.shape(i,j);
            }

            tets_.add_to_bucket(i, 3, tot_leaving_mols);                            // last remaining direction possible: all the rest
            if (tot_leaving_mols)
                update_tet_idxs.insert(tets_.neighbor(i, 3));
        }

        for (auto i : update_tet_idxs)
            recompute_propensities_after_diff(s, i);                                // update propensity of reacs in tets affected by diffusion

    }




private:
    std::mt19937 rand_engine_;
    readi::Tets<IntType, FloatType> tets_;
    readi::Model<IntType, FloatType> model_;
    readi::CompRej<IntType, FloatType> comprej_;
};

} // namespace readi
#endif// MAPP_READ_RDSOLVER_
