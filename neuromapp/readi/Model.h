/*
 * Neuromapp - Reac.h, Copyright (c), 2015,
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
 * @file neuromapp/readi/Model.h
 * \brief Reactions, Diffusions, and Species Model for Readi Miniapp
 */

#ifndef MAPP_READI_MODEL_
#define MAPP_READI_MODEL_

#include <algorithm>

#include "Reac.h"

namespace readi {

template <class IntType, class FloatType>
class Model {
  public:
    using int_type = IntType;
    using float_type = FloatType;
    using reaction_type = Reac<int_type, float_type>;
    static_assert(std::is_unsigned<int_type>::value,
                  "int_type should be an unsigned literal type");

    /// \return number of species
    inline float_type get_n_species() const noexcept { return n_species_; }

    /// \return number of reactions
    inline float_type get_n_reactions() const noexcept { return n_reactions_; }

    /// \return species name
    inline const std::string& get_species_name(int_type s) const {
        return species_names_[s];
    }

    /**
     * \brief applies r-th reaction to i-th tetrahedron
     * \param r reaction identifier
     * \param i tetrahedron identifier
     * \param tets mesh
     */
    template <class MeshType>
    inline void apply_reaction(int_type r, int_type i, MeshType& tets) const {
        static_assert(
            std::is_same<typename MeshType::int_type, int_type>::value,
            "Invalid mesh type");
        static_assert(
            std::is_same<typename MeshType::float_type, float_type>::value,
            "Invalid mesh type");
        reactions_[r].apply(i, tets);
    }

    /// \brief computes propensity of r-th reaction in i-th tetrahedron
    template <class MeshType>
    inline float_type compute_reaction_propensity(int_type r, int_type i,
                                                  MeshType& tets) const {
        static_assert(
            std::is_same<typename MeshType::int_type, int_type>::value,
            "Invalid mesh type");
        static_assert(
            std::is_same<typename MeshType::float_type, float_type>::value,
            "Invalid mesh type");
        return reactions_[r].compute_propensity(i, tets);
    }

    // return max diffusion coefficient
    inline float_type get_max_diff() const {
        /// TODO TCL diffusion coefficients are constant, and thus should be
        /// computed only once
        return *std::max_element(diffusion_coefficients_.begin(),
                                 diffusion_coefficients_.end());
    }

    // access diffusion constant of s-th species
    inline float_type diffusion_coeff(int_type s) const {
        assert(s < n_species_);
        return diffusion_coefficients_[s];
    }

    // read from file the model data (species names + reactions + diffusions)
    // TODO TCL make it a static class method to have many const member
    // variables
    void read_from_file(std::string const& filename_model) {
        std::ifstream f;

        try {
            f.open(filename_model);
            std::string discard;

            // --- READ SPECIES NAMES ---
            f >> discard >> n_species_; // read how many species
            species_names_.resize(n_species_);
            diffusion_coefficients_.resize(n_species_);
            std::getline(f, discard); // skip '\n'
            std::getline(f, discard); // skip description line
            // int_type tot_mol_per_spec;
            for (int_type i = 0; i < n_species_; ++i) {
                f >> species_names_[i] >>
                    discard; // read species name and skip total count
            }
            std::getline(f, discard); // skip '\n'

            // --- READ REACTIONS ---
            std::getline(f, discard);     // skip empty line
            f >> discard >> n_reactions_; // read how many reactions
            std::getline(f, discard);     // skip '\n'
            std::getline(f, discard);     // skip description
            reaction_coefficients_.resize(n_reactions_);
            for (int_type i = 0; i < n_reactions_; ++i) {
                std::getline(f, discard); // read reaction lhs, rhs, and coeff
                std::vector<std::string> lhs;
                std::vector<std::string> rhs;
                double k_reac;
                std::stringstream sstr(discard);
                std::string buf;
                while (sstr >> buf) {
                    if (buf == "-")
                        break;
                    lhs.push_back(buf);
                }
                while (sstr >> buf) {
                    if (buf == "-")
                        break;
                    rhs.push_back(buf);
                }
                sstr >> k_reac;
                reactions_.emplace_back(species_names_, lhs, rhs,
                                        k_reac); // create new reaction with
                                                 // such lhs, rhs, and coeff
            }

            // --- READ DIFFUSIONS ---
            std::getline(f, discard); // skip empty line
            std::getline(f, discard); // skip description line
            for (int_type s = 0; s < n_species_; ++s) {
                f >> discard >>
                    diffusion_coeff_(s); // read coefficients of diffusion
            }

            // --- COMPUTE DEPENDENCY GRAPHS FOR REACTIONS AND DIFFUSIONS
            compute_dependencies_of_reactions();
            compute_dependencies_of_diffusions();

        } catch (const std::exception& ex) {
            f.close();
            throw;
        }

        printf(
            "----  MODEL info --------------------------------------------\n");
        printf("\t n. of species   :%10d\n", n_species_);
        printf("\t n. of reactions :%10d\n", n_reactions_);
        printf(
            "-------------------------------------------------------------\n");

        // assert diffusion coefficients are correctly read
        for (auto coeff : diffusion_coefficients_) {
            assert(coeff);
        }
    }

