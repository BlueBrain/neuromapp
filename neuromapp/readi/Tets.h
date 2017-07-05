/*
 * Neuromapp - Tets.h, Copyright (c), 2015,
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
 * @file neuromapp/readi/Tets.h
 * \brief Mesh for  Readi Miniapp
 */

#ifndef MAPP_READI_TETS_
#define MAPP_READI_TETS_

#include <fstream>
#include <string>
#include <cassert>
#include <algorithm>
#include <numeric>

#include "rng_utils.h"


namespace readi {

template<class IntType, class FloatType >
class Tets{
public:
    using idx_type = IntType;
    using real_type = FloatType;

    
    // access volume of i-th tetrahedron
    inline FloatType& volume(IntType i) {
        assert(i>=0 && i<n_tets_);
        return volumes_[i];
    }
    inline FloatType volume(IntType i) const {
        assert(i>=0 && i<n_tets_);
        return volumes_[i];
    }


    // access idx of j-th neighbor (j=0..3) of i-th tetrahedron
    inline IntType& neighbor(IntType i, IntType j) {
        assert(i>=0 && i<n_tets_);
        assert(j>=0 && j<=3);
        return neighbors_[4*i+j];
    }
    inline IntType neighbor(IntType i, IntType j) const {
        assert(i>=0 && i<n_tets_);
        assert(j>=0 && j<=3);
        return neighbors_[4*i+j];
    }

    
    // access shape of j-th neighbor (j=0..3) of i-th tetrahedron
    inline FloatType& shape(IntType i, IntType j) {
        assert(i>=0 && i<n_tets_);
        assert(j>=0 && j<=3);
        return shapes_[4*i+j];
    }
    inline FloatType shape(IntType i, IntType j) const {
        assert(i>=0 && i<n_tets_);
        assert(j>=0 && j<=3);
        return shapes_[4*i+j];
    }


    // access sum of neigh shapes of i-th tetrahedron
    inline FloatType& shape_sum(IntType i) {
        assert(i>=0 && i<n_tets_);
        return shapes_sums_[i];
    }
    inline FloatType shape_sum(IntType i) const {
        assert(i>=0 && i<n_tets_);
        return shapes_sums_[i];
    }


    // access molecule for s-th species in i-th tetrahedron
    inline FloatType& molecule_count(IntType s, IntType i) {
        assert(s>=0 && s<n_species_);
        assert(i>=0 && i<n_tets_);
        return mol_counts_[n_tets_*s + i];
    }
    inline FloatType molecule_count(IntType s, IntType i) const {
        assert(s>=0 && s<n_species_);
        assert(i>=0 && i<n_tets_);
        return mol_counts_[n_tets_*s + i];
    }


    // compute max shape d_K, so that tau = D_max * d_K
    FloatType get_max_shape() {
        return *std::max_element(shapes_sums_.begin(),shapes_sums_.end());
    }


    // compute total volume Omega
    FloatType get_tot_volume() {
        return std::accumulate(volumes_.begin(), volumes_.end(), 0.);
    }
    


    // read mesh + model and constructs internal objects
    void read_from_file(std::string const& filename_mesh, std::string const& filename_model) {
            
        std::ifstream file_mesh(filename_mesh);
        std::ifstream file_model(filename_model);
        
        try {
            std::string discard;

            file_mesh >> discard >> n_tets_;        // read
            std::cout << "N tets: " <<  n_tets_ << std::endl;
            std::getline(file_mesh, discard);       // skip \n
            std::getline(file_mesh, discard);       // skip headers
            volumes_.resize(n_tets_);       // each tet has a volume
            neighbors_.resize(n_tets_*4);   // each tet has (up to) 4 neighbors
            shapes_.resize(n_tets_*4);      // each connect to neighb has a shape
            shapes_sums_.resize(n_tets_);   // each tet has tot sum of neighb shapes

            file_model >> discard >> n_species_;     // how many species?
            std::cout << "N species: " <<  n_species_ << std::endl;
            file_model.close();
            mol_counts_.resize(n_tets_*n_species_); // each tet knows how many mol of each species it contains
            mol_counts_bucket_.resize(n_tets_); // bucket containing molecules received from diffusion
            
            
            for (IntType i=0; i<n_tets_; ++i) {
                file_mesh >> discard >> volume(i);      // read volume
                for (IntType j=0; j<4; ++j) 
                   file_mesh >> neighbor(i, j);         // read idx of neighbors
                for (IntType j=0; j<4; ++j) {
                   file_mesh >> shape(i, j);
                   if (neighbor(i,j) == -1)
                       shape(i, j) = 0.;
                }
                shape_sum(i) = 0;
                for (IntType j=0; j<4; ++j)  
                    shape_sum(i) += shape(i, j);
            }

        }
        catch(const std::exception& ex) {
            file_mesh.close();
            file_model.close();
            throw;
        }

    }        


