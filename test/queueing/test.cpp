/*
 * Neuromapp - test.cpp, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
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
 * @file neuromapp/test/queuing/test.cpp
 *  Test on the Queueing Miniapp.
 */

#define BOOST_TEST_MODULE QueueingTest
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/filesystem.hpp>

#include "test/coreneuron_1.0/helper.h"
#include <vector>
#include "queueing/queueing.h"
#include "utils/error.h"

namespace bfs = ::boost::filesystem;

BOOST_AUTO_TEST_CASE(queueing_test){
    char arg1[]="--numthread=8";
    char * const argv[] = {arg1};
    int argc = 1;
    BOOST_CHECK(queueing_execute(argc,argv)==0);
}

