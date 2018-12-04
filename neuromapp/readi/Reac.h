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
 * @file neuromapp/readi/Reac.h
 * \brief Reactio for Readi Miniapp
 */

#ifndef MAPP_READI_REAC_
#define MAPP_READI_REAC_

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "Mesh.h"
#include "rng_utils.h"

static constexpr double N_avogadro = 6.022140857e+23;

namespace readi {

// template <class IntType, class FloatType>
// class Model;

template <class IntType, class FloatType>
class Reac {
  public:
    using int_type = IntType;
    using float_type = FloatType;

    static_assert(std::is_unsigned<int_type>::value,
                  "int_type should be an unsigned literal type");
    using idx_type = int_type;
    using real_type = float_type;

    // c-tor                                         // e.g. 2B + C --> A
    Reac(const std::vector<std::string>&
             spec_names,                      // e.g. {'A', 'B', 'C', 'D', 'E'}
         std::vector<std::string> const& lhs, // e.g. {'B', 'B', 'C'}
         std::vector<std::string> const& rhs, // e.g. {'A'}
         float_type reaction_rate)
        : // the k_cst constant of reaction
          order_(lhs.size()), reaction_rate_(reaction_rate) {
        std::map<int_type, int_type>
            lhs_counts_map; // tells us that in LHS species indexed by 2 ("B")
                            // appears 2 times, species indexed by 3 appears 1
                            // time.
        std::map<int_type, int_type> rhs_counts_map;

        /// TODO TCL factor 2 sections of code below
        // 1. count species in LHS
        for (auto& sp : lhs) {
            int_type idx = std::find(spec_names.begin(), spec_names.end(), sp) -
                           spec_names.begin();
            if (idx == spec_names.size())
                throw std::range_error("species " + sp +
                                       " from lhs is undeclared!");
            else {
                if (!lhs_counts_map.count(idx))
                    lhs_counts_map[idx] = 1;
                else
                    ++lhs_counts_map[idx];
            }
        }

        // 2. count species in RHS
        for (auto& sp : rhs) {
            int_type idx = std::find(spec_names.begin(), spec_names.end(), sp) -
                           spec_names.begin();
            if (idx == spec_names.size())
                throw std::range_error("species " + sp +
                                       " from rhs is undeclared!");
            else {
                if (!rhs_counts_map.count(idx))
                    rhs_counts_map[idx] = 1;
                else
                    ++rhs_counts_map[idx];
            }
        }

        // 3. count species in update vector
        // 3.1. create a set with all the idxes of the species that appear at
        // least in one of the two maps
        std::set<int_type> lhs_rhs_idxes;
        for (auto& el : lhs_counts_map)
            lhs_rhs_idxes.insert(el.first);
        for (auto& el : rhs_counts_map)
            lhs_rhs_idxes.insert(el.first);
        // 3.2. for each idx in either rhs or lhs, check if it belongs to lhs
        // only, rhs only, or both
        for (auto idx : lhs_rhs_idxes) {
            // if the idx is on both sides AND the difference is not 0, then add
            // to update vector
            if (lhs_counts_map.count(idx) && rhs_counts_map.count(idx)) {
                if (lhs_counts_map[idx] != rhs_counts_map[idx]) {
                    upd_idxs_.push_back(idx);
                    upd_counts_.push_back(rhs_counts_map[idx] -
                                          lhs_counts_map[idx]);
                }
            } else {
                // if the idx is only on the LHS...
                if (!rhs_counts_map.count(idx)) {
                    upd_idxs_.push_back(idx);
                    upd_counts_.push_back(-lhs_counts_map[idx]);
                }
                // if the idx is only on the RHS
                else {
                    upd_idxs_.push_back(idx);
                    upd_counts_.push_back(rhs_counts_map[idx]);
                }
            }
        }

        // 4. finally, transform LHS and RHS from maps to vectors
        for (auto& el : lhs_counts_map) {
            lhs_idxs_.push_back(el.first);
            lhs_counts_.push_back(el.second);
        }
        for (auto& el : rhs_counts_map) {
            rhs_idxs_.push_back(el.first);
            rhs_counts_.push_back(el.second);
        }
    }

    // apply this reaction on a tetrahedron

    template <typename MeshType>
    inline void apply(int_type tet_idx, MeshType& tets) const {
        static_assert(
            std::is_same<typename MeshType::int_type, int_type>::value,
            "Invalid mesh type");
        static_assert(
            std::is_same<typename MeshType::float_type, float_type>::value,
            "Invalid mesh type");
        for (auto i = 0u; i < upd_idxs_.size(); ++i) {
            assert(tets.molecule_count(upd_idxs_[i], tet_idx) >=
                   -upd_counts_[i]);
            tets.molecule_count(upd_idxs_[i], tet_idx) += upd_counts_[i];
        }
    }

    // compute scaled reaction rate for propensity
    [[gnu::pure]] inline float_type compute_c_mu(float_type reaction_rate,
                                                 float_type volume) const {
        float_type volume_scaled = 1.e3 * volume * N_avogadro;
        return reaction_rate * std::pow(volume_scaled, -order_ + 1);
    }

    // compute propensity of this reaction on a tetrahedron
    template <typename MeshType>
    float_type compute_propensity(int_type tet_idx,
                                  const MeshType& tets) const {
        static_assert(
            std::is_same<typename MeshType::int_type, int_type>::value,
            "Invalid mesh type");
        static_assert(
            std::is_same<typename MeshType::float_type, float_type>::value,
            "Invalid mesh type");
        float_type propensity =
            compute_c_mu(reaction_rate_, tets.volume(tet_idx));
        for (int i = 0; i < lhs_idxs_.size(); ++i) {
            int_type X_i = tets.molecule_count(
                lhs_idxs_[i],
                tet_idx); // number of molecules of i-th species inside tet
            for (int j = 0; j < lhs_counts_[i]; ++j) {
                propensity *= X_i - j;
            }
        }
        return propensity;
    }

    inline const std::vector<int_type>& get_lhs_idxs() const noexcept {
        return lhs_idxs_;
    }

    inline const std::vector<int_type>& get_lhs_counts() const noexcept {
        return lhs_counts_;
    }

    inline const std::vector<int_type>& get_rhs_idxs() const noexcept {
        return rhs_idxs_;
    }

    inline const std::vector<int_type>& get_rhs_counts() const noexcept {
        return rhs_counts_;
    }

    inline const std::vector<int_type>& get_upd_idxs() const noexcept {
        return upd_idxs_;
    }

    inline const std::vector<int_type>& get_upd_counts() const noexcept {
        return upd_counts_;
    }

    inline float_type get_rate() const { return reaction_rate_; }

  private:
    /// TODO TCL all std::vector member variables below are constant
    /// and thus should be \a const

    /// \brief order of reaction
    const int_type order_;
    /// \brief idxes of species in lhs
    std::vector<int_type> lhs_idxs_;
    /// \brief counts of molecules per species in lhs
    std::vector<int_type> lhs_counts_;
    /// \brief idxes of species in rhs
    std::vector<int_type> rhs_idxs_;
    /// \brief counts of molecules per species in rhs
    std::vector<int_type> rhs_counts_;
    /// \brief idxes of species in update vector
    std::vector<int_type> upd_idxs_;
    /// \brief counts of molecules per species in update vector
    std::vector<int_type> upd_counts_;
    /// \brief non-scaled reaction rate
    const float_type reaction_rate_;
};

} // namespace readi

#endif // MAPP_READI_REAC_
