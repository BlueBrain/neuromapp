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

#include "Tets.h"
#include "rng_utils.h"

static constexpr double N_avogadro = 6.022140857e+23;

namespace readi {

template <class IntType, class FloatType>
class Model;

template <class IntType, class FloatType>
class Reac {
  public:
    using idx_type = IntType;
    using real_type = FloatType;

    // c-tor                                         // e.g. 2B + C --> A
    Reac(std::vector<std::string> const&
             spec_names,                      // e.g. {'A', 'B', 'C', 'D', 'E'}
         std::vector<std::string> const& lhs, // e.g. {'B', 'B', 'C'}
         std::vector<std::string> const& rhs, // e.g. {'A'}
         FloatType reaction_rate)
        : // the k_cst constant of reaction
          order_(lhs.size()), reaction_rate_(reaction_rate) {
        std::map<IntType, IntType>
            lhs_counts_map; // tells us that in LHS species idxed by 2 ("B")
                            // appears 2 times, species idxed by 3 appears 1
                            // time.
        std::map<IntType, IntType> rhs_counts_map;

        // 1. count species in LHS
        for (auto& sp : lhs) {
            IntType idx = std::find(spec_names.begin(), spec_names.end(), sp) -
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
            IntType idx = std::find(spec_names.begin(), spec_names.end(), sp) -
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
        std::set<IntType> lhs_rhs_idxes;
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
    inline void apply(IntType tet_idx,
                      readi::Tets<IntType, FloatType>& tets) const {
        for (IntType i = 0; i < upd_idxs_.size(); ++i) {
            assert(tets.molecule_count(upd_idxs_[i], tet_idx) >=
                   -upd_counts_[i]);
            tets.molecule_count(upd_idxs_[i], tet_idx) += upd_counts_[i];
        }
    }

    // compute scaled reaction rate for propensity
    inline FloatType compute_c_mu(FloatType reaction_rate,
                                  FloatType volume) const {
        FloatType volume_scaled = 1.e3 * volume * N_avogadro;
        return reaction_rate * std::pow(volume_scaled, -order_ + 1);
    }

    // compute propensity of this reaction on a tetrahedron
    FloatType
    compute_propensity(IntType tet_idx,
                       readi::Tets<IntType, FloatType> const& tets) const {
        FloatType propensity =
            compute_c_mu(reaction_rate_, tets.volume(tet_idx));
        for (int i = 0; i < lhs_idxs_.size(); ++i) {
            IntType X_i = tets.molecule_count(
                lhs_idxs_[i],
                tet_idx); // number of molecules of i-th species inside tet
            for (int j = 0; j < lhs_counts_[i]; ++j) {
                propensity *= X_i - j;
            }
        }
        return propensity;
    }

    friend class readi::Model<IntType, FloatType>; // why? because Model needs
                                                   // to access lhs and rhs

    inline std::vector<IntType> get_lhs_idxs() const { return lhs_idxs_; }

    inline std::vector<IntType> get_lhs_counts() const { return lhs_counts_; }

    inline std::vector<IntType> get_rhs_idxs() const { return rhs_idxs_; }

    inline std::vector<IntType> get_rhs_counts() const { return rhs_counts_; }

    inline std::vector<IntType> get_upd_idxs() const { return upd_idxs_; }

    inline std::vector<IntType> get_upd_counts() const { return upd_counts_; }

    inline FloatType get_rate() const { return reaction_rate_; }

  private:
    IntType order_;                   // order of reaction
    std::vector<IntType> lhs_idxs_;   // idxes of species in lhs
    std::vector<IntType> lhs_counts_; // counts of molecules per species in lhs
    std::vector<IntType> rhs_idxs_;   // idxes of species in rhs
    std::vector<IntType> rhs_counts_; // counts of molecules per species in rhs
    std::vector<IntType> upd_idxs_;   // idxes of species in update vector
    std::vector<IntType>
        upd_counts_; // counts of molecules per species in update vector
    FloatType reaction_rate_; // non-scaled reaction rate
};

} // namespace readi

#endif // MAPP_READI_REAC_
