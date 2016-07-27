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
#include "coreneuron_1.0/event_passing/environment/event_generators.hpp"

#include "neuromapp/utils/mpi/mpi_helper.h"

/** namespace alias for boost::program_options **/
namespace po = boost::program_options;

namespace nest
{

    enum subpragram {connection, connector, manager, distributed};

    /** \fn help(int argc, char *const argv[], po::variables_map& vm)
        \brief Helper using boost program option to facilitate the command line manipulation
        \param argc number of argument from the command line
        \param argv the command line from the driver or external call
        \param vm encapsulate the command line
        \return error message from mapp::mapp_error
     */
    int model_help(int argc, char* const argv[], po::variables_map& vm, subpragram& subprog)
    {
        std::string subprog_str;
        if (argc>1)
            subprog_str = argv[1];

        bool use_mpi = false;
        bool use_manager = false;
        bool use_connector = false;
        bool use_connection = false;

        if (argc >= 2 && subprog_str == "connection") {
            subprog = connection;
            use_connection = true;
        }
        else if (argc >= 2 && subprog_str == "connector") {
            subprog = connector;
            use_connector = true;
        }
        else if (argc >= 2 && subprog_str == "manager") {
            subprog = manager;
            use_manager = true;
        }
        else if (argc >= 2 && subprog_str == "distributed") {
            subprog = distributed;
            use_mpi = true;
        }
        else {
            std::cout << "subprogram could not be detected. Use --help for information" << std::endl;
        }
        po::options_description desc("Allowed options");

        desc.add_options()
        ("help", "produce help message");

        if (use_manager || use_mpi || use_connector || use_connection)
        desc.add_options()
        ("models", "list available connection models")
        ("model", po::value<std::string>()->default_value("tsodyks2"), "connection model")
        // tsodyks2 parameters
        ("delay", po::value<double>()->default_value(1.0), "delay")
        ("weight", po::value<double>()->default_value(1.0), "weight")
        ("U", po::value<double>()->default_value(0.5), "U")
        ("u", po::value<double>()->default_value(0.5), "u")
        ("x", po::value<double>()->default_value(1), "x")
        ("tau_rec", po::value<double>()->default_value(800.0), "tau_rec")
        ("tau_fac", po::value<double>()->default_value(0.0), "tau_fac")
        ("nSpikes", po::value<int>()->default_value(2), "total number of spikes");

        if (use_manager || use_mpi || use_connector)
            desc.add_options()
            ("pool", po::value<bool>()->default_value(false), "pool memory manager") //memory pool for hte connector
            ("fanout", po::value<int>()->default_value(1), "number of incoming(manager)/outgoing(connector) connections");

        if (use_manager || use_mpi)
            desc.add_options()
            ("min_delay", po::value<int>()->default_value(2), "min delay of simulation")
            ("nThreads", po::value<int>()->default_value(1), "number of threads")
            ("nProcesses", po::value<int>()->default_value(1), "number of ranks")
            ("simtime", po::value<int>()->default_value(4), "simulation time")
            ("nNeurons", po::value<int>()->default_value(10), "number of neurons");

        if (use_mpi)
            desc.add_options()
            ("run", po::value<std::string>()->default_value("/usr/bin/mpiexec"), "mpi run command")
            ("rate", po::value<double>()->default_value(-1), "firing rate per neuron");

        if (use_manager)
            desc.add_options()
            ("manager", "encapsulate connectors in connection manager")
            ("rank", po::value<int>()->default_value(0), "fake rank id")
            ("thread", po::value<int>()->default_value(0), "fake thread id");

        if (use_connector)
            desc.add_options()
            ("connector", "encapsulate connections in connector");

        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);


        //check for all
        if (use_mpi || use_manager || use_connector || use_connection)
        if (vm["nSpikes"].as<int>() <= 0 ) {
            std::cout << "Error: nSpikes has to be greater than 0" << std::endl;
            return mapp::MAPP_BAD_DATA;
        }
        //check for multiple
        if (use_mpi || use_manager || use_connector) {
            if (vm["fanout"].as<int>() <= 0) {
                std::cout << "Error: Number of connections has to be greater than 0" << std::endl;
                return mapp::MAPP_BAD_DATA;
            }
        }

