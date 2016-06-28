/*
 * Neuromapp - main.cpp, Copyright (c), 2015,
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
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/nest/synapse/drivers/model_main.cpp
 * \brief NEST synapse model Miniapp
 */

#include <iostream>
#include <string>


#include <boost/program_options.hpp>
#include <boost/chrono.hpp>

#include "nest/synapse/drivers/synapse.h"
#include "nest/synapse/event.h"
#include "nest/synapse/scheduler.h"
#include "nest/synapse/models/tsodyks2.h"
#include "nest/synapse/connector_base.h"
#include "nest/synapse/connectionmanager.h"
#include "utils/error.h"

#include "coreneuron_1.0/event_passing/environment/generator.h"
#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"

/** namespace alias for boost::program_options **/
namespace po = boost::program_options;
using namespace environment; 
namespace nest
{
    /** \fn help(int argc, char *const argv[], po::variables_map& vm)
        \brief Helper using boost program option to facilitate the command line manipulation
        \param argc number of argument from the command line
        \param argv the command line from the driver or external call
        \param vm encapsulate the command line
        \return error message from mapp::mapp_error
     */
    int model_help(int argc, char* const argv[], po::variables_map& vm)
    {
        po::options_description desc("Allowed options");
        desc.add_options()
        ("help", "produce help message")
        ("models", "list available connection models")
        ("connector", "encapsulate connections in connector")
        ("nConnections", po::value<int>()->default_value(1), "number of incoming(manager)/outgoing(connector) connections")
        ("nDetectors", po::value<int>()->default_value(1), "number of spike detectors")


        ("manager", "encapsulate connectors in connection manager")
        ("nNeurons", po::value<int>()->default_value(1), "number of neurons")
        ("min_delay", po::value<int>()->default_value(2), "min delay of simulation")
        ("nSpikes", po::value<int>()->default_value(2), "total number of spikes")
        ("nGroups", po::value<int>()->default_value(1), "theoretical number of threads")
        ("size", po::value<int>()->default_value(1), "theoretical number of ranks")
        ("rank", po::value<int>()->default_value(0), "theoretical rank id")
        ("thread", po::value<int>()->default_value(0), "theoretical thread id")

        ("model", po::value<std::string>()->default_value("tsodyks2"), "connection model")

        // tsodyks2 parameters
        // synapse parameters are not checked
        ("delay", po::value<double>()->default_value(1.0), "delay")
        ("weight", po::value<double>()->default_value(1.0), "weight")
        ("U", po::value<double>()->default_value(0.5), "U")
        ("u", po::value<double>()->default_value(0.5), "u")
        ("x", po::value<double>()->default_value(1), "x")
        ("tau_rec", po::value<double>()->default_value(800.0), "tau_rec")
        ("tau_fac", po::value<double>()->default_value(0.0), "tau_fac")
        //memory pool for hte connector
        ("pool", po::value<bool>()->default_value(false), "pool memory manager")

        // simulation parameters
        ("dt", po::value<double>()->default_value(0.1), "time between spikes")
        ("iterations", po::value<int>()->default_value(1), "number of iterations (spikes)");


        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("manager")) {
            if (vm["nGroups"].as<int>() <= vm["thread"].as<int>()) {
                std::cout << "Error: thread has to be smaller than number of threads" << std::endl;
                return mapp::MAPP_BAD_DATA;
            }
            if (vm["size"].as<int>() <= vm["rank"].as<int>()) {
                std::cout << "Error: rank has to be smaller than number of ranks" << std::endl;
                return mapp::MAPP_BAD_DATA;
            }
            if (vm["min_delay"].as<int>() <= 0) {
                std::cout << "Error: min_delay has to be greater than 0" << std::endl;
                return mapp::MAPP_BAD_DATA;
            }
            if (vm["nSpikes"].as<int>() <= 0) {
                std::cout << "Error: nSpikes has to be greater than 0" << std::endl;
                return mapp::MAPP_BAD_DATA;
            }
            if (vm["nNeurons"].as<int>() <= 0) {
                std::cout << "Error: nNeurons has to be greater than 0" << std::endl;
                return mapp::MAPP_BAD_DATA;
            }
        }
        else if (vm.count("connector")) {
            if (vm["nConnections"].as<int>() <= 0) {
                std::cout << "Error: Number of connections per connector has to be greater than 0" << std::endl;
                return mapp::MAPP_BAD_DATA;
            }
        }
        else {
            if (vm["nConnections"].as<int>() != 1) {
                std::cout << "Error: Encapsulate connections in connector to enable multiple connections" << std::endl;
                return mapp::MAPP_BAD_DATA;
            }
        }

