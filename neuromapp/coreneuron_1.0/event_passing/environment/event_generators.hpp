#ifndef MAPP_ENV_GENERATORS_HPP
#define MAPP_ENV_GENERATORS_HPP

#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random.hpp>

#include "coreneuron_1.0/event_passing/environment/generator.h"

namespace environment {

    template< typename Iterator >
    void generate_events_kai(Iterator beg, Iterator end,  int simtime, int ngroups, int rank, int nprocs, int ncells, double lambda);

}

#include "coreneuron_1.0/event_passing/environment/event_generators.ipp"

#endif
