/*
 * Neuromapp - args.cpp, Copyright (c), 2015,
 * Till Schumann - Swiss Federal Institute of technology in Lausanne,
 * till.schumann@epfl.ch,
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
 * @file neuromapp/test/nest/synapse.cpp
 *  Test on the nest synapse module
 */

#define BOOST_TEST_MODULE SynapseDistriTest
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <mpi.h>

#include "utils/error.h"

#include "nest/nestkernel/event_passing/mpi_manager.h"
#include "nest/nestkernel/event_passing/simulationmanager.h"
#include "nest/nestkernel/event_passing/eventdelivermanager.h"
#include "nest/nestkernel/environment/connectionmanager.h"

#include "coreneuron_1.0/event_passing/environment/event_generators.hpp"

#include "coreneuron_1.0/common/data/helper.h" // common functionalities

#include "test/tools/mpi_helper.h"


BOOST_AUTO_TEST_CASE(nest_distri_mpi)
{
    int num_processes;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //test communication with constant send_buffer size
    std::vector< int > displacements(num_processes);
    int send_buffer_size = 4;

    int recv_buffer_size = send_buffer_size * num_processes;

    std::vector< uint_t > send_buffer(send_buffer_size);
    std::vector< uint_t > recv_buffer(recv_buffer_size);

    for (int i=0; i<send_buffer_size; i++)
        send_buffer[i] = i+send_buffer_size*rank;

    nest::mpi_manager::communicate( send_buffer, recv_buffer, displacements, send_buffer_size, recv_buffer_size);

    BOOST_CHECK_EQUAL(send_buffer_size, 4);
    BOOST_CHECK_EQUAL(recv_buffer_size, 4*num_processes);

    //values should be in row
    for (int i=0; i<recv_buffer_size; i++)
        BOOST_CHECK_EQUAL(recv_buffer[i], i);

    //test communication with variable send_buffer size
    if (rank==0) {
        //send_buffer_size++;
        send_buffer.push_back(999);
    }

    nest::mpi_manager::communicate( send_buffer, recv_buffer, displacements, send_buffer_size, recv_buffer_size);

    //send_buffer_size should be the maximum value of all send_buffer sizes
    BOOST_CHECK_EQUAL(send_buffer_size, 5);
    BOOST_CHECK_EQUAL(recv_buffer_size, 5*num_processes);


    for (int r=0; r<num_processes; r++)
        for (int i=0; i<4; i++)
            BOOST_CHECK_EQUAL(recv_buffer[r*5+i], r*4+i);

    BOOST_CHECK_EQUAL(recv_buffer[4], 999);

    //empty entries should contain zeros
    for (int r=1; r<num_processes; r++)
        BOOST_CHECK_EQUAL(recv_buffer[5*r+4], 0);
}


std::vector<int> getTargets(environment::presyn_maker& presyns, const int& s_gid)
{
    std::vector<int> outputs;

    const environment::presyn* local_synapses = presyns.find_output(s_gid);
    const environment::presyn* global_synapses = presyns.find_input(s_gid);
    if(local_synapses != NULL) {
        for(int i = 0; i<local_synapses->size(); ++i){
            const int gid = (*local_synapses)[i];
            outputs.push_back(gid);
        }
    }
    if(global_synapses != NULL) {
        for(int i = 0; i<global_synapses->size(); ++i){
            const int gid = (*global_synapses)[i];
            outputs.push_back(gid);
        }
    }
    return outputs;
}

