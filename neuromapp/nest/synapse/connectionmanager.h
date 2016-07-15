/*
 * connectionmanager.h
 *
 *  Created on: Jun 27, 2016
 *      Author: schumann
 */

#ifndef CONNECTIONMANAGER_H_
#define CONNECTIONMANAGER_H_

#include <boost/program_options.hpp>
#include "nest/synapse/connector_base.h"
#include "nest/libnestutil/sparsetable.h"
#include "nest/synapse/models/tsodyks2.h"
#include "nest/synapse/event.h"
#include "nest/synapse/memory.h"

#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"

namespace po = boost::program_options;

namespace nest
{

    typedef google::sparsetable< ConnectorBase* > tSConnector; // for all neurons having targets
    typedef std::vector< tSConnector > tVSConnector;           // for all threads

    class connectionmanager {

    private:
        int ncells;
        po::variables_map const& vm;


        ConnectorBase* validate_source_entry( thread tid, index s_gid);
    public:
        tVSConnector connections_;

        connectionmanager(po::variables_map const& vm);
        void connect(thread t, index s_gid, targetindex target);
        void send( thread t, index sgid, event& e );
    };

    void build_connections_from_neuron(const thread& thrd,
                                       const environment::continousdistribution& neuro_vp_dist,
                                       const environment::presyn_maker& presyns,
                                       const std::vector<targetindex>& detectors_targetindex,
                                       connectionmanager& cm);
};


#endif /* CONNECTIONMANAGER_H_ */
