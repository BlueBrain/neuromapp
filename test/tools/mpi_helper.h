/*
 * Neuromapp - mpi_helper.h, Copyright (c), 2015,
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
 * @file neuromapp/test/tools/mpi_helper.h
 *  Contains the MPI init/finalize calls needed by some tests
 */

#ifndef MAPP_TEST_MPI_HELPER_H
#define MAPP_TEST_MPI_HELPER_H

#include <boost/test/unit_test.hpp>
#include <mpi.h>

struct MPIInitializer {
    MPIInitializer(){
        int init = 0;
        MPI_Initialized(&init);
        if (init == 0) {
            MPI::Init();
        }
    }
    ~MPIInitializer(){
        int fini = 0;
        MPI_Finalized(&fini);
        if (!fini) {
            MPI::Finalize();
        }
    }
};

BOOST_GLOBAL_FIXTURE(MPIInitializer);

#endif
