/*
 * Neuromapp - memory.cpp, Copyright (c), 2015,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * judit.planas@epfl.ch,
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
 * GNU General Public License for more details. See
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/test/keyvalue/memory.cpp
 *  Test on the key/value store miniapp memory
 */

#define BOOST_TEST_MODULE KeyValueTest
#include <boost/test/unit_test.hpp>
#include "keyvalue/utils/trait.h"
#include "keyvalue/memory.h"
//#include "utils/argv_data.h"


BOOST_AUTO_TEST_CASE(nrnthread_constructor_default_test){
    keyvalue::nrnthread n;
    BOOST_CHECK_EQUAL(n.size(), 0);
}

BOOST_AUTO_TEST_CASE(nrnthread_constructor_test){
    keyvalue::nrnthread n(10);
    BOOST_CHECK_EQUAL(n.size(), 10);

    keyvalue::nrnthread::container_type::const_pointer p = n.front_pointer();
    BOOST_CHECK(p != NULL);
    for (unsigned int i = 0; i < n.size(); i++) {
        BOOST_CHECK(p[i] <= 75.0 && p[i] >= -75.0);
    }
}

BOOST_AUTO_TEST_CASE(group_constructor_default_test){
    keyvalue::group<keyvalue::trait_meta<keyvalue::map>::meta_type> g;
    BOOST_CHECK_EQUAL(g.size(), 0);
}

BOOST_AUTO_TEST_CASE(group_constructor_test){
    keyvalue::group<keyvalue::trait_meta<keyvalue::map>::meta_type> g;
    keyvalue::nrnthread n1(10);
    keyvalue::nrnthread n2(15);
    keyvalue::nrnthread n3(20);

    g.push_back(n1);
    BOOST_CHECK_EQUAL(g.size(), 1);
    // n1
    BOOST_CHECK_EQUAL(g.meta_at(0).value_size(), n1.size());
    for (unsigned int i = 0; i < n1.size(); i++) {
        BOOST_CHECK_EQUAL(g.meta_at(0).value()[i], n1.front_pointer()[i]);
    }

    g.push_back(n2);
    BOOST_CHECK_EQUAL(g.size(), 2);
    // n1
    BOOST_CHECK_EQUAL(g.meta_at(0).value_size(), n1.size());
    for (unsigned int i = 0; i < n1.size(); i++) {
        BOOST_CHECK_EQUAL(g.meta_at(0).value()[i], n1.front_pointer()[i]);
    }
    // n2
    BOOST_CHECK_EQUAL(g.meta_at(1).value_size(), n2.size());
    for (unsigned int i = 0; i < n2.size(); i++) {
        BOOST_CHECK_EQUAL(g.meta_at(1).value()[i], n2.front_pointer()[i]);
    }

    g.push_back(n3);
    BOOST_CHECK_EQUAL(g.size(), 3);
    // n1
    BOOST_CHECK_EQUAL(g.meta_at(0).value_size(), n1.size());
    for (unsigned int i = 0; i < n1.size(); i++) {
        BOOST_CHECK_EQUAL(g.meta_at(0).value()[i], n1.front_pointer()[i]);
    }
    // n2
    BOOST_CHECK_EQUAL(g.meta_at(1).value_size(), n2.size());
    for (unsigned int i = 0; i < n2.size(); i++) {
        BOOST_CHECK_EQUAL(g.meta_at(1).value()[i], n2.front_pointer()[i]);
    }
    // n3
    BOOST_CHECK_EQUAL(g.meta_at(2).value_size(), n3.size());
    for (unsigned int i = 0; i < n3.size(); i++) {
        BOOST_CHECK_EQUAL(g.meta_at(2).value()[i], n3.front_pointer()[i]);
    }
}

BOOST_AUTO_TEST_CASE(group_constructor_killer_test){
    std::vector<keyvalue::nrnthread> nts;
    unsigned int numnts = 130;
    keyvalue::group<keyvalue::trait_meta<keyvalue::map>::meta_type> g(numnts);

    nts.reserve(numnts);
    for (unsigned int i = 0; i < nts.capacity(); i++) {
        // introduce some variability in size
        unsigned int ntsize = 20 + i - (i%3 * i%4 + i%2);
        nts.push_back(keyvalue::nrnthread(ntsize));
    }

    for (unsigned int i = 0; i < nts.size(); i++) {
        g.push_back(nts[i]);
        BOOST_CHECK_EQUAL(g.size(), i+1);
        for (unsigned int j = 0; j < g.size(); j++) {
            BOOST_CHECK_EQUAL(g.meta_at(j).value_size(), nts[j].size());
            for (unsigned int k = 0; k < nts[j].size(); k++) {
                BOOST_CHECK_EQUAL(g.meta_at(j).value()[k], nts[j].front_pointer()[k]);
            }
        }
    }
}
