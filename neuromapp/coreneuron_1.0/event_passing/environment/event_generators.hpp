#ifndef MAPP_ENV_GENERATORS_HPP
#define MAPP_ENV_GENERATORS_HPP

#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random.hpp>

#include "coreneuron_1.0/event_passing/environment/generator.h"

namespace environment {

    /** \fn void generate_events_kai(Iterator beg, int simtime, int ngroups, int rank, int nprocs, int ncells, double lambda)
        \brief Kai's original implementation: generates events according to an exponential law.
	\param Iterator beg
    
	This implementation generates an event per timestep, and attributes it to a random cell in the network. It then samples a survival interval using an exponential distribution, to determine when the next event will occur.
	\warning this implementation lacks a biological implementation, since no two events can happen at the same time. This would imply that, in the whole network, only one cell fired at a time.




    */
    template< typename Iterator >
    void generate_events_kai(Iterator beg, int simtime, int ngroups, int rank, int nprocs, int ncells, double lambda);

    template< typename Iterator >
    void generate_poisson_events(Iterator beg, int simtime, int ngroups, int rank, int nprocs, int ncells, double lambda);

    template< typename Iterator >
    void generate_uniform_events(Iterator beg, int simtime, int ngroups, int rank, int nprocs, int ncells, double firing_interval);

}

#include "coreneuron_1.0/event_passing/environment/event_generators.ipp"

#endif
