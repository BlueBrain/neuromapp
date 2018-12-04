/*
 * Neuromapp - hello.cpp, Copyright (c), 2015,
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

#define BOOST_TEST_MODULE ReadiTest
#include "readi/Reac.h"
#include "readi/readi.h"
#include <boost/test/unit_test.hpp>

static constexpr double rel_tolerance = 1.e-12;

BOOST_AUTO_TEST_CASE(reac_constructor1_test) {
    std::vector<std::string> spec_names = {"H", "O_2", "K", "HO_2", "M"};
    std::vector<std::string> lhs = {"H", "O_2", "M"};
    std::vector<std::string> rhs = {"M", "HO_2"};
    double reaction_rate = 1.23e+8;

    readi::Reac<unsigned int, double> reac(spec_names, lhs, rhs, reaction_rate);

    std::vector<int> lhs_idxs = {0, 1, 4};
    std::vector<int> lhs_cnts = {1, 1, 1};
    auto reac_lhs_idxs = reac.get_lhs_idxs();
    auto reac_lhs_cnts = reac.get_lhs_counts();
    BOOST_CHECK_EQUAL_COLLECTIONS(lhs_idxs.begin(), lhs_idxs.end(),
                                  reac_lhs_idxs.begin(), reac_lhs_idxs.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(lhs_cnts.begin(), lhs_cnts.end(),
                                  reac_lhs_cnts.begin(), reac_lhs_cnts.end());

    std::vector<int> rhs_idxs = {3, 4};
    std::vector<int> rhs_cnts = {1, 1};
    auto reac_rhs_idxs = reac.get_rhs_idxs();
    auto reac_rhs_cnts = reac.get_rhs_counts();
    BOOST_CHECK_EQUAL_COLLECTIONS(rhs_idxs.begin(), rhs_idxs.end(),
                                  reac_rhs_idxs.begin(), reac_rhs_idxs.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(rhs_cnts.begin(), rhs_cnts.end(),
                                  reac_rhs_cnts.begin(), reac_rhs_cnts.end());

    std::vector<int> upd_idxs = {0, 1, 3};
    std::vector<int> upd_cnts = {-1, -1, 1};
    auto reac_upd_idxs = reac.get_upd_idxs();
    auto reac_upd_cnts = reac.get_upd_counts();
    BOOST_CHECK_EQUAL_COLLECTIONS(upd_idxs.begin(), upd_idxs.end(),
                                  reac_upd_idxs.begin(), reac_upd_idxs.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(upd_cnts.begin(), upd_cnts.end(),
                                  reac_upd_cnts.begin(), reac_upd_cnts.end());

    BOOST_CHECK_CLOSE(reaction_rate, reac.get_rate(), rel_tolerance);
}

BOOST_AUTO_TEST_CASE(reac_constructor2_test) {
    std::vector<std::string> spec_names = {"D", "E", "C", "A", "B"};
    std::vector<std::string> lhs = {"B", "C", "B"};
    std::vector<std::string> rhs = {"A", "C"};
    double reaction_rate = 1.23e+8;

    readi::Reac<unsigned int, double> reac(spec_names, lhs, rhs, reaction_rate);

    std::vector<int> lhs_idxs = {2, 4};
    std::vector<int> lhs_cnts = {1, 2};
    auto reac_lhs_idxs = reac.get_lhs_idxs();
    auto reac_lhs_cnts = reac.get_lhs_counts();
    BOOST_CHECK_EQUAL_COLLECTIONS(lhs_idxs.begin(), lhs_idxs.end(),
                                  reac_lhs_idxs.begin(), reac_lhs_idxs.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(lhs_cnts.begin(), lhs_cnts.end(),
                                  reac_lhs_cnts.begin(), reac_lhs_cnts.end());

    std::vector<int> rhs_idxs = {2, 3};
    std::vector<int> rhs_cnts = {1, 1};
    auto reac_rhs_idxs = reac.get_rhs_idxs();
    auto reac_rhs_cnts = reac.get_rhs_counts();
    BOOST_CHECK_EQUAL_COLLECTIONS(rhs_idxs.begin(), rhs_idxs.end(),
                                  reac_rhs_idxs.begin(), reac_rhs_idxs.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(rhs_cnts.begin(), rhs_cnts.end(),
                                  reac_rhs_cnts.begin(), reac_rhs_cnts.end());

    std::vector<int> upd_idxs = {3, 4};
    std::vector<int> upd_cnts = {1, -2};
    auto reac_upd_idxs = reac.get_upd_idxs();
    auto reac_upd_cnts = reac.get_upd_counts();
    BOOST_CHECK_EQUAL_COLLECTIONS(upd_idxs.begin(), upd_idxs.end(),
                                  reac_upd_idxs.begin(), reac_upd_idxs.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(upd_cnts.begin(), upd_cnts.end(),
                                  reac_upd_cnts.begin(), reac_upd_cnts.end());

    BOOST_CHECK_CLOSE(reaction_rate, reac.get_rate(), rel_tolerance);
}

BOOST_AUTO_TEST_CASE(reac_scaled_rate_test) {
    std::vector<std::string> spec_names = {"D", "E", "C", "A", "B"};
    std::vector<std::string> lhs = {"B", "C", "B"};
    std::vector<std::string> rhs = {"A", "C"};
    double reaction_rate = 1.23e+8;

    readi::Reac<unsigned int, double> reac(spec_names, lhs, rhs, reaction_rate);

    double volume = 1.10e-18;

    BOOST_CHECK_CLOSE(reac.compute_c_mu(reaction_rate, volume),
                      reaction_rate *
                          std::pow(1.0e+3 * volume * N_avogadro, -2),
                      rel_tolerance);
}
