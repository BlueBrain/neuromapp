/*
 * connectionmanager.cpp
 *
 *  Created on: Jun 27, 2016
 *      Author: schumann
 */

#include "nest/synapse/connectionmanager.h"
#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"

namespace nest {
    connectionmanager::connectionmanager(po::variables_map const& vm):
        vm(vm)
    {
        ncells = vm["nNeurons"].as<int>();
        const int num_threads = vm["nGroups"].as<int>();
        tVSConnector tmp( num_threads, tSConnector() );
        connections_.swap( tmp );
    }

    void
    connectionmanager::send( thread t, index sgid, event& e )
    {
      if ( sgid < connections_[ t ].size() ) // probably test only fails, if there are no connections
        if ( connections_[ t ].get( sgid ) != 0 ) // only send, if connections exist
          connections_[ t ].get( sgid )->send( e );
    }

    void
    connectionmanager::connect(thread t, index s_gid, targetindex target)
    {
        if (vm["model"].as<std::string>() == "tsodyks2") {
            const double delay = vm["delay"].as<double>();
            const double weight = vm["weight"].as<double>();
            const double U = vm["U"].as<double>();
            const double u = vm["u"].as<double>();
            const double x = vm["x"].as<double>();
            const double tau_rec = vm["tau_rec"].as<double>();
            const double tau_fac = vm["tau_fac"].as<double>();
            //TODO permute parameters
            tsodyks2 syn(delay, weight, U, u, x, tau_rec, tau_fac, target);

            ConnectorBase* conn = validate_source_entry( t, s_gid);
            ConnectorBase* c = add_connection<tsodyks2>( conn, syn );
            connections_[ t ].set( s_gid, c );
        }
        else {
            throw std::invalid_argument("synapse model unknown");
        }
    }

    ConnectorBase*
    connectionmanager::validate_source_entry( thread tid, index s_gid)
    {
      assert(s_gid<ncells);
      // resize sparsetable to full network size
      if ( connections_[ tid ].size() < ncells )
        connections_[ tid ].resize( ncells );

      // check, if entry exists
      // if not put in zero pointer
      if ( connections_[ tid ].test( s_gid ) )
        return connections_[ tid ].get(
          s_gid ); // returns non-const reference to stored type, here ConnectorBase*
      else
        return 0; // if non-existing
    }


    /*
     * \fn build_connections_from_neuron(std::vector<targetindex>& detectors_targetindex, connectionmanager& cn, po::variables_map const& vm)
     * \brief build connections in connection manager using generator from coreneuron miniapp
     * \param detectors_targetindex vector of targetindexes to target nodes
     * \param cn reference to connection manager
     * \param vm refrence to boost variables map
     */
    void build_connections_from_neuron(std::vector<targetindex>& detectors_targetindex, connectionmanager& cm, po::variables_map const& vm) {
        const int size = vm["size"].as<int>(); //get all connections for all nodes
        const int rank = vm["rank"].as<int>();
        const int t = vm["thread"].as<int>(); // thread_num
        const int ngroups = vm["nGroups"].as<int>(); //one thread available
        const int fan = vm["nConnections"].as<int>();
        const int ncells = vm["nNeurons"].as<int>();

        //environment::event_generator generator(nSpikes, simtime, ngroups, rank, size, ncells);
        environment::presyn_maker presyns(ncells, fan, environment::fixedoutdegree);
        presyns(size, ngroups, rank);

        for (unsigned int s_gid=0; s_gid<ncells; s_gid++) {

            const environment::presyn* local_synapses = presyns.find_output(s_gid);
            if(local_synapses != NULL) {
                for(int i = 0; i<local_synapses->size(); ++i){
                   const unsigned int t_gid = (*local_synapses)[i];
                   //sort out locally stored connections
                   const unsigned int dest = t_gid % (ngroups * size);
                   if(dest == t) {
                       //local id out of global id
                       const unsigned int t_lid = t_gid / (ngroups * size);
                       targetindex target = detectors_targetindex[t_lid%detectors_targetindex.size()];
                       cm.connect(t, s_gid, target);
                   }
                }
            }
            const environment::presyn* global_synapses = presyns.find_input(s_gid);
            if(global_synapses != NULL) {
                for(int i = 0; i<global_synapses->size(); ++i){
                    const unsigned int t_gid = (*global_synapses)[i];
                    //sort out locally stored connections
                    const unsigned int dest = t_gid % (ngroups * size);
                    if(dest == t) {
                        //local id out of global id
                        const unsigned int t_lid = t_gid / (ngroups * size);
                        targetindex target = detectors_targetindex[t_lid%detectors_targetindex.size()];
                        cm.connect(t, s_gid, target);
                    }
                }
            }
        }
    }
};

