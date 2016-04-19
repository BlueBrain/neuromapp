/*
 * Neuromapp - benchmark.h, Copyright (c), 2015,
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
 * GNU General Public License for more details. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

#ifndef MAPP_BENCHMARK_H
#define MAPP_BENCHMARK_H

#include <vector>
#include <numeric>
#include <functional>

#include "keyvalue/meta.h"
#include "keyvalue/memory.h"
#include "keyvalue/memory.h"
#include "keyvalue/meta.h"
#include "keyvalue/utils/tools.h"
#include "keyvalue/utils/argument.h"
#include "keyvalue/utils/statistic.h"
#include "keyvalue/utils/trait.h"
#include "keyvalue/benchmarks/loop.h"

#if _OPENMP >= 201307
#include "keyvalue/benchmarks/task.h"
#endif

#include "utils/mpi/timer.h"

template<keyvalue::selector S>
class benchmark{
public:
    typedef typename keyvalue::trait_meta<S>::meta_type meta_type; // extract the good meta
    /** \fun benchmark(std::size_t choice, std::size_t qmb = 4096)
        \brief compute the total number of compartments (2.5 = 2.5 MB per neuron, 350 compartments per neuron)
        4096 MB, 25% of the memory of a compute node of the BG/Q
     */
    benchmark(keyvalue::argument & args):a(args){
        init();
    }

    benchmark() : a(keyvalue::argument()) {
        init();
    }

    /** \fun get_group() const
        \brief get the group i.e. the memory */
    keyvalue::group<meta_type> const& get_group() const{
        return g;
    }

    /** \fun get_group()
        \brief get the group i.e. the memory */
    keyvalue::group<meta_type> & get_group_nc() {
        return g;
    }

    /** \fun get_args() const
        \brief return the argument */
    keyvalue::argument const & get_args() const {
        return a;
    }

    /** \fun get_args() const
        \brief return the argument */
    keyvalue::argument & get_args() {
        return a;
    }

private:
    /** memory for the bench */
    keyvalue::group<meta_type> g;
    /** reference on the arguments structure */
    keyvalue::argument & a;
    /** corresponds to the total number of compartments */
    std::size_t s;

    /** \fun init()
        \brief initialize the object, common code for different constructors */
    void init() {
        s = a.voltages_size();
        int cg_size = s / a.cg();
        int first_size = cg_size + (s % a.cg());

        g = keyvalue::group<meta_type>(cg_size);
        g.push_back(keyvalue::nrnthread(first_size));

        for (int i = 1; i < a.cg(); i++)
            g.push_back(keyvalue::nrnthread(cg_size));
    }
};

template<keyvalue::selector S>
keyvalue::statistic run_benchmark(benchmark<S> & b){
    if (b.get_args().taskdeps()) {
#if _OPENMP >= 201307
        return task<S>(b);
#else
        std::cout << "Error: calling OpenMP task implementation, "
                << "but task dependencies not supported by this OpenMP version." << std::endl
                << "Running the OpenMP loop implementation instead." << std::endl;
        b.get_args().taskdeps() = false;
#endif
    }
    return loop<S>(b);
}

#endif