        //check for valid synapse model & parameters
        if (vm["model"].as<std::string>() == "tsodyks2") {
            const double delay = vm["delay"].as<double>();
            const double weight = vm["weight"].as<double>();
            const double U = vm["U"].as<double>();
            const double u = vm["u"].as<double>();
            const double x = vm["x"].as<double>();
            const double tau_rec = vm["tau_rec"].as<double>();
            const double tau_fac = vm["tau_fac"].as<double>();

            try {
                short lid = 0; // only one node
                spikedetector sd;
                tsodyks2 syn(delay, weight, U, u, x, tau_rec, tau_fac, lid);
            }
            catch (std::invalid_argument& e) {
                std::cout << "Error in model parameters: " << e.what() << std::endl;
                return mapp::MAPP_BAD_DATA;
            }
        }
        /* else if ( more models ) */
        else {
            std::cout << "Error: Selected connection model is  unknown" << std::endl;
            return mapp::MAPP_BAD_DATA;
        }
        //check for valid dt
        if (vm["dt"].as<double>() < 0.1) {
            std::cout << "Error: Time between spikes has to be bigger than 0.1" << std::endl;
            return mapp::MAPP_BAD_DATA;
        }
        //check for valid iterations
        if (vm["iterations"].as<int>() < 1) {
            std::cout << "Error: Number of iterations has to be a greater than 0" << std::endl;
            return mapp::MAPP_BAD_DATA;
        }
        //list available synapse models
        if (vm.count("models")){
            std::cout << "   Following connection models are available: \n";
            std::cout << "       name           list of accepted parameters\n";
            std::cout << "       tsodyks2       delay, weight, U, u, x, tau_rec, tau_fac\n";
                std::cout << "";
                return mapp::MAPP_USAGE;
            }

