/*
 * Neuromapp - loop.h, Copyright (c), 2015,
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

#ifndef MAPP_LOOP_H
#define MAPP_LOOP_H

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

#include "utils/mpi/timer.h"

//forward declaration
template<keyvalue::selector S>
class benchmark;

template<keyvalue::selector S>
keyvalue::statistic loop(benchmark<S> & b){
    typedef typename keyvalue::trait_meta<S>::meta_type meta_type; // get the good meta type
    // extract the group of memory and the argument
    keyvalue::group<meta_type> & g = b.get_group_nc(); //git it to the group
    keyvalue::argument const& a = b.get_args();

    // build the needed function in function of the backend
    typename keyvalue::trait_meta<S>::keyvalue_type kv;
    kv.async() = a.async();

    // the timer
    mapp::timer t;
    // go to dodo
    int comp_time_us = 100 * a.usecase() * 1000;

    // keep time trace
    std::vector<double> vtime;
    vtime.reserve(a.st() / a.dt());

    // these two loops should be merged
    for (float st = 0; st < a.st(); st += a.md()) {
        for (float md = 0; md < a.md(); md += a.dt()) {
            usleep(comp_time_us);

            t.tic();

            #pragma omp parallel for
            for (int cg = 0; cg < a.cg(); cg++)
                kv.insert(g.meta_at(cg));


            #pragma omp parallel for
            for (int cg = 0; cg < a.cg(); cg++)
                kv.wait(g.meta_at(cg));

            t.toc();
            vtime.push_back(t.time());
        }
    }

    return keyvalue::statistic(a, (a.st() / a.dt()) * a.cg(),
            a.voltages_size() * sizeof(keyvalue::nrnthread::value_type) / a.cg(), vtime);
}
#endif
