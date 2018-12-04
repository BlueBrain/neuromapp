/*
 * Neuromapp - CompRej.h, Copyright (c), 2015,
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
 * @file neuromapp/readi/CompRej.h
 * \brief Composition-Rejection class for Readi Miniapp
 */

#ifndef MAPP_READI_COMPREJ_
#define MAPP_READI_COMPREJ_

#include "rng_utils.h"
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <unordered_set>
#include <vector>

#include "Mesh.h"

namespace readi {

template <class Mesh>
class CompRej {
  public:
    using IntType = typename Mesh::int_type;
    using FloatType = typename Mesh::float_type;
    static_assert(std::is_unsigned<IntType>::value,
                  "IntType may be an unsigned literal type");
    using idx_type = IntType;
    using real_type = FloatType;

    CompRej(Mesh& mesh) : a0_(0.), mesh_(mesh) {}

    struct CompRejGroup {
        CompRejGroup() : ag_(0.) {}

        std::unordered_set<IntType>
            propensity_idxes; // set of propensities within this range
        inline FloatType get_group_propensity() const noexcept { return ag_; }
        inline IntType size() const noexcept { return propensity_idxes.size(); }
        FloatType ag_;
    };

    // set n. of reacs and tetrahedra
    void set_size(IntType n_reacs, IntType n_tets) {
        mesh_.set_size(n_reacs, n_tets);
        cr_groups_pos_.emplace_back(new CompRejGroup);
        cr_groups_neg_.emplace_back(new CompRejGroup);
    }

    // get total propensity
    inline FloatType get_total_propensity() const noexcept { return a0_; }

    // CATEGORICAL SAMPLING TO CHOOSE GROUP OF PROPENSITIES
    template <class Generator>
    IntType select_group(Generator& g) const {
        FloatType unif = (g() - g.min()) / double(g.max() - g.min());
        FloatType discr = unif * get_total_propensity();
        FloatType part_sum = 0.;
        for (auto i = cr_groups_neg_.size() - 1; i >= 0; --i) {
            part_sum += cr_groups_neg_[i]->get_group_propensity();
            if (discr <= part_sum)
                return -i - 1;
        }
        for (int i = 0; i < cr_groups_pos_.size() - 1; ++i) {
            part_sum += cr_groups_pos_[i]->get_group_propensity();
            if (discr <= part_sum)
                return i;
        }
        return cr_groups_pos_.size() - 1; // last remaining group
    }

    // REJECTION SAMPLING TO CHOOSE IDX WITHIN A GROUP (NEGATIVE GROUPS ALLOWED)
    template <class Genenerator>
    IntType select_propensity_in_group(Genenerator& g,
                                       IntType group_idx) const {
        // REJECTION SAMPLING
        // - would like to sample X ~ f  [f is a p.d.f.]
        // - we know how to sample from Y ~ g [g is a p.d.f.]
        // - we know a value M s.t. f(x) < M g(x)  [for every x]
        // ALGORITHM
        // 1. sample x ~ g, u ~ Unif(0,1)
        // 2. if (f(x) > M g(x) u) accept x [as x ~ f]
        //    else reject and goto 1.
        // IN OUR CASE
        // - f = Categorical{a_1/a_g, ..., a_N/a_g}
        // - g = Uniform{1/N, ..., 1/N}
        // - M = N * uppb/a_g  [uppb = upper bound of range (2^{i}, 2^{i+1})]
        // ---> if (a_j/a_g > N * uppb/a_g * 1/N) <==> if (a_j > uppb)
        IntType uppbd = (group_idx < 0) ? (std::pow(2.0, group_idx))
                                        : (std::pow(2.0, group_idx + 1));
        CompRejGroup& group = (group_idx < 0)
                                  ? (*cr_groups_neg_[-group_idx - 1])
                                  : (*cr_groups_pos_[group_idx]);
        while (true) {
            FloatType u1 = (g() - g.min()) / double(g.max() - g.min());
            IntType local_idx =
                std::floor(u1 * group.size()); // index within the group
            FloatType u2 = (g() - g.min()) / double(g.max() - g.min());
            IntType global_idx =
                *std::next(group.propensity_idxes.begin(),
                           local_idx); // global index of propensity
            FloatType a_j = mesh_.propensity_values(global_idx);
            if (a_j > uppbd * u2)
                return global_idx;
        }
    }