        if (vm.count("help")){
            std::cout << desc;
            return mapp::MAPP_USAGE;
        }
        return mapp::MAPP_OK;
    }


    /*
     * \fn build_connections_from_neuron(std::vector<targetindex>& detectors_targetindex, connectionmanager& cn, po::variables_map const& vm)
     * \brief build connections in connection manager using generator from coreneuron miniapp
     * \param detectors_targetindex vector of targetindexes to target nodes
     * \param cn reference to connection manager
     * \param vm refrence to boost variables map
     */
    void build_connections_from_neuron(std::vector<targetindex>& detectors_targetindex, connectionmanager& cn, po::variables_map const& vm) {
        const int size = vm["size"].as<int>(); //get all connections for all nodes
        const int rank = vm["rank"].as<int>();
        const int t = vm["thread"].as<int>(); // thread_num
        const int ngroups = vm["nGroups"].as<int>(); //one thread available
        const int fan = vm["nConnections"].as<int>();
        const int ncells = vm["nNeurons"].as<int>();

        //environment::event_generator generator(nSpikes, simtime, ngroups, rank, size, ncells);
        presyn_maker presyns(ncells, fan, fixedoutdegree);
        presyns(size, ngroups, rank);

        int n_local_connections = 0;
        for (unsigned int s_gid=0; s_gid<ncells; s_gid++) {

            const environment::presyn* local_synapses = presyns.find_output(s_gid);
            if(local_synapses != NULL) {
                for(int i = 0; i<local_synapses->size(); ++i){
                   const unsigned int dest = (*local_synapses)[i] % ngroups;
                   if(dest == t) {
                       targetindex target = detectors_targetindex[n_local_connections%detectors_targetindex.size()];
                       cn.connect(t, s_gid, target);
                       n_local_connections++;
                   }
                }
            }
            const environment::presyn* global_synapses = presyns.find_input(s_gid);
            if(global_synapses != NULL) {
                for(int i = 0; i<global_synapses->size(); ++i){
                    const unsigned int dest = (*global_synapses)[i] % ngroups;
                   if(dest == t) {
                       targetindex target = detectors_targetindex[n_local_connections%detectors_targetindex.size()];
                       cn.connect(t, s_gid, target);
                       n_local_connections++;
                   }
                }
            }
        }
    }

    /** \fn content(po::variables_map const& vm)
        \brief Execute the NEST synapse Miniapp.
        \param vm encapsulate the command line and all needed informations
     */
    void model_content(po::variables_map const& vm)
    {
        PoorMansAllocator poormansallocpool;
        double dt = vm["dt"].as<double>();
        int iterations = vm["iterations"].as<int>();
        const int num_connections = vm["nConnections"].as<int>();
        const int num_detectors = vm["nDetectors"].as<int>();
        bool with_connector = vm.count("connector") > 0;
        bool with_manager = vm.count("manager") > 0;

        //will turn into ptr to base class if more synapse are implemented
        tsodyks2* syn;
        ConnectorBase* conn = NULL;

        //preallocate vector for results
        std::vector<spikedetector> detectors(num_detectors);
        std::vector<targetindex> detectors_targetindex(num_detectors);

        // register spike detectors

        connectionmanager* cn = NULL;

        for(unsigned int i=0; i < num_detectors; ++i) {
            detectors[i].set_lid(i);    //give nodes a local id
            //scheduler stores pointers to the spike detectors
            detectors_targetindex[i] = scheduler::add_node(&detectors[i]);  //add them to the scheduler
        }

        if (vm["model"].as<std::string>() == "tsodyks2") {
            const double delay = vm["delay"].as<double>();
            const double weight = vm["weight"].as<double>();
            const double U = vm["U"].as<double>();
            const double u = vm["u"].as<double>();
            const double x = vm["x"].as<double>();
            const double tau_rec = vm["tau_rec"].as<double>();
            const double tau_fac = vm["tau_fac"].as<double>();
            const bool pool = vm["pool"].as<bool>();
            if(pool){
                poormansallocpool.states = pool;
            }
            if ( with_manager ) {
                //build connection manager
                cn = new connectionmanager(vm);
                build_connections_from_neuron(detectors_targetindex, *cn, vm);
            }
            else if ( with_connector ) {
                //build connector
                for(unsigned int i=0; i < num_connections; ++i) {
                    //TODO permute parameters
                    tsodyks2 synapse(delay, weight, U, u, x, tau_rec, tau_fac, detectors_targetindex[i%num_detectors]);
                    conn = add_connection(conn, synapse); //use static function from connectionmanager
                }
            }
            else {
                syn = new tsodyks2(delay, weight, U, u, x, tau_rec, tau_fac, detectors_targetindex[0]);
            }
        }
        /* else if () .. further synapse models*/
        else {
            throw std::invalid_argument("connection model implementation missing");
        }
        //create a few events
        std::vector< spikeevent > events(iterations);
        for (unsigned int i=0; i<iterations; i++) {
            Time t(i*10.0);
            events[i].set_stamp( t ); // in Network::send< SpikeEvent >
            events[i].set_sender( NULL ); // in Network::send< SpikeEvent >
            //events[i]->set_sender_gid( sgid ); // Network::send_local
        }

        boost::chrono::system_clock::duration delay;

        if ( with_manager ) {
            if (cn==NULL) {
                throw std::runtime_error("connectionmanager pointer is not valid");
            }
            const int t = vm["thread"].as<int>(); // thead_num
            const int min_delay=vm["min_delay"].as<int>();
            const int nSpikes = vm["nSpikes"].as<int>();
            const int simtime = iterations * min_delay;
            const int ngroups = vm["nGroups"].as<int>();
            const int rank = vm["rank"].as<int>();
            const int size = vm["size"].as<int>();
            const int ncells = vm["nNeurons"].as<int>();
            environment::event_generator generator(nSpikes, simtime, ngroups, rank, size, ncells);

            std::cout << "Real generated spikes: " << generator.get_size(t) << std::endl;
            int sim_time = 0;
            spikeevent se;

            boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();
            for (unsigned int i=0; i<iterations; i++) {
                sim_time+=min_delay;
                while(generator.compare_top_lte(t, sim_time)){
                    environment::gen_event g = generator.pop(t);
                    index nid = g.first;
                    se.set_stamp( Time(g.second) ); // in Network::send< SpikeEvent >
                    se.set_sender_gid( nid ); // in Network::send< SpikeEvent >

                    std::cout << "Event " << g.first << " " << g.second << std::endl;
                    cn->send(t, nid, se); //send spike
                }
            }
            delay = boost::chrono::system_clock::now() - start;
            std::cout << "Connection manager simulated" << std::endl;
            std::cout << "Statistics:" << std::endl;
            std::cout << "\tnumber of send spikes: " << nSpikes << std::endl;
            int recvSpikes=0;
            for (unsigned int i=0; i<detectors.size(); i++)
                recvSpikes+=detectors[i].spikes.size();
            std::cout << "\tnumber of recv spikes: " << recvSpikes << std::endl;

            std::cout << "\tEvents left:" << std::endl;
            while (!generator.empty(t)) {
                environment::gen_event g = generator.pop(t);
                std::cout << "Event " << g.first << " " << g.second << std::endl;
            }

            delete cn;
        }
        else if ( with_connector ) {
            if (conn==NULL) {
                throw std::runtime_error("connector pointer is not valid");
            }
            boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();
            for (unsigned int i=0; i<iterations; i++) {
                conn->send(events[i]); //send spike
            }
            delay = boost::chrono::system_clock::now() - start;

            std::cout << "Connector simulated with " << num_connections << " connections" << std::endl;
        }
        else {
            if (!syn) {
                throw std::runtime_error("connection pointer is not valid");
            }
            double t_lastspike = 0.0;
            boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();
            for (unsigned int i=0; i<iterations; i++) {
                syn->send(events[i], t_lastspike); //send spike
                t_lastspike += dt;
            }
            delay = boost::chrono::system_clock::now() - start;
            std::cout << "Single connection simulated" << std::endl;
            delete syn;
        }

        std::cout << "Duration: " << delay << std::endl;
        std::cout << "Last weight " << detectors[0].spikes.back().get_weight() << std::endl;
    }

    int model_execute(int argc, char* const argv[])
    {
        try {
            po::variables_map vm; // it contains everything
            if(int error = model_help(argc, argv, vm)) return error;
            model_content(vm); // execute the miniapp
        }
        catch(std::exception& e){
            std::cout << e.what() << "\n";
            return mapp::MAPP_UNKNOWN_ERROR;
        }
        return mapp::MAPP_OK; // 0 ok, 1 not ok
    }
}
