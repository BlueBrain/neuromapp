/*
 * Neuromapp - cstep.cpp, Copyright (c), 2015,
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
#include "keyvalue/mpikeyvalue.h"
#include "keyvalue/utils/tools.h"
#include "keyvalue/utils/argument.h"
#include "keyvalue/utils/statistic.h"
#include "keyvalue/utils/trait.h"

#include "utils/mpi/timer.h"

template<keyvalue::selector S>
class benchmark{
public:
    typedef typename keyvalue::trait_meta<S>::meta_type meta_type; // extract the good meta
    /** \fun benchmark(std::size_t choice, std::size_t qmb = 4096)
        \brief compute the total number of compartment (2.5 = 2.5 MB per neuron, 350 compartiment per neuron)
        4096 MB, 25% of the memory of a compute node of the BG/Q
     */
    benchmark(keyvalue::argument const& args = keyvalue::argument()):a(args){
        s = args.voltages_size();
        int cg_size = s / args.cg();
        int first_size = cg_size + (s % args.cg());

        g = keyvalue::group<meta_type>(cg_size);
        g.push_back(keyvalue::nrnthread(first_size));

        for (int i = 1; i < args.cg(); i++)
            g.push_back(keyvalue::nrnthread(cg_size));
    }

    /** \fun get_group() const
        \brief get the group i.e. the memory */
    keyvalue::group<meta_type> const& get_group() const{
        return g;
    }

    /** \fun get_args() const
        \brief return the argument */
    keyvalue::argument const & get_args() const {
        return a;
    }

private:
    /** memory for the bench */
    keyvalue::group<meta_type> g;
    /** reference on the arguments structure */
    keyvalue::argument const & a;
    /** correspond to the total number of compartement */
    std::size_t s;
};

template<keyvalue::selector S>
keyvalue::statistic run_loop(benchmark<S> const& b){
    typedef typename keyvalue::trait_meta<S>::meta_type meta_type; // get the good meta type
    // extract the group of memory
    keyvalue::group<meta_type> const& g = b.get_group(); //git it to the group
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
    vtime.reserve(1024);

    // these two loops should be merge
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

    return keyvalue::statistic(a,vtime);
}

template<keyvalue::selector S>
std::pair<keyvalue::statistic,keyvalue::statistic> run_task(benchmark<S> const& b){
    typedef typename keyvalue::trait_meta<S>::meta_type meta_type; // get the good meta type
    // extract the group of memory and the arguement
    keyvalue::group<meta_type> const& g = b.get_group(); //get the group
    keyvalue::argument const& a = b.get_args(); // get the arguments
    // build the needed function in function of the backend
    typename keyvalue::trait_meta<S>::keyvalue_type kv;

    // time variables
    int comp_time_us = 100 * a.usecase() * 1000;
    int adjust = 1000 * a.usecase()/a.threads();
    int sleep_time;

    // the timer
    mapp::timer t1,t2;

    // keep time trace
    std::vector<double> vtime1,vtime2;
    vtime1.reserve(1024);
    vtime2.reserve(1024);


    do{

        /******************** Compute base line computation without I/O ********************/
        t1.tic();

        for (float st = 0; st < a.st(); st += a.md()) {
            for (float md = 0; md < a.md(); md += a.dt()) {
                #pragma omp parallel
                {
                    #pragma omp single nowait
                    {
                        for (int cg = 0; cg < a.cg(); cg++) {
                            const double * dep1 = g.meta_at(cg).value(); //adress of the first element
                            const double * dep2 = (g.meta_at(cg).value()+1);  //adress of the second element

                            sleep_time += (int) ((0.45 * comp_time_us) / a.threads()) + adjust;
                            #pragma omp task depend(inout:dep1)
                            {
                                // First computation - 45%
                                usleep((int) ((0.45 * comp_time_us) / a.threads()) + adjust);
                            }

                            sleep_time += (int) ((0.05 * comp_time_us) / a.threads()) + adjust;
                            #pragma omp task depend(inout:dep1, dep2)
                            {
                                // Linear algebra - 5%
                                usleep((int) ((0.05 * comp_time_us) / a.threads()) + adjust);
                            }

                            sleep_time += (int) ((0.38 * comp_time_us) / a.threads()) + adjust;
                            #pragma omp task depend(inout:dep1)
                            {
                                // Second computation - 38%
                                usleep((int) ((0.38 * comp_time_us) / a.threads()) + adjust);
                            }
                        }

                    } // omp single
                } // omp parallel

            }
            #pragma omp barrier

            sleep_time += (int) ((0.05 * comp_time_us)) + adjust;
            usleep((int) (0.05 * comp_time_us) + adjust);
        }

        t1.toc();

        t2.tic();
        // these two loops should be merge
        for (float st = 0; st < a.st(); st += a.md()) {
            for (float md = 0; md < a.md(); md += a.dt()) {
                    #pragma omp parallel
                    {
                        #pragma omp single nowait
                        {
                            for (int cg = 0; cg < a.cg(); cg++) { // correct cell group

                                const double * dep1 = g.meta_at(cg).value(); //adress of the first element
                                const double * dep2 = (g.meta_at(cg).value()+1);  //adress of the second element

                                #pragma omp task depend(inout:dep1)
                                {
                                    // First computation - 45%
                                    usleep((int) ((0.45 * comp_time_us) / a.threads()) + adjust);
                                }

                                #pragma omp task depend(inout:dep1, dep2)
                                {
                                    // Linear algebra - 5%
                                    usleep((int) ((0.05 * comp_time_us) / a.threads()) + adjust);
                                }

                                #pragma omp task depend(inout:dep2)
                                {
                                    // I/O
                                    kv.insert(g.meta_at(cg));
                                }

                                #pragma omp task depend(inout:dep1)
                                {
                                    // Second computation - 38%
                                    usleep((int) ((0.38 * comp_time_us) / a.threads()) + adjust);
                                }
                            } // end loop cg
                        } // omp single
                    } // omp parallel
                } // for loop dt

                #pragma omp barrier

                // Spike exchange - 5%
                usleep((int) (0.05 * comp_time_us) + adjust);
            } // for loop st
        t2.toc();
        vtime1.push_back(t1.time());
        vtime2.push_back(t2.time());
    if ((t2.time()-t1.time())< 0.)
        std::cout << "Negative time, computing again..." << std::endl;

    } while ((t2.time()-t1.time()) < 0);

    return std::make_pair(keyvalue::statistic(a,vtime1),keyvalue::statistic(a,vtime2));
}


#endif
