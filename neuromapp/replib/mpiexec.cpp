/*
 * Neuromapp - mpiexec.cpp, Copyright (c), 2015,
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
 * GNU General Public License for more details..  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

// Include the MPI version 2 C++ bindings:
#include <mpi.h>
#include <iostream>
#include <stdlib.h>

#include "replib/benchmark.h"
#include "replib/utils/statistics.h"

/** \fun main
    \brief main program of the miniapp, run by MPI
 */
int main(int argc, char* argv[]) {
    // Build benchmark according to command line arguments
    benchmark b(argc, argv);
    replib::statistics s = run_benchmark(b);
    s.process();
    std::cout << s << std::endl;
   return 0;
}