    // compute graph of dependencies for reactions-reactions
    void compute_dependencies_of_reactions() {
        for (int_type r = 0; r < n_reactions_; ++r) {
            std::vector<int_type> dependencies;
            for (int_type r_dep = 0; r_dep < n_reactions_; ++r_dep) {
                // r_dep is a reaction that may be affected by r.
                // To decide if r_dep is really affected, we check if any of the
                // elements in the upd_idxs of r is present in the lhs of r_dep
                if (std::find_first_of(reactions_[r_dep].get_lhs_idxs().begin(),
                                       reactions_[r_dep].get_lhs_idxs().end(),
                                       reactions_[r].get_upd_idxs().begin(),
                                       reactions_[r].get_upd_idxs().end()) !=
                    reactions_[r_dep].get_lhs_idxs().end())
                    dependencies.push_back(r_dep);
            }
            reac_idxs_affected_by_r_.push_back(dependencies);
        }
    }

    // compute graph of dependencies for diffusion-reactions
    void compute_dependencies_of_diffusions() {
        for (int_type s = 0; s < n_species_; ++s) {
            std::vector<int_type> dependencies = {};
            for (int_type r_dep = 0; r_dep < n_reactions_; ++r_dep) {
                if (std::find(reactions_[r_dep].get_lhs_idxs().begin(),
                              reactions_[r_dep].get_lhs_idxs().end(),
                              s) != reactions_[r_dep].get_lhs_idxs().end())
                    dependencies.push_back(r_dep);
            }
            reac_idxs_affected_by_d_.push_back(dependencies);
        }
    }

    // Get vector with all reaction idxs affected by r-th reaction
    std::vector<int_type>& get_reaction_dependencies(int_type r) {
        return reac_idxs_affected_by_r_[r];
    }

    // Get vector with all diffusion idxs affected by s-th reaction
    std::vector<int_type>& get_diffusion_dependencies(int_type s) {
        return reac_idxs_affected_by_d_[s];
    }

    // Get update idxs for r-th reaction
    std::vector<int_type> get_update_idxs(int_type r) {
        return reactions_[r].get_upd_idxs();
    }

  private:
    // access diffusion constant of s-th species
    inline float_type& diffusion_coeff_(int_type s) {
        assert(s < n_species_);
        return diffusion_coefficients_[s];
    }

    int_type n_species_;
    int_type n_reactions_;
    std::vector<std::string> species_names_;
    std::vector<float_type> diffusion_coefficients_;
    std::vector<float_type> reaction_coefficients_;
    std::vector<reaction_type> reactions_;
    /// TODO TCL improve data locality by having only one std::vector
    std::vector<std::vector<int_type>>
        reac_idxs_affected_by_r_; // the r-th element of this vector contains a
                                  // vector with all the reaction idxs affected
                                  // by the r-th reaction

    /// TODO TCL improve data locality by having only one std::vector
    std::vector<std::vector<int_type>>
        reac_idxs_affected_by_d_; // the s-th element of this vector contains a
                                  // vector with all the reaction idxs affected
                                  // by the d-th diffusion
};

} // namespace readi

#endif // MAPP_READI_MODEL_
