/*
 * Neuromapp - solver.cpp, Copyright (c), 2015,
 * Sam Yates - Swiss Federal Institute of technology in Lausanne,
 * Sam.Yatest@epfl.ch,
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/test/utils/test_argv_data.cpp
 *  Test the command line helper
 */

#define BOOST_TEST_MODULE ArgvDataTest

#include <boost/test/unit_test.hpp>

#include "utils/argv_data.h"

BOOST_AUTO_TEST_CASE(argv_data_empty){
    mapp::argv_data A;
    BOOST_CHECK(A.argc()==0);
    BOOST_CHECK(A.argv()[0]==(char *)0);
}

BOOST_AUTO_TEST_CASE(argv_data_values){
    std::string s[4]={"zero","one","two","three"};
    int narg=sizeof(s)/sizeof(s[0]);

    mapp::argv_data A(s,s+narg);

    int argc=A.argc();
    char * const *argv=A.argv();

    BOOST_CHECK(argc==narg);
    for (int i=0;i<narg;++i)
        BOOST_CHECK(argv[i]==s[i]);

    BOOST_CHECK(argv[argc]==(char *)0);
}
