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

namespace readi {


template<class IntType, class FloatType >
class CompRejGroup;


template<class IntType, class FloatType>
class CompRej{
public:
    using idx_type = IntType;
    using real_type = FloatType;

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
        return propensity_values_[n_reacs_*i + r];
    }
    inline FloatType propensity_val(IntType r, IntType i) const {
        assert(r>=0 && r<n_reacs_);
        assert(i>=0 && i<n_tets_);
        return propensity_values_[n_reacs_*i + r];
    }



    // get total propensity
    inline FloatType get_total_propensity() {
        return a0_;
    }


    // select the group where reaction will happen, using a Categorical distribution
    template <class Generator>
    IntType select_group(Generator& g) const {
        // TODO: test if this really works
        const FloatType unif = (g() - g.min())/double(g.max() - g.min());
        const FloatType discr = unif * get_total_propensity();
        FloatType part_sum = 0.;
        for (int i=0; i<cr_groups_neg_.size(); ++i){
            part_sum += cr_groups_neg_[i].get_group_propensity();
            if (discr <= part_sum)
                return -(i+1);
        }
        for (int i=0; i<cr_groups_pos_.size()-1; ++i){
            part_sum += cr_groups_pos_[i].get_group_propensity();
            if (discr <= part_sum)
                return i;
        }
        return cr_groups_pos_.size()-1; // last remaining group
    }



    // insert a propensity in a group, given the idx of the group (negative values allowed here!)
    void insert_prop_in_group(FloatType prop, IntType group_idx) {
        // TODO
        // - also check if enough elements in vector, resize if needed
        // - pointer to groups, not groups!!

    }

    // erease a propensity from a group, given the idx of the group (negative values allowed here!)
    void erease_prop_from_group(FloatType prop, IntType group_idx) {
        // TODO

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

        if (old_prop_group != new_prop_group)
            // ...
    }



private:
    IntType n_reacs_;
    IntType n_tets_;
    std::vector<FloatType> propensity_values_;      // vector with all propensity values
    FloatType a0_;                                  // total propensity
    std::vector<CompRejGroup<IntType,FloatType> > cr_groups_pos_; // i-th group, i=0.., covers range (2^{i}, 2^{i+1})
    std::vector<CompRejGroup<IntType,FloatType> > cr_groups_neg_; // i-th group, i=0.., covers range (2^{-i-1}, 2^{-i})
};



template<class IntType, class FloatType>
class CompRejGroup{
public:
    using idx_type = IntType;
    using real_type = FloatType;


    // get total propensity
    inline FloatType get_group_propensity() {
        return ag_;
    }

private:
    FloatType a0_;                                  // total group propensity


};

}

#endif// MAPP_READI_COMPREJ_
