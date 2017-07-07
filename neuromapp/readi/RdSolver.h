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

#include "Reac.h"
#include "Tets.h"


namespace readi {

template <class IntType, class FloatType>
class RdSolver {
public:



    // access diffusion constant of s-th species
    inline FloatType& diffusion_coeff(IntType s) {
        assert(s>=0 && s<n_species_);
        return diffusion_coefficients_[s];
    }
    inline FloatType diffusion_coeff(IntType s) const {
        assert(s>=0 && s<n_species_);
        return diffusion_coefficients_[s];
    }


    // read all data from files
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
            IntType tot_mol_per_spec;
            for (IntType i=0; i<n_species_; ++i) {
                f >> species_names_[i] >> tot_mol_per_spec; // read species name and total count
                tets_.distribute_molecules(i, tot_mol_per_spec);// distribute these molecules in the mesh
            }
            std::getline(f, discard);       // skip '\n'
            
            // Reaction is not implemented yet: readfile is skipped for this part
            std::getline(f, discard);       // skip empty line
            f >> discard >> n_reactions_;   // read how many reactions
            std::getline(f, discard);       // skip '\n'
            std::getline(f, discard);       // skip description
            reaction_coefficients_.resize(n_reactions_);
            for (IntType i=0; i<n_reactions_; ++i) {
                std::getline(f, discard); // read reaction lhs, rhs, and coeff
                std::cout << "+++ reac: " << discard << std::endl;
                std::vector<std::string> lhs;
                std::vector<std::string> rhs;
                double k_reac;
                std::stringstream sstr(discard);
                std::string buf;
                while (sstr >> buf) {
                    if (buf == "-") break;
                    lhs.push_back(buf);
                }
                while (sstr >> buf) {
                    if (buf == "-") break;
                    rhs.push_back(buf);
                }
                sstr >> k_reac;
                reactions_.emplace_back(species_names_, lhs, rhs, k_reac);
            }


            // Diffusion
            std::getline(f, discard); // skip empty line 
            std::getline(f, discard); // skip description line
            for (IntType s=0; s<n_species_; ++s) {
                f >> discard >> diffusion_coeff(s);
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
        // TODO: fill this function
        return;
    }


    // run diffusions
    void run_diffusions(FloatType tau) {
        for (IntType s=0; s<n_species_; ++s) {
            tets_.diffuse(tau, s, diffusion_coeff(s));  // diffuse molecules of species s
            tets_.empty_buckets(s);                      // empty buckets of species s
        }

    }


private:
    readi::Tets<IntType, FloatType> tets_;
    IntType n_species_;
    IntType n_reactions_;
    std::vector<std::string> species_names_;
    std::vector<FloatType> diffusion_coefficients_;
    std::vector<FloatType> reaction_coefficients_;
    std::vector< readi::Reac<IntType,FloatType> > reactions_;
};

} // namespace readi
#endif// MAPP_READ_RDSOLVER_
