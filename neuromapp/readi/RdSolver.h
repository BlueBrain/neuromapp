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

#include "Tets.h"
#include <vector>
#include <string>
#include <fstream>
#include <cassert>

namespace readi {

template <class IntType, class FloatType>
class RdSolver {
public:

    void read_mesh_and_model(std::string const& filename_mesh, std::string const& filename_model) {
        tets_.read_from_file(filename_mesh, filename_model);
        
        std::ifstream f;
       
        try {
            f.open(filename_model);
            std::string discard;
            
            // Read Species
            f >> discard >> n_species_; // read how many species
            species_names_.resize(n_species_);
            diffusion_coefficients_.resize(n_species_);
            std::getline(f, discard); // read \n
            std::getline(f, discard); // read description line
            for (IntType i=0; i<n_species_; ++i) {
                f >> species_names_[i] >> discard; // read species name
            }
            std::getline(f, discard);       // skip '\n'
            
            // Reaction is not implemented yet: readfile is skipped for this part
            std::getline(f, discard);       // skip empty line
            f >> discard >> n_reactions_;   // read how many reactions
            std::getline(f, discard);       // skip '\n'
            std::getline(f, discard);       // skip description
            reaction_coefficients_.resize(n_reactions_);
            reaction_lhs_.resize(n_reactions_);
            reaction_rhs_.resize(n_reactions_);
            for (IntType i=0; i<n_reactions_; ++i) {
                std::getline(f, discard); // read reaction lhs, rhs, and coeff
            }


            // Diffusion
            std::getline(f, discard); // skip empty line 
            std::getline(f, discard); // skip description line
            for (IntType i=0; i<n_species_; ++i) {
                f >> discard >> diffusion_coefficients_[i];
            }

        }
        catch(const std::exception& ex) {
            f.close();
            throw;
        }

        // assert diffusion coefficients are correctely read
        for (auto d : diffusion_coefficients_) 
            assert(d!=0);
    }


    // compute update period, a.k.a. tau
    FloatType get_update_period() {
        FloatType max_diffusion_coeff = *std::max_element(diffusion_coefficients_.begin(), diffusion_coefficients_.end());
        std::cout << "max diffusion coeff: " << max_diffusion_coeff << "\n";
        FloatType max_shape = tets_.get_max_shape();
        std::cout << "max shape: " << max_shape << "\n";
        std::cout << "tau = " << max_diffusion_coeff * max_shape << "\n";
        return max_diffusion_coeff * max_shape ;
    }


private:
    readi::Tets<IntType, FloatType> tets_;
    IntType n_species_;
    IntType n_reactions_;
    std::vector<std::string> species_names_;
    std::vector<FloatType> diffusion_coefficients_;
    std::vector<FloatType> reaction_coefficients_;
    std::vector< std::vector<IntType> > reaction_lhs_;
    std::vector< std::vector<IntType> > reaction_rhs_;
};

} // namespace readi
#endif// MAPP_READ_RDSOLVER_