        //check for multiple
        if (use_mpi || use_manager) {
            if (vm["nThreads"].as<int>() <= 0) {
                std::cout << "Error: nThreads has to be greater than 0" << std::endl;
                return mapp::MAPP_BAD_DATA;
            }
            if (vm["nProcesses"].as<int>() <= 0) {
                std::cout << "Error: nProcesses has to be greater than 0" << std::endl;
                return mapp::MAPP_BAD_DATA;
            }
            if (vm["min_delay"].as<int>() <= 0) {
                std::cout << "Error: min_delay has to be greater than 0" << std::endl;
                return mapp::MAPP_BAD_DATA;
            }
            if (vm["simtime"].as<int>() <= 0 || (vm["simtime"].as<int>() % vm["min_delay"].as<int>()) != 0) {
                std::cout << "Error: simtime has to be a multiple of min_delay" << std::endl;
                return mapp::MAPP_BAD_DATA;
            }
            if (vm["nNeurons"].as<int>() <= 0) {
                std::cout << "Error: nNeurons has to be greater than 0" << std::endl;
                return mapp::MAPP_BAD_DATA;
            }
        }

        if (use_manager) {
            if (vm["nProcesses"].as<int>() <= vm["rank"].as<int>()) {
                std::cout << "Error: rank has to be smaller than nProcesses" << std::endl;
                return mapp::MAPP_BAD_DATA;
            }
            if (vm["nThreads"].as<int>() <= vm["thread"].as<int>()) {
                std::cout << "Error: thread has to be smaller than nThreads" << std::endl;
                return mapp::MAPP_BAD_DATA;
            }
        }

        //check for valid synapse model & parameters
        if (use_mpi || use_manager || use_connector || use_connection)
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


        //list available synapse models
        if (use_mpi || use_manager || use_connector || use_connection)
        if (vm.count("models")){
            std::cout << "   Following connection models are available: \n";
            std::cout << "       name           list of accepted parameters\n";
            std::cout << "       tsodyks2       delay, weight, U, u, x, tau_rec, tau_fac\n";
                std::cout << "";
                return mapp::MAPP_USAGE;
            }

        if (vm.count("help")){
            if (use_manager || use_mpi || use_connector || use_connection)
                std::cout << desc;
            else
                std::cout << "chose subprogram: connection, connector, manager or distributed" << std::endl;
            return mapp::MAPP_USAGE;
        }

