/*
 * Neuromapp - hello.cpp, Copyright (c), 2015,
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
 * @file neuromapp/test/hello/hello.cpp
 *  Test on the Hello world !
 */

#define BOOST_TEST_MODULE HelloTest
#include "hello/hello.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(hello_test){
    char arg1[]="--name=world";
    char arg2[]="NULL";
    char * const argv[] = {arg1, arg2};
    int argc = 2;
    BOOST_CHECK(hello_execute(argc,argv)==0);
}