BOOST_AUTO_TEST_CASE(nest_distri_event)
{
    int ncells = 2;
    int mindelay = 10;
    int nthreads = 1;
    int outgoing = 2;
    int simtime = 5*mindelay;
    //int nSpikes = 200;

    double delay = 1.0;


    int num_processes;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    namespace po = boost::program_options;
    po::variables_map vm;
    vm.insert(std::make_pair("nNeurons", po::variable_value(ncells, false)));
    vm.insert(std::make_pair("nThreads", po::variable_value(nthreads, false)));

    vm.insert(std::make_pair("model", po::variable_value(std::string("tsodyks2"), false)));
    vm.insert(std::make_pair("delay", po::variable_value(delay, false)));
    vm.insert(std::make_pair("weight", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("U", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("u", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("x", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("tau_rec", po::variable_value(1.0, false)));
    vm.insert(std::make_pair("tau_fac", po::variable_value(1.0, false)));

    nest::pool_env penv(nthreads);

    //preallocate vector for results
    std::vector<nest::spikedetector> detectors(ncells);
    std::vector<nest::targetindex> detectors_targetindex(ncells);
    // register spike detectors
    for(unsigned int i=0; i < detectors.size(); ++i) {
        //scheduler stores pointers to the spike detectors
        detectors_targetindex[i] = nest::scheduler::add_node(&detectors[i]);  //add them to the scheduler
    }

    //init neuron environment
    environment::continousdistribution neuro_dist(num_processes, rank, ncells);
    environment::presyn_maker presyns(outgoing, environment::fixedoutdegree);
    presyns(rank, &neuro_dist);

    //generate nest data structure out of it
    nest::connectionmanager cn(vm);

    for (unsigned int thrd=0; thrd<nthreads; thrd++) {
        environment::continousdistribution neuro_vp_dist(nthreads, thrd, &neuro_dist);
        nest::build_connections_from_neuron(thrd, neuro_vp_dist, presyns, detectors_targetindex, cn);
    }

    nest::eventdelivermanager edm(cn, num_processes, nthreads, mindelay);

    //create environment
    environment::event_generator generator(nthreads);
    environment::event_generator generator_compare(1);

    //create squential distribution: to get all events on all ranks
    environment::continousdistribution neuro_dist_compare(1, 0, ncells);

    //uniform distribution does not include a random generator: generate twice the same events
    environment::generate_uniform_events(generator.begin(), simtime, nthreads, 1, &neuro_dist);
    environment::generate_uniform_events(generator_compare.begin(), simtime, 1, 1, &neuro_dist_compare);

    nest::simulationmanager sm(edm, generator, rank, num_processes, nthreads);

    int events=0;
    for (unsigned int i=0; i<nthreads; i++)
        events += generator.get_size(i);

    for (unsigned int i=0; i<nthreads; i++)
        sm.update(i, 0, 0, mindelay);

    edm.gather_events();

    for (unsigned int i=0; i<nthreads; i++)
        edm.deliver_events(i, mindelay);

    for (unsigned int i=0; i<nthreads; i++)
        events -= generator.get_size(i);

    int spikes = 0;
    for(unsigned int i=0; i < detectors.size(); ++i)
        spikes += detectors[i].spikes.size();


    //check number of events over all ranks: receiving vs sending
    int all_spikes;
    MPI_Reduce(&spikes,&all_spikes,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    int all_events;
    MPI_Reduce(&events,&all_events,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    if (rank == 0)
        BOOST_CHECK_EQUAL(all_spikes, all_events*outgoing);

    //check if events were send properly
    int kept = 0;
    //generate on all nodes all spike events
    // get events from interval [0, mindelay)
    while(generator_compare.compare_top_lte(0, mindelay-1)) {
        environment::gen_event g = generator_compare.pop(0);
        //use local connections
        std::vector<int> targets = getTargets(presyns, g.first);
        //trace all connections
        for(unsigned int i=0; i < targets.size(); ++i) {
            unsigned int di = targets[i]%detectors.size();
            bool found=false;

            //try to find spike event in spike detectors
            for (std::vector<nest::spikeevent>::iterator it = detectors[di].spikes.begin(); it!=detectors[di].spikes.end();  it++) {
                const int sender = it->get_sender_gid();
                const nest::Time t = it->get_stamp();

                //compare all spike events
                nest::Time g_t(g.second);
                if (sender == g.first && std::abs(g_t.get_ms() - t.get_ms()) < 0.000001) {
                    //remove all matches from the spike detectors
                    detectors[di].spikes.erase(it);
                    found = true;
                    break;
                }
            }
            kept+=!found;
            if (!found)
                std::cout << "not found: sender=" << g.first << " t=" << g.second << std::endl;
        }
    }

    //all spikes should be removed from the spike detectors
    BOOST_CHECK_EQUAL(kept, 0);
    for (int i=0; i <detectors.size(); i++) {
        BOOST_CHECK_EQUAL(detectors[i].spikes.size(), 0);
    } 
}



