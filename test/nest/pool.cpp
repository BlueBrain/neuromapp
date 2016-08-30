/*
 * Neuromapp - args.cpp, Copyright (c), 2015,
 * Tim Ewart - Swiss Federal Institute of technology in Lausanne,
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
 * GNU General Public License for more details. See
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/test/nest/pool.cpp
 *  Test on the nest pool memory manager
 */

#define BOOST_TEST_MODULE PoolTest
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "utils/error.h"
#include <iostream>
#include "nest/nestkernel/environment/memory.h"

BOOST_AUTO_TEST_CASE(nest_pool_constructor)
{
    {
        nest::PoorMansAllocator p;
        p.init(); // NEST design ...
        BOOST_CHECK_EQUAL(p.chunk_size() , 1048576);
    }

    {
        nest::PoorMansAllocator p;
        p.init(32); //size in byte !
        BOOST_CHECK_EQUAL(p.chunk_size() , 32);
    }
}

BOOST_AUTO_TEST_CASE(nest_pool_allocate)
{
    {
        nest::PoorMansAllocator p;
        p.states = true;
        p.init(128);
        double * d0 = new(p.alloc(sizeof(double[16])))(double[16]); // 16*8 = 128
        BOOST_CHECK_EQUAL(p.capacity(), 0); //
        BOOST_CHECK_EQUAL(p.total_capacity(), 128); //
        double * d1 = new(p.alloc(sizeof(double[8])))(double[8]); // new chunk twice smaller for the alloc
        BOOST_CHECK_EQUAL(p.capacity(), 64); //
        BOOST_CHECK_EQUAL(p.total_capacity(), 256); //
    }

}
