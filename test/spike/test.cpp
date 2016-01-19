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
 *  Test on the Spike Miniapp.
 */

#define BOOST_TEST_MODULE SpikeTest
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/filesystem.hpp>
#include <vector>

#include "test/coreneuron_1.0/helper.h"
#include "spike/spike.h"
#include "spike/mpispikegraph.h"
#include "utils/error.h"

namespace bfs = ::boost::filesystem;

BOOST_AUTO_TEST_CASE(spike_setup_test){
    MPI::Init(NULL,NULL);
    DistributedSpikeGraph dsg(4,3,2,1);


    MPI::finalize();
}