    // distribute tot number of molecules on each tetrahedron
    void distribute_molecules(IntType species_idx, IntType n_molecules_tot) {
        IntType n_molecules_partial = 0; // molecules that have been placed until now
        FloatType tot_volume = get_tot_volume();
        std::random_device rd;
        std::mt19937 g(rd());
        for (IntType i=0; i<n_tets_; ++i) {
            FloatType volume_ratio =  volume(i) / tot_volume;
            IntType mols =  readi::rand_round<IntType, FloatType>(n_molecules_tot * volume_ratio, g);
            n_molecules_partial += mols;
            molecule_count(species_idx, i) = mols;
        }
        printf("Molecule Tot: %15d\n", n_molecules_tot);
        printf("Molecule Dis: %15d\n", n_molecules_partial);
        printf("Count error: \t%5.2f%%\n\n", 100*(double(n_molecules_tot-n_molecules_partial)/n_molecules_tot));
    }


    // run diffusion of molecules of species s
    void diffuse(FloatType tau, IntType s, FloatType diff_cnst) {

        std::random_device rd;
        std::mt19937 g(rd());
        // molecules of species s diffuse from each tetrahedron
        for (IntType i=0; i<n_tets_; ++i) {
            FloatType zeta_k = diff_cnst * shape_sum(i) * tau;
            // TODO: actually the max number of leaving molecules should be based on occupancy...
            IntType n_leaving_max = molecule_count(s, i);
            readi::binomial_distribution<IntType> binomial(n_leaving_max, zeta_k);
            IntType tot_leaving_mols = binomial(g); // how many molecules leave K
            molecule_count(s, i) -= tot_leaving_mols;
            FloatType shapes_partial = shape_sum(i);
            for (IntType j=0; j<3; ++j) { // select destinations with multinomial
                readi::binomial_distribution<IntType> binomial_destination(tot_leaving_mols, shape(i,j)/shapes_partial);
                IntType leaving_neighb = binomial_destination(g);
                tot_leaving_mols -= leaving_neighb;
                mol_counts_bucket_[neighbor(i,j)] += leaving_neighb;
                shapes_partial -= shape(i,j);
            }
            // last destination: all the remaining
            mol_counts_bucket_[neighbor(i,3)] += tot_leaving_mols;
        }

    }


    // empty buckets after diffusion
    void empty_buckets(IntType s) {
        for (IntType i=0; i<n_tets_; ++i) {
            molecule_count(s, i) += mol_counts_bucket_[i];
            mol_counts_bucket_[i] = 0;
        }
        return;
    }



private:
    IntType n_tets_;
    IntType n_species_;
    std::vector<FloatType> volumes_;
    std::vector<IntType> neighbors_;
    // the shape(i,j) of tetrahedron i w.r.t to neighbor j=0..3 represent this geometrical value:
    //      shape = surface_separating(i,j) / (volume(i) * distance_of_barycenters(i,j))
    std::vector<FloatType> shapes_;
    std::vector<FloatType> shapes_sums_;
    std::vector<FloatType> mol_counts_;
    std::vector<FloatType> mol_counts_bucket_; 

};

}

#endif// MAPP_READI_TETS_
