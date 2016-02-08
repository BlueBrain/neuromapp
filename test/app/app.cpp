/*
 * Neuromapp - app.cpp, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
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
 * @file neuromapp/test/app/app.cpp
 *  The the main app e.g. the driver
 */

#define BOOST_TEST_MODULE AppTest
#include <boost/test/unit_test.hpp>

#include "neuromapp/app/driver.h"

template<int n>
int foo(int argc, char * const argv[]){
    return n;
}

BOOST_AUTO_TEST_CASE(app_test){
    mapp::driver d;
    char arg0[]="dummy";
    char arg1[]="foo";
    char * const argv[] = {arg0,arg1};
    int argc = 2;
    int(*f)(int,char * const*);
    f = foo<mapp::MAPP_UNKNOWN_ERROR>;
    d.insert("foo",f);
    BOOST_CHECK_THROW(d.execute(argc,argv), mapp::driver_exception);
}