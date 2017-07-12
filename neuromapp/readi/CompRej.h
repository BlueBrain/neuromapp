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
#include <vector>
#include <set>
#include <memory>
#include <iterator>


namespace readi {


template<class IntType, class FloatType>
class CompRej{
public:
    using idx_type = IntType;
    using real_type = FloatType;

    struct CompRejGroup {
        std::set<IntType> propensity_idxes;            // set of propensities within this range
        inline IntType get_group_propensity() const {return ag_;}
        FloatType ag_;
    };


    // c-tor
    CompRej(IntType n_reacs, IntType n_tets) :
        n_reacs_(n_reacs),
        n_tets_(n_tets),
        propensity_values_(n_reacs_*n_tets_, 1) // initialize all propensities to 1
    {
        // TODO: here we should call update_propensity on each reac, on each tet
    }


    // access propensity value of r-th reaction inside i-th tetrahedron
    inline FloatType& propensity_val(IntType r, IntType i) {
        assert(r>=0 && r<n_reacs_);
        assert(i>=0 && i<n_tets_);
        return propensity_values_[propensity_idx(r, i)];
    }
    inline FloatType propensity_val(IntType r, IntType i) const {
        assert(r>=0 && r<n_reacs_);
        assert(i>=0 && i<n_tets_);
        return propensity_values_[propensity_idx(r, i)];
    }


    inline IntType propensity_idx(IntType r, IntType i) const {
        assert(r>=0 && r<n_reacs_);
        assert(i>=0 && i<n_tets_);
        return n_reacs_*i + r;
    }


    // get total propensity
    inline FloatType get_total_propensity() {
        return a0_;
    }


    // CATEGORICAL SAMPLING TO CHOOSE GROUP OF PROPENSITIES
    template <class Generator>
    IntType select_group(Generator& g) const {
        const FloatType unif = (g() - g.min())/double(g.max() - g.min());
        const FloatType discr = unif * get_total_propensity();
        FloatType part_sum = 0.;
        for (int i=cr_groups_neg_.size()-1; i>=0; --i){
            part_sum += cr_groups_neg_[i]->get_group_propensity();
            if (discr <= part_sum)
                return -i-1;
        }
        for (int i=0; i<cr_groups_pos_.size()-1; ++i){
            part_sum += cr_groups_pos_[i]->get_group_propensity();
            if (discr <= part_sum)
                return i;
        }
        return cr_groups_pos_.size()-1; // last remaining group
    }


    // REJECTION SAMPLING TO CHOOSE IDX WITHIN A GROUP (NEGATIVE GROUPS ALLOWED)
    template <class Genenerator>
    IntType select_propensity_in_group(Genenerator &g, IntType group_idx) const {
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
        IntType uppbd = (group_idx<0)?(std::pow(2.0, group_idx)):(std::pow(2.0, group_idx+1));
        CompRejGroup& group = (group_idx<0)?(*cr_groups_neg_[group_idx+1]):(cr_groups_pos_[group_idx]);
        while(true) {
            FloatType u1 = (g() - g.min())/double(g.max() - g.min());
            IntType local_idx = std::floor(u1 * group.size());            // index within the group
            FloatType u2 = (g() - g.min())/double(g.max() - g.min());
            IntType global_idx = *std::next(group.begin(), local_idx);            // global index of propensity
            FloatType a_j = propensity_values_[global_idx];
            if (a_j > uppbd * u2)
                return global_idx;
        }

    }



    // insert a propensity in a group, given the idx of the group (negative values allowed here!)
    void insert_propensity_in_group(IntType prop_idx, IntType group_idx) {
        FloatType prop_val = propensity_values_[prop_idx];
        assert(group_idx == std::logb(prop_val));
        if (group_idx>=0) {
            if (group_idx >= cr_groups_pos_->size()) {
                cr_groups_pos_.reserve(group_idx);
                IntType n_groups_to_add = group_idx - cr_groups_pos_->size() + 1;
                for (IntType i=0; i<n_groups_to_add; ++i)
                    cr_groups_pos_.emplace_back(new CompRejGroup);
            }
            cr_groups_pos_[group_idx]->propensity_idxes.insert(prop);
            cr_groups_pos_[group_idx]->ag_ += prop_val;                             // update group propensity
        }
        else {
            ++group_idx;
            if (group_idx >= cr_groups_neg_->size()) {
                cr_groups_neg_.reserve(group_idx);
                IntType n_groups_to_add = group_idx - cr_groups_pos_->size() + 1;
                for (IntType i=0; i<n_groups_to_add; ++i)
                    cr_groups_neg_.emplace_back(new CompRejGroup);
            }
            cr_groups_neg_[group_idx]->propensity_idxes.insert(prop);
            cr_groups_neg_[group_idx]->ag_ += prop;                                 // update group propensity
        }
        a0_ += prop;                                                                // update total propensity
    }


    // erease a propensity from a group, given the idx of the group (negative values allowed here!)
    void erease_propensity_from_group(IntType prop_idx, IntType group_idx) {
        assert(group_idx == std::logb(prop));
        if (group_idx>=0) {
            cr_groups_pos_[group_idx]->propensity_idxes.erase(prop);
            cr_groups_pos_[group_idx]->ag_ -= prop;                                 // update group propensity
            if (cr_groups_pos_[group_idx]->propensity_idxes.size() == 0)
                cr_groups_pos_[group_idx]->ag_ = 0.;
        }
        else {
            ++group_idx;
            cr_groups_neg_[group_idx]->propensity_idxes.erase(prop);
            cr_groups_neg_[group_idx]->ag_ -= prop;                                 // update group propensity
            if (cr_groups_neg_[group_idx]->propensity_idxes.size() == 0)
                cr_groups_neg_[group_idx]->ag_ = 0.;
        }
        a0_ -= prop;                                                                // update total propensity
    }


    // update the propensity relative to r-th reaction inside i-th tetrahedron
    void update_propensity(IntType r, IntType i, FloatType new_prop){
        // TODO: here we should do:
        //  1- check what was old value of propensity --> old_prop
        //  2- compute propensity of r-th, i-th --> new_prop
        //  3- if in a new group J:
        //      3.a check if enough groups, if not cr_groups_xxx_.resize() // ?? is it ok or will take forever due to copy c-tor or correctly move? better to use a pointer?
        //      3.b erease from old group, insert to new group
        //  3/bis- if in same group: erease() old val and insert() new val in same group

        IntType old_prop_group = std::logb(propensity_val(r, i));
        IntType new_prop_group = std::logb(new_prop);

        //propensity_val(r, i) = new_prop;

        if (old_prop_group != new_prop_group) {
            erease_propensity_from_group(propensity_idx, old_prop_group);
        }

        // TODO: this funciton is wrong (mess up idx and val of old/new props)


    }



private:
    IntType n_reacs_;
    IntType n_tets_;
    std::vector<FloatType> propensity_values_;      // vector with all propensity values
    FloatType a0_;                                  // total propensity
    std::vector<std::unique_ptr<CompRejGroup> > cr_groups_pos_; // i-th group, i=0.., covers range (2^{i}, 2^{i+1})
    std::vector<std::unique_ptr<CompRejGroup> > cr_groups_neg_; // i-th group, i=0.., covers range (2^{-i-1}, 2^{-i})
};



}

#endif// MAPP_READI_COMPREJ_
