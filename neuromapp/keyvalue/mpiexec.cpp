/*
 * Neuromapp - meta.h, Copyright (c), 2015,
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

#include "keyvalue/utils/trait.h"
#include "keyvalue/benchmarks/benchmark.h"
#include "keyvalue/utils/statistic.h"
#include "keyvalue/utils/trait.h"

int main(int argc, char* argv[]) {


    // build argument from the command line
	keyvalue::argument a(argc, argv);
    // build the bench infunction of the argument
    if(a.backend() == "map"){
        benchmark<keyvalue::map> b(a);
        //bench
        keyvalue::statistic s = loop(b);
        //compute statistics
        s.process();
        //print the results
        std::cout << s << std::endl;

#if _OPENMP >= 201307

        std::pair<keyvalue::statistic,keyvalue::statistic> p = task(b);
        // compute the statistic
        p.first.process();
        p.second.process();
        //print the results
        std::cout << p.first << std::endl;
        std::cout << p.second << std::endl;
#endif
    }

    mapp::master.finalize();
    return 0;
}