    // COMPOSITION-REJECTION ALGORITHM
    template <class Generator, class ModelType>
    void select_next_reaction(Generator& g, ModelType& model, IntType& r,
                              IntType& i) const {
        IntType group_idx = select_group(g);
        IntType global_idx = select_propensity_in_group(g, group_idx);
        auto div_result = std::div(static_cast<long>(global_idx),
                                   static_cast<long>(model.get_n_reactions()));
        i = div_result.quot;
        r = div_result.rem;
    }

    // update the propensity relative to r-th reaction inside i-th tetrahedron
    template <typename ModelType>
    void update_propensity(const ModelType& model, const IntType r,
                           const IntType i, FloatType new_prop) {
        assert(i < mesh_.get_n_tets());
        auto const propen_val = mesh_.propensity_val(model, r, i);
        assert(propen_val >= 0.0);
        assert(new_prop >= 0.0);

        // CASE 0: OLD_GROUP=-IFTY / NEW_GROUP=-IFTY
        // CASE 0.1: OLD == NEW == 0
        if (propen_val == 0.0 && new_prop == 0.0)
            return;
        // CASE 0.2: OLD == 0, NEW != 0
        if (propen_val == 0.0) {
            IntType new_group_idx = std::logb(new_prop);
            if (new_group_idx >= 0) {
                // B- handle case where not enuough groups
                if (new_group_idx >= cr_groups_pos_.size()) {
                    cr_groups_pos_.reserve(new_group_idx + 1);
                    while (cr_groups_pos_.size() < new_group_idx + 1)
                        cr_groups_pos_.emplace_back(new CompRejGroup);
                }
                // C- insert idx of a_j in new group + update value of a_g in
                // new group
                cr_groups_pos_[new_group_idx]->propensity_idxes.insert(
                    mesh_.propensity_idx(model, r, i));
                cr_groups_pos_[new_group_idx]->ag_ += new_prop;
            } else {
                new_group_idx = -new_group_idx - 1;
                // B- handle case where not enuough groups
                if (new_group_idx >= cr_groups_neg_.size()) {
                    cr_groups_neg_.reserve(new_group_idx + 1);
                    while (cr_groups_neg_.size() < new_group_idx + 1)
                        cr_groups_neg_.emplace_back(new CompRejGroup);
                }
                // C- insert idx of a_j in new group + update value of a_g in
                // new group
                cr_groups_neg_[new_group_idx]->propensity_idxes.insert(
                    mesh_.propensity_idx(model, r, i));
                cr_groups_neg_[new_group_idx]->ag_ += new_prop;
            }
            // D- update value of a_0
            // E- update value of a_j in the vector of propensities
            a0_ += new_prop;
            mesh_.propensity_val(model, r, i) = new_prop;
            return;
        }
        // CASE 0.3: OLD != 0, NEW == 0
        if (new_prop == 0.0) {
            IntType old_group_idx = std::logb(propen_val);
            // group where a_j was until now
            // A- remove idx of a_j in old_group + update value of a_g in
            // old_group
            if (old_group_idx >= 0) {
                cr_groups_pos_[old_group_idx]->propensity_idxes.erase(
                    mesh_.propensity_idx(model, r, i));
                if (cr_groups_pos_[old_group_idx]
                        ->propensity_idxes.empty()) // if no more idxes in
                                                    // group, set exactely to 0.
                    cr_groups_pos_[old_group_idx]->ag_ = 0.;
                else
                    cr_groups_pos_[old_group_idx]->ag_ -= propen_val;
            } else {
                old_group_idx = -old_group_idx - 1;
                cr_groups_neg_[old_group_idx]->propensity_idxes.erase(
                    mesh_.propensity_idx(model, r, i));
                if (cr_groups_neg_[old_group_idx]
                        ->propensity_idxes.empty()) // if no more idxes in
                                                    // group, set exactely to 0.
                    cr_groups_neg_[old_group_idx]->ag_ = 0.;
                else
                    cr_groups_neg_[old_group_idx]->ag_ -= propen_val;
            }
            // D- update value of a_0
            // E- update value of a_j in the vector of propensities
            a0_ -= propen_val;
            mesh_.propensity_val(model, r, i) = 0;
            return;
        }

        // If we arrived until here, then OLD!=0, NEW!=0

        IntType old_group_idx =
            std::logb(propen_val); // group where a_j was until now
        IntType new_group_idx = std::logb(new_prop); // group where a_j will go

        // CASE 1: A_J CHANGES VALUE BUT STAYS IN SAME GROUP
        // A- update value of a_g
        if (old_group_idx == new_group_idx) {
            if (old_group_idx >= 0)
                cr_groups_pos_[old_group_idx]->ag_ += (new_prop - propen_val);
            else
                cr_groups_neg_[-old_group_idx - 1]->ag_ +=
                    (new_prop - propen_val);
        }
        // CASE 2: A_J CHANGES VALUE AND ALSO CHANGES GROUP
        // A- remove idx of a_j in old_group + update value of a_g in old_group
        // B- handle case where not enuough groups
        // C- insert idx of a_j in new group + update value of a_g in new group
        else {
            // A- remove idx of a_j in old_group + update value of a_g in
            // old_group
            if (old_group_idx >= 0) {
                cr_groups_pos_[old_group_idx]->propensity_idxes.erase(
                    mesh_.propensity_idx(model, r, i));
                if (cr_groups_pos_[old_group_idx]
                        ->propensity_idxes.empty()) // if no more idxes in
                                                    // group, set exactely to 0.
                    cr_groups_pos_[old_group_idx]->ag_ = 0.;
                else
                    cr_groups_pos_[old_group_idx]->ag_ -= propen_val;
            } else {
                old_group_idx = -old_group_idx - 1;
                cr_groups_neg_[old_group_idx]->propensity_idxes.erase(
                    mesh_.propensity_idx(model, r, i));
                if (cr_groups_neg_[old_group_idx]
                        ->propensity_idxes.empty()) // if no more idxes in
                                                    // group, set exactely to 0.
                    cr_groups_neg_[old_group_idx]->ag_ = 0.;
                else
                    cr_groups_neg_[old_group_idx]->ag_ -= propen_val;
            }

            if (new_group_idx >= 0) {
                // B- handle case where not enuough groups
                if (new_group_idx >= cr_groups_pos_.size()) {
                    cr_groups_pos_.reserve(new_group_idx +
                                           1); // or should we make this O(1)?
                    while (cr_groups_pos_.size() < new_group_idx + 1)
                        cr_groups_pos_.emplace_back(new CompRejGroup);
                }
                // C- insert idx of a_j in new group + update value of a_g in
                // new group
                cr_groups_pos_[new_group_idx]->propensity_idxes.insert(
                    mesh_.propensity_idx(model, r, i));
                cr_groups_pos_[new_group_idx]->ag_ += new_prop;
            } else {
                new_group_idx = -new_group_idx - 1;
                // B- handle case where not enuough groups
                if (new_group_idx >= cr_groups_neg_.size()) {
                    cr_groups_neg_.reserve(new_group_idx +
                                           1); // or should we make this O(1)?
                    while (cr_groups_neg_.size() < new_group_idx + 1)
                        cr_groups_neg_.emplace_back(new CompRejGroup);
                }
                // C- insert idx of a_j in new group + update value of a_g in
                // new group
                cr_groups_neg_[new_group_idx]->propensity_idxes.insert(
                    mesh_.propensity_idx(model, r, i));
                cr_groups_neg_[new_group_idx]->ag_ += new_prop;
            }
        }

        // CASE 1 and 2:
        // D- update value of a_0
        // E- update value of a_j in the vector of propensities
        a0_ += (new_prop - propen_val);
        mesh_.propensity_val(model, r, i) = new_prop;
    }

  private:
    /// \brief total propensity
    FloatType a0_;
    /// \brief i-th group, i=0.., covers range (2^{i}, 2^{i+1})
    std::vector<std::unique_ptr<CompRejGroup>> cr_groups_pos_;
    /// \brief i-th group, i=0.., covers range (2^{-i-1}, 2^{-i})
    std::vector<std::unique_ptr<CompRejGroup>> cr_groups_neg_;
    Mesh& mesh_;
};

} // namespace readi

#endif // MAPP_READI_COMPREJ_
