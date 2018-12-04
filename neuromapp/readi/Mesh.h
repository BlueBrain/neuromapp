/*
 * Neuromapp - Mesh.h, Copyright (c), 2015,
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

#include <algorithm>
#include <cassert>
#include <fstream>
#include <numeric>
#include <string>

#include "rng_utils.h"

namespace readi {

template <typename IntType, typename FloatType, class SamplingPolicy>
class Mesh {
  public:
    using int_type = IntType;
    using float_type = FloatType;
    using idx_type = int_type;
    using real_type = float_type;
    static_assert(std::is_unsigned<int_type>::value,
                  "int_type should be an unsigned literal type");

    // get number of tetrahedra
    inline int_type get_n_tets() const { return n_tets_; }

    // access volume of i-th tetrahedron
    inline float_type& volume(int_type i) {
        assert(i < n_tets_);
        return volumes_[i];
    }
    inline float_type volume(int_type i) const {
        assert(i < n_tets_);
        return volumes_[i];
    }

    // access idx of j-th neighbor (j=0..3) of i-th tetrahedron
    inline int_type& neighbor(int_type i, int_type j) {
        assert(i < n_tets_);
        assert(j <= 3);
        return neighbors_[4 * i + j];
    }
    inline int_type neighbor(int_type i, int_type j) const {
        assert(i < n_tets_);
        assert(j <= 3);
        return neighbors_[4 * i + j];
    }

    // access shape of j-th neighbor (j=0..3) of i-th tetrahedron
    inline float_type& shape(int_type i, int_type j) {
        assert(i < n_tets_);
        assert(j <= 3);
        return shapes_[4 * i + j];
    }
    inline float_type shape(int_type i, int_type j) const {
        assert(i < n_tets_);
        assert(j <= 3);
        return shapes_[4 * i + j];
    }

    // access sum of neigh shapes of i-th tetrahedron
    inline float_type& shape_sum(int_type i) {
        assert(i < n_tets_);
        return shapes_sums_[i];
    }
    inline float_type shape_sum(int_type i) const {
        assert(i < n_tets_);
        return shapes_sums_[i];
    }

    // access molecule for s-th species in i-th tetrahedron
    inline int_type& molecule_count(int_type s, int_type i) {
        assert(s < n_species_);
        assert(i < n_tets_);
        return mol_counts_[n_tets_ * s + i];
    }
    inline int_type molecule_count(int_type s, int_type i) const {
        assert(s < n_species_);
        assert(i < n_tets_);
        return mol_counts_[n_tets_ * s + i];
    }

    // access occupancy of s-th species in i-th tetrahedron
    inline float_type& molecule_occupancy_count(int_type s, int_type i) {
        assert(s < n_species_);
        assert(i < n_tets_);
        return mol_occupancy_counts_[n_tets_ * s + i];
    }
    inline float_type molecule_occupancy_count(int_type s, int_type i) const {
        assert(s < n_species_);
        assert(i < n_tets_);
        return mol_occupancy_counts_[n_tets_ * s + i];
    }

    // access last update time of s-th species in i-th tetrahedron
    inline float_type& molecule_occupancy_last_update_time(int_type s,
                                                           int_type i) {
        assert(s < n_species_);
        assert(i < n_tets_);
        return mol_occupancy_lastupdtime_[n_tets_ * s + i];
    }
    inline float_type molecule_occupancy_last_update_time(int_type s,
                                                          int_type i) const {
        assert(s < n_species_);
        assert(i < n_tets_);
        return mol_occupancy_lastupdtime_[n_tets_ * s + i];
    }

    /// \brief compute max shape d_K, so that tau = D_max * d_K
    float_type get_max_shape() {
        return *std::max_element(shapes_sums_.begin(), shapes_sums_.end());
    }

    // compute total volume Omega
    float_type get_tot_volume() {
        return std::accumulate(volumes_.begin(), volumes_.end(), 0.);
    }

    // read mesh + model and constructs internal objects
    template <class Generator>
    void read_from_file(std::string const& filename_mesh,
                        std::string const& filename_model, Generator& g) {

        std::ifstream file_mesh(filename_mesh);
        std::ifstream file_model(filename_model);

        try {

            std::string discard;

            // --- [MESH_FILE] READ VOLUME + NEIGHBORS + SHAPES ---
            file_mesh >> discard >> n_tets_;  // read
            std::getline(file_mesh, discard); // skip '\n'
            std::getline(file_mesh, discard); // skip headers
            volumes_.resize(n_tets_);         // each tet has a volume
            neighbors_.resize(n_tets_ * 4); // each tet has (up to) 4 neighbors
            shapes_.resize(n_tets_ * 4); // each connect to neighb has a shape
            shapes_sums_.resize(
                n_tets_); // each tet has tot sum of neighb shapes

            for (int_type i = 0; i < n_tets_; ++i) {
                file_mesh >> discard >> volume(i); // read volume
                for (int_type j = 0; j < 4; ++j)
                    file_mesh >> neighbor(i, j); // read idxes of neighbors
                for (int_type j = 0; j < 4; ++j) {
                    file_mesh >> shape(i, j); // read shapes of neighbors
                    if (neighbor(i, j) == -1)
                        shape(i, j) = 0.;
                }
                shape_sum(i) = 0;
                for (int_type j = 0; j < 4; ++j)
                    shape_sum(i) += shape(i, j);
            }

            // --- [MODEL_FILE] READ N. SPECIES + N. INITIAL MOLECULES ---
            file_model >> discard >> n_species_; // read n. of species
            mol_counts_.resize(n_tets_ *
                               n_species_); // each tet knows how many mol of
                                            // each species it contains
            mol_occupancy_counts_.resize(n_tets_ * n_species_);
            mol_occupancy_lastupdtime_.resize(n_tets_ * n_species_);
            mol_counts_bucket_.resize(
                n_tets_); // bucket containing molecules received from diffusion
            std::getline(file_model, discard); // read \n
            std::getline(file_model, discard); // read description line
            int_type tot_mol_per_spec;
            for (int_type i = 0; i < n_species_; ++i) {
                file_model >> discard >>
                    tot_mol_per_spec; // read species name and total count
                distribute_molecules(
                    i, tot_mol_per_spec,
                    g); // distribute these molecules in the mesh
            }
            file_model.close();

            printf("----  TETS info "
                   "---------------------------------------------\n");
            printf("\t n. of tetrahedra :%10d\n", n_tets_);
            printf("\t n. of species    :%10d\n", n_species_);
            printf("-----------------------------------------------------------"
                   "--\n");

        } catch (const std::exception& ex) {
            file_mesh.close();
            file_model.close();
            throw;
        }
    }

    // distribute tot number of molecules on each tetrahedron, used at
    // initialization of mol counts
    template <class Generator>
    void distribute_molecules(int_type species_idx, int_type n_molecules_tot,
                              Generator& g) {
        // TODO: actually the distribution of molecules should be done by
        // implementing an adjusted Pareto sampler in two phases:
        //  1. attribute to each tet the relative rounded down fraction of
        //  molecules
        //  2. attribute the remaining molecules through Pareto sampling
        int_type n_molecules_partial =
            0; // molecules that have been placed until now
        float_type tot_volume = get_tot_volume();
        for (int_type i = 0; i < n_tets_; ++i) {
            float_type volume_ratio = volume(i) / tot_volume;
            int_type mols = readi::rand_round<int_type, float_type>(
                n_molecules_tot * volume_ratio, g);
            n_molecules_partial += mols;
            molecule_count(species_idx, i) = mols;
        }
        //        printf("---- Distribution of molecules for species %d
        //        ---------------\n", species_idx); float_type err_distr =
        //        100*(double(n_molecules_tot-n_molecules_partial)/n_molecules_tot);
        //        printf("\t Theoretical:%d,  Distributed:%d, Error:%5.2f%%\n",
        //        n_molecules_tot, n_molecules_partial, err_distr);
        //        printf("-------------------------------------------------------------\n");
    }

    // add to buckets during diffusion
    inline void add_to_bucket(int_type tet_idx, int_type neighb_idx,
                              int_type diffusing_count) {
        mol_counts_bucket_[neighbor(tet_idx, neighb_idx)] += diffusing_count;
    }

    // empty buckets after diffusion
    void empty_buckets(int_type s) {
        for (int_type i = 0; i < n_tets_; ++i) {
            molecule_count(s, i) += mol_counts_bucket_[i];
            mol_counts_bucket_[i] = 0;
        }
    }

  private:
    int_type n_tets_;
    int_type n_species_;
    std::vector<float_type> volumes_;
    std::vector<int_type> neighbors_;
    // the shape(i,j) of tetrahedron i w.r.t to neighbor j=0..3 represent this
    // geometrical value:
    //      shape = surface_separating(i,j) / (volume(i) *
    //      distance_of_barycenters(i,j))
    std::vector<float_type> shapes_;
    std::vector<float_type> shapes_sums_;
    std::vector<int_type> mol_counts_;
    std::vector<int_type> mol_counts_bucket_;
    std::vector<float_type> mol_occupancy_counts_;
    std::vector<float_type> mol_occupancy_lastupdtime_;
};

/**
 * Mesh additional methods and data for SSA algorithm
 */
