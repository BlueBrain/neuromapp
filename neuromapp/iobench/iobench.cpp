/*
 * Neuromapp - iobench.cpp, Copyright (c), 2015,
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


#include <iostream>

#include "iobench/benchmark.h"


/** \fun main
    \brief main program of the miniapp, run by MPI+OMP or just OMP
    Measure R/W performance of different K/V stores
 */
int main(int argc, char* argv[]) {
    iobench::benchmark b(argc, argv);
    b.createData();
    b.run();
    b.print_stats(std::cout);

    return 0;
}
