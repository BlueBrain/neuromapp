/*
 * connectionmanager.cpp
 *
 *  Created on: Jun 27, 2016
 *      Author: schumann
 */

#include "nest/synapse/connectionmanager.h"

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
            ConnectorBase* c = add_connection( conn, syn );
            connections_[ t ].set( s_gid, c );
        }
    }

    ConnectorBase*
    connectionmanager::validate_source_entry( thread tid, index s_gid)
    {
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

    ConnectorBase*
    connectionmanager::add_connection( ConnectorBase* conn, tsodyks2& syn )
    {
        if ( conn == 0 ){
          conn = allocate< Connector< 1, tsodyks2 > >( syn );
        }
        else {
            vector_like< tsodyks2 >* vc = static_cast< vector_like< tsodyks2 >* >( conn );
            conn = &vc->push_back( syn );
        }
    }
};

