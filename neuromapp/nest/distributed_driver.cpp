/*
 * Neuromapp - distributed_driver.cpp, Copyright (c), 2015,
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
 * @file neuromapp/event_passing/drivers/distributed_driver.cpp
 * runs the spike exchange simulation
 */
#include <mpi.h>
#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <cassert>
#include <sys/time.h>

#include "utils/storage/neuromapp_data.h"

#ifdef _OPENMP
    #include <omp.h>
#endif

#include "nest/synapse/connectionmanager.h"
#include "nest/simulationmanager.h"


int main(int argc, char* argv[]) {
    assert(argc == 8);

    MPI_Init(NULL, NULL);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int nthreads = atoi(argv[1]);
    int simtime = atoi(argv[2]);
    int ncells = atoi(argv[3]);
    int fan = atoi(argv[4]);
    int nSpikes = atoi(argv[5]);
    int mindelay = atoi(argv[6]);
    bool algebra = atoi(argv[7]);

    namespace po = boost::program_options;
    po::variables_map vm;
    vm.insert(std::make_pair("nNeurons", po::variable_value(ncells, false)));
    vm.insert(std::make_pair("nGroups", po::variable_value(nthreads, false)));
    vm.insert(std::make_pair("size", po::variable_value(size, false)));
    vm.insert(std::make_pair("rank", po::variable_value(rank, false)));
    vm.insert(std::make_pair("thread", po::variable_value(0, false)));
    vm.insert(std::make_pair("nConnections", po::variable_value(fan, false)));
    vm.insert(std::make_pair("model", po::variable_value(std::string("tsodyks2"), false)));
    vm.insert(std::make_pair("delay", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("weight", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("U", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("u", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("x", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("tau_rec", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("tau_fac", po::variable_value(1.0, false)));

    //create environment
    environment::event_generator generator(nthreads);

    double mean = static_cast<double>(simtime) / static_cast<double>(nSpikes);
    double lambda = 1.0 / static_cast<double>(mean * size);

    environment::continousdistribution neuro_dist(size, rank, ncells);

    environment::generate_events_kai(generator.begin(),
                              simtime, nthreads, rank, size, lambda, &neuro_dist);

    //preallocate vector for results
    int num_detectors = ncells;
    std::vector<nest::spikedetector> detectors(num_detectors);
    std::vector<nest::targetindex> detectors_targetindex(num_detectors);

    for(unsigned int i=0; i < num_detectors; ++i) {
        detectors[i].set_lid(i);    //give nodes a local id
        //scheduler stores pointers to the spike detectors
        detectors_targetindex[i] = nest::scheduler::add_node(&detectors[i]);  //add them to the scheduler
    }

    environment::presyn_maker presyns(fan, environment::fixedoutdegree);
    presyns(rank, &neuro_dist);

    nest::connectionmanager cn(vm);
    nest::build_connections_from_neuron(presyns, neuro_dist, detectors_targetindex, vm, cn);

    nest::eventdelivermanager edm(cn, size, nthreads, mindelay);
    nest::simulationmanager sm(edm, generator, rank, size, nthreads);


    struct timeval start, end;
    //run simulation
    gettimeofday(&start, NULL);

    int t = 0;
    long from_step = 0;
    long to_step = mindelay;
    long Tstop = simtime;

    omp_set_num_threads(nthreads);

    #pragma omp parallel
    {
        const int thrd = omp_get_thread_num();
        while(t < Tstop){
            #pragma omp barrier

            // deliver only from second time step on
            if (t>0)
                edm.deliver_events(thrd, t);
            sm.update(thrd, t, from_step, to_step);
            #pragma omp barrier
            #pragma omp master
            {
                edm.gather_events();
            }
            
            #pragma master
            {
                t++;
                if (t+to_step>Tstop)
                    to_step = Tstop-t;
            }
            #pragma barrier
        }
    }



    gettimeofday(&end, NULL);

    long long diff_ms = (1000 * (end.tv_sec - start.tv_sec))
        + ((end.tv_usec - start.tv_usec) / 1000);

    if(rank == 0){
        std::cout<<"run time: "<<diff_ms<<" ms"<<std::endl;
    }

    //pl.accumulate_stats();
    //accumulate_stats(s_interface);

    MPI_Finalize();
    return 0;
}
