#include <boost/range/algorithm_ext/iota.hpp>
#include <iostream>
#include <time.h>
#include <ctime>
#include <cassert>
#include <numeric>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random.hpp>

#include "coreneuron_1.0/event_passing/environment/generator.h"
#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"

namespace environment {

event_generator::event_generator(int nSpikes, int simtime,
int ngroups, int rank, int nprocs, int ncells){
    int dest = 0;
    double event_time = 0;
    int src_gid = 0;
    double percent = 0;
    gen_event new_event;
    presyn* output;

    assert(nSpikes > 0);
    int cells_per = ncells / nprocs;
    int start = rank * cells_per;

    //for the last rank, add the remaining cellgroups
    if(rank == nprocs)
        cells_per = ngroups - start;

    double mean = static_cast<double>(simtime) / static_cast<double>(nSpikes);
    double lambda = 1.0 / static_cast<double>(mean * nprocs);

    //create random number generator/distributions
    /*
     * rng       => random number generator
     * time_d    => exponential distribution of event times
     * gid_d     => uniform distribution to create indices for output gids
     * percent_d => uniform distribution to decide event type based on percent
     */
    boost::mt19937 rng(rank + time(NULL));
    boost::random::exponential_distribution<double> time_d(lambda);
    boost::random::uniform_int_distribution<> gid_d(start, (start + cells_per - 1));
    boost::random::uniform_int_distribution<> cellgroup_d(0, (ngroups-1));


    event_pool_.resize(ngroups);
    event_time = 0;
    //create events up until simulation end
    while(event_time < simtime){
        double diff = time_d(rng);
        assert(diff > 0.0);
        event_time += diff;
        if(event_time >= simtime){
            break;
        }
        else{
            src_gid = gid_d(rng);

            //cellgroups are determined by:
            //group # = gid % number of groups
            dest = src_gid % ngroups;

            new_event.first = src_gid;
            new_event.second = static_cast<int>(event_time);
            event_pool_[dest].push(new_event);
        }
    }
}

bool event_generator::compare_top_lte(int id, double comparator) const{
    if(this->empty(id))
        return false;
    else
        return (event_pool_[id].front().second <= comparator);
}

gen_event event_generator::pop(int id){
    gen_event ev = event_pool_[id].front();
    event_pool_[id].pop();
    return ev;
}

} //end of namespace
