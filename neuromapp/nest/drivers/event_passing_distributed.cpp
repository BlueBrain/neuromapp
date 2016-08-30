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

#include "nest/nestkernel/environment/connectionmanager.h"
#include "nest/nestkernel/event_passing/simulationmanager.h"


int main(int argc, char* argv[]) {
    assert(argc == 16);

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

    std::string model(argv[7]);
    double syn_delay = boost::lexical_cast<double>(argv[8]);
    double syn_weight = boost::lexical_cast<double>(argv[9]);
    double syn_U = boost::lexical_cast<double>(argv[10]);
    double syn_u = boost::lexical_cast<double>(argv[11]);
    double syn_x = boost::lexical_cast<double>(argv[12]);
    double syn_tau_rec = boost::lexical_cast<double>(argv[13]);
    double syn_tau_fac = boost::lexical_cast<double>(argv[14]);
    bool pool = boost::lexical_cast<bool>(argv[15]);

    //use program options to pass parameters
    namespace po = boost::program_options;
    po::variables_map vm;
    vm.insert(std::make_pair("nNeurons", po::variable_value(ncells, false)));
    vm.insert(std::make_pair("nThreads", po::variable_value(nthreads, false)));
    vm.insert(std::make_pair("model", po::variable_value(std::string("tsodyks2"), false)));
    vm.insert(std::make_pair("delay", po::variable_value(syn_delay, false)));
    vm.insert(std::make_pair("weight", po::variable_value(syn_weight, false)));
    vm.insert(std::make_pair("U", po::variable_value(syn_U, false)));
    vm.insert(std::make_pair("u", po::variable_value(syn_u, false)));
    vm.insert(std::make_pair("x", po::variable_value(syn_x, false)));
    vm.insert(std::make_pair("tau_rec", po::variable_value(syn_tau_rec, false)));
    vm.insert(std::make_pair("tau_fac", po::variable_value(syn_tau_fac, false)));

    nest::pool_env penv(nthreads);

    //create environment
    environment::event_generator generator(nthreads);

    double mean = static_cast<double>(simtime) / static_cast<double>(nSpikes);
    double lambda = 1.0 / static_cast<double>(mean * size);

    environment::continousdistribution neuro_dist(size, rank, ncells);

    environment::generate_events_kai(generator.begin(),
                              simtime, nthreads, rank, size, lambda, &neuro_dist);

    //preallocate vector for results
    int num_detectors = ncells;
    std::vector<nest::spikecounter> detectors(num_detectors);
    std::vector<nest::targetindex> detectors_targetindex(num_detectors);

    for(unsigned int i=0; i < num_detectors; ++i) {
        detectors[i].set_lid(i);    //give nodes a local id
        //scheduler stores pointers to the spike detectors
        detectors_targetindex[i] = nest::scheduler::add_node(&detectors[i]);  //add them to the scheduler
    }

    environment::presyn_maker presyns(fan, environment::fixedoutdegree);
    presyns(rank, &neuro_dist);

    nest::connectionmanager cn(vm);

    #ifdef _OPENMP
    omp_set_num_threads(nthreads);
    #endif
    #pragma omp parallel
    {
        #ifdef _OPENMP
        const int thrd = omp_get_thread_num();
        const int num_threads = omp_get_num_threads();
        #else
        const int thrd = 0;
        const int num_threads = 1;
        #endif

        //neuron distribution on thread based on rank distribution
        environment::continousdistribution neuro_vp_dist(num_threads, thrd, &neuro_dist);
        nest::build_connections_from_neuron(thrd, neuro_vp_dist, presyns, detectors_targetindex, cn);
    }

    nest::eventdelivermanager edm(cn, size, nthreads, mindelay);
    nest::simulationmanager sm(edm, generator, rank, size, nthreads);

    struct timeval start, end;
    //run simulation
    gettimeofday(&start, NULL);

    int t = 0;
    long from_step = 0;
    long to_step = mindelay;
    long Tstop = simtime;

    #pragma omp parallel
    {
        #ifdef _OPENMP
        const int thrd = omp_get_thread_num();
        #else
        const int thrd = 0;
        #endif

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
                t+=mindelay;
                //std::cout << "NEXT TIMESTEP: " << t << std::endl;
                if (t+to_step>Tstop)
                    to_step = Tstop-t;
            }
            #pragma barrier
        }
    }

    gettimeofday(&end, NULL);

    long long diff_ms = (1000 * (end.tv_sec - start.tv_sec))
        + ((end.tv_usec - start.tv_usec) / 1000);

    // collect statistics
    int l_num = 0;
    double  l_sumtime = 0;
    for(unsigned int i=0; i < num_detectors; ++i) {
        l_num += detectors[i].num;
        l_sumtime += detectors[i].sumtime;
    }
    int g_num;
    MPI_Reduce( &l_num, &g_num, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD );
    double g_sumtime;
    MPI_Reduce( &l_sumtime, &g_sumtime, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD );

    if(rank == 0){
        std::cout<<"run time: "<<diff_ms<<" ms"<<std::endl;
        std::cout<<"statistics: num_recv="<< g_num << " acc_spike_times=" << g_sumtime << std::endl;
    }

    MPI_Finalize();
    return 0;
}