        if (use_manager || use_mpi || use_connector || use_connection){
            return mapp::MAPP_OK;
        }
        else{
            return mapp::MAPP_USAGE;
        }
    }

    /** \fn content(po::variables_map const& vm)
        \brief Execute the NEST synapse Miniapp.
        \param vm encapsulate the command line and all needed informations
     */
    void model_content(po::variables_map const& vm, subpragram& subprog)
    {
        int nSpikes = vm["nSpikes"].as<int>();

        bool use_connection = subprog == connection;
        bool use_connector = subprog == connector;
        bool use_manager = subprog == manager;
        bool use_mpi = subprog == distributed;

        if (use_mpi) {
            std::stringstream command;

            std::string path = helper_build_path::mpi_bin_path();

            size_t nthread = vm["nThreads"].as<int>();
            std::string mpi_run = vm["run"].as<std::string>();
            size_t nproc = vm["nProcesses"].as<int>();

            //command line args

            size_t ncells = vm["nNeurons"].as<int>();
            size_t fan = vm["fanout"].as<int>();
            size_t mindelay = vm["min_delay"].as<int>();
            size_t simtime = vm["simtime"].as<int>();
            
            double rate = vm["rate"].as<double>();
            if (rate>=0) {
                nSpikes = rate * ncells * simtime;
                std::cout << "WARNING: nSpikes is overwritten by rate. new value of nSpikes=" << nSpikes << std::endl; 
            }
            std::string syn_model = vm["model"].as<std::string>();
            double syn_delay = vm["delay"].as<double>();
            double syn_weight = vm["weight"].as<double>();
            double syn_U = vm["U"].as<double>();
            double syn_u = vm["u"].as<double>();
            double syn_x = vm["x"].as<double>();
            double syn_tau_rec = vm["tau_rec"].as<double>();
            double syn_tau_fac = vm["tau_fac"].as<double>();
            bool pool = vm["pool"].as<bool>();

            std::string exec ="nest_dist_exec";

            command << "OMP_NUM_THREADS=" << nthread << " " <<
                mpi_run <<" -n "<< nproc << " " << path << exec <<
                " " << nthread << " " << simtime << " " <<
                ncells << " " << fan << " " <<
                nSpikes << " " << mindelay << " " <<
                syn_model << " " << syn_delay << " " <<
                syn_weight << " " << syn_U << " " <<
                syn_u << " " << syn_x << " " <<
                syn_tau_rec << " " << syn_tau_fac << " " << pool;

            std::cout<< "Running command " << command.str() <<std::endl;
            system(command.str().c_str());

            return;
        }

        boost::chrono::system_clock::duration delay;


        if ( use_manager ) {
            const bool pool = vm["pool"].as<bool>();
            const int thrd = vm["thread"].as<int>(); // thead_num
            const int min_delay=vm["min_delay"].as<int>();
            const int simtime = vm["simtime"].as<int>();
            const int nthreads = vm["nThreads"].as<int>();
            const int rank = vm["rank"].as<int>();
            const int size = vm["nProcesses"].as<int>();
            const int ncells = vm["nNeurons"].as<int>();
            const int fanout = vm["fanout"].as<int>();

            //setup allocator
            nest::pool_env penv(nthreads, pool);

            //build connection manager
            connectionmanager cm(vm);
            environment::continousdistribution neuro_dist(size, rank, ncells);
            environment::presyn_maker presyns(fanout, environment::fixedoutdegree);
            presyns(thrd, &neuro_dist);

            //preallocate vector for results
            std::vector<spikedetector> detectors(ncells);
            std::vector<targetindex> detectors_targetindex(ncells);

            // register spike detectors
            for(unsigned int i=0; i < ncells; ++i) {
                detectors[i].set_lid(i);    //give nodes a local id
                //scheduler stores pointers to the spike detectors
                detectors_targetindex[i] = scheduler::add_node(&detectors[i]);  //add them to the scheduler
            }

            environment::continousdistribution neuro_vp_dist(nthreads, thrd, &neuro_dist);
            build_connections_from_neuron(thrd, neuro_vp_dist, presyns, detectors_targetindex, cm);

            // generate all events for one thread
            environment::event_generator generator(1);
            double mean = static_cast<double>(simtime) / static_cast<double>(nSpikes);
            double lambda = 1.0 / static_cast<double>(mean * size);

            //all events available
            environment::continousdistribution event_dist(1, 0, ncells);

            environment::generate_poisson_events(generator.begin(),
                             simtime, nthreads, rank, size, lambda, &event_dist);

            const unsigned int stats_generated_spikes = generator.get_size(0);
            int t = 0;
            spikeevent se;
            boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();
            while (t<simtime) {
                t+=min_delay;
                // get events from all threads
                while(generator.compare_top_lte(0, t)) {
                    environment::gen_event g = generator.pop(0);
                    index nid = g.first;
                    se.set_stamp( Time(g.second) ); // in Network::send< SpikeEvent >
                    se.set_sender_gid( nid ); // in Network::send< SpikeEvent >
                    cm.send(thrd, nid, se); //send spike
                }
            }
            delay = boost::chrono::system_clock::now() - start;
            std::cout << "Connection manager simulated" << std::endl;
            std::cout << "Statistics:" << std::endl;
            std::cout << "\tgenerated spikes: " << stats_generated_spikes << std::endl;
            int recvSpikes=0;
            for (unsigned int i=0; i<detectors.size(); i++)
                recvSpikes+=detectors[i].spikes.size();
            std::cout << "\trecv spikes: " << recvSpikes << std::endl;

            std::cout << "\tEvents left:" << std::endl;

            while (!generator.empty(0)) {  // thread 0
                environment::gen_event g = generator.pop(0); // thread 0
                std::cout << "Event " << g.first << " " << g.second << std::endl;
            }
        }
        else if ( use_connector ) {
            const bool pool = vm["pool"].as<bool>();
            const double syn_delay = vm["delay"].as<double>();
            const double syn_weight = vm["weight"].as<double>();
            const double syn_U = vm["U"].as<double>();
            const double syn_u = vm["u"].as<double>();
            const double syn_x = vm["x"].as<double>();
            const double syn_tau_rec = vm["tau_rec"].as<double>();
            const double syn_tau_fac = vm["tau_fac"].as<double>();

            const int fanout = vm["fanout"].as<int>();

            //setup allocator
            nest::pool_env penv(1, pool); // use one thread

            //preallocate vector for results
            std::vector<spikedetector> detectors(fanout);
            std::vector<targetindex> detectors_targetindex(fanout);
            for(unsigned int i=0; i < fanout; ++i) {
                detectors[i].set_lid(i);    //give nodes a local id
                //scheduler stores pointers to the spike detectors
                detectors_targetindex[i] = scheduler::add_node(&detectors[i]);  //add them to the scheduler
            }

            //create connector ptr
            //has to be set to NULL (check add_connection(..))
            ConnectorBase* conn = NULL;

            //build connector
            for(unsigned int i=0; i < fanout; ++i) {
                //TODO permute parameters
                tsodyks2 synapse(syn_delay, syn_weight, syn_U, syn_u, syn_x, syn_tau_rec, syn_tau_fac, detectors_targetindex[i%fanout]);
                conn = add_connection(conn, synapse); //use static function from connectionmanager
            }

            //create a few events
            std::vector< spikeevent > events(nSpikes);
            for (unsigned int i=0; i<nSpikes; i++) {
                Time t(i*10.0);
                events[i].set_stamp( t ); // in Network::send< SpikeEvent >
                events[i].set_sender( NULL ); // in Network::send< SpikeEvent >
            }

            boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();
            for (unsigned int i=0; i<nSpikes; i++) {
                conn->send(events[i]); //send spike
            }

            delay = boost::chrono::system_clock::now() - start;
            std::cout << "Connector simulated with " << fanout << " connections" << std::endl;
        }
        else {
            const double syn_delay = vm["delay"].as<double>();
            const double syn_weight = vm["weight"].as<double>();
            const double syn_U = vm["U"].as<double>();
            const double syn_u = vm["u"].as<double>();
            const double syn_x = vm["x"].as<double>();
            const double syn_tau_rec = vm["tau_rec"].as<double>();
            const double syn_tau_fac = vm["tau_fac"].as<double>();

            //preallocate vector for results
            spikedetector detector;
            // register spike detectors
            targetindex detector_targetindex = scheduler::add_node(&detector);  //add them to the scheduler

            tsodyks2 syn(syn_delay, syn_weight, syn_U, syn_u, syn_x, syn_tau_rec, syn_tau_fac, detector_targetindex);

            //create a few events
            std::vector< spikeevent > events(nSpikes);
            for (unsigned int i=0; i<nSpikes; i++) {
                Time t(i*10.0);
                events[i].set_stamp( t ); // in Network::send< SpikeEvent >
                events[i].set_sender( NULL ); // in Network::send< SpikeEvent >
            }

            double t_lastspike = 0.0;
            boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();
            for (unsigned int i=0; i<nSpikes; i++) {
                syn.send(events[i], t_lastspike); //send spike
                t_lastspike += 0.2; // dt - time between spiks
            }
            delay = boost::chrono::system_clock::now() - start;
            std::cout << "Single connection simulated" << std::endl;
            std::cout << "Last weight " << detector.spikes.back().get_weight() << std::endl;
        }

        std::cout << "Duration: " << delay << std::endl;

    }

    int model_execute(int argc, char* const argv[])
    {
        try {
            po::variables_map vm; // it contains everything
            subpragram subprog;
            if(int error = model_help(argc, argv, vm, subprog)) return error;
            model_content(vm, subprog); // execute the miniapp
        }
        catch(std::exception& e){
            std::cout << e.what() << "\n";
            return mapp::MAPP_UNKNOWN_ERROR;
        }
        return mapp::MAPP_OK; // 0 ok, 1 not ok
    }
}