template <class IntType, class FloatType>
class SSAMesh : public Mesh<IntType, FloatType, SSAMesh<IntType, FloatType>> {
  public:
    using int_type = IntType;
    using float_type = FloatType;

    void set_size(IntType n_reacs, IntType n_tets) {
        propensity_values_.resize(n_reacs * n_tets,
                                  0); // initialize all propensities to 0
    }

    // access propensity value of r-th reaction inside i-th tetrahedron
    template <typename ModelType>
    inline FloatType& propensity_val(const ModelType& model, IntType r,
                                     IntType i) noexcept {
        assert(i < this->get_n_tets());
        return propensity_values_[propensity_idx(model, r, i)];
    }
    template <typename ModelType>
    inline FloatType propensity_val(const ModelType& model, IntType r,
                                    IntType i) const noexcept {
        assert(i < this->get_n_tets());
        return propensity_values_[propensity_idx(r, i)];
    }

    template <typename ModelType>
    inline IntType propensity_idx(ModelType& model, IntType r, IntType i) const
        noexcept {
        assert(i < this->get_n_tets());
        return model.get_n_reactions() * i + r;
    }

    FloatType propensity_values(IntType index) const {
        return propensity_values_[index];
    }

  private:
    /// \brief vector with all propensity values
    std::vector<FloatType> propensity_values_;
};

} // namespace readi

#endif // MAPP_READI_TETS_
