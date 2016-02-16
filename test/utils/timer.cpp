/*
 * Neuromapp - timer.cpp, Copyright (c), 2015,
 * Sam Yates - Swiss Federal Institute of technology in Lausanne,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Sam.Yatest@epfl.ch,
 * timothee.ewart@epfl.ch
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
 * @file neuromapp/test/utils/timer.cpp
 *  Test the storage class
 */

#define BOOST_TEST_MODULE TIMER_TEST

#include <boost/test/unit_test.hpp>
#include "utils/mpi/controler.h"
#include "utils/mpi/timer.h"

#include <unistd.h>

BOOST_AUTO_TEST_CASE(timer_test){
    mapp::timer t;
    t.tic();
    usleep(1000000);
    t.toc();
    BOOST_CHECK_CLOSE(t.time(),1,1);
}
