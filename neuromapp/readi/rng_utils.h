/*
 * Neuromapp - rng_utils.h, Copyright (c), 2015,
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
 * @file neuromapp/readi/rng_utils.h
 * \brief Random number genrator utilities for  Readi Miniapp
 */

#ifndef MAPP_READI_RNG_UTILS_
#define MAPP_READI_RNG_UTILS_

#include <cassert>
#include <cmath>

namespace readi {

// Random rounding of a floating point number.
// Returns floor(x) or ceil(x) by drawing a Bernoulli
template <class IntType, class FloatType, class Generator>
IntType rand_round(FloatType x, Generator& g) {

    FloatType x_floor = std::floor(x);
    FloatType prob_roundup = x - x_floor;

    if ((g() - g.min()) < prob_roundup * (g.max() - g.min()))
        return static_cast<IntType>(x_floor) + 1;
    else
        return static_cast<IntType>(x_floor);
}

template <class IntType = int>
class binomial_distribution {
    static_assert(std::is_unsigned<IntType>::value,
                  "IntType should be an unsigned literal type");

  public:
    //! Type of generated values. Alias of IntType
    typedef IntType result_type;

    //! Constructor
    /*!
      \param t number of trials
      \param probabilty of event 1
    */
    explicit binomial_distribution(IntType t = 1, double p = 0.5)
        : t_(t), p_(p) {
        assert(p_ >= 0. && p <= 1.);
    }

    //! Generate random vector following binomial distribution
    /*!
      \param g an uniform random bit generator object
      \return next random vector from given distribution
    */
    template <class G>
    result_type operator()(G& g) const {
        if (p_ == 0.0 || t_ == 0)
            return 0;

        result_type pos_trials = 0; // number of positive trials out of total t_
        const auto g_min = g.min();
        const auto g_max = g.max();
        for (IntType tt = 0; tt < t_; ++tt) {
            const auto val_g = g();

            if ((val_g - g_min) < p_ * (g_max - g_min))
                ++pos_trials;
        }
        return pos_trials;
    }

    //! Get number of trials
    /*!
      \return number of trials
    */
    double p() const { return p_; }

    //! Get probability of event 1
    /*!
      \return probability of event 1
    */
    result_type t() const { return t_; }

    //! Get minimum potentially generated value
    /*!
      \return minimum potentially generated value
    */
    result_type min() const { return 0; }

    //! Get maximum potentially generated value
    /*!
      \return maximum potentially generated value
    */
    result_type max() const { return t_; }

  private:
    // number of trials
    const IntType t_;

    // probability of event 1
    const double p_;
};

} // namespace readi

#endif // MAPP_READI_TETS_
