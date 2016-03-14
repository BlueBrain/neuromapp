#include <boost/range/algorithm_ext/iota.hpp>
#include <iostream>
#include <time.h>
#include <ctime>
#include <numeric>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random.hpp>

#include "coreneuron_1.0/event_passing/environment/generator.h"
#include "coreneuron_1.0/event_passing/environment/environment.h"
#include "coreneuron_1.0/event_passing/environment/presyns_maker.h"

namespace environment {

event_generator::event_generator(int nSpikes, int nLocal, int nIte){
    double  sum_ = Spike + nIte + nLocal;
    //mean here the amount of time between spikes for entire sim
    cumulative_percents_[SPIKE] = nSpikes/sum;
    cumulative_percents_[ITE] = (nSpike + nItes)/sum;
}

void event_generator::operator(
const sim_constraints& constraints, const presyn_maker& presyns){
    int dest = 0;
    double event_time = 0;
    int int_tt = 0;
    double percent = 0;
    event_type type;

    double mean = constraints.get_simtime() / sum_;
    double lambda = 1.0 / (mean * constraints.get_ngroups());


    //create random number generator/distributions
    /*
     * rng       => random number generator
     * time_d    => exponential distribution of event times
     * gid_d     => uniform distribution to create indices for output gids
     * percent_d => uniform distribution to decide event type based on percent
     */
    boost::mt19937 rng(constraints.get_rank() + time(NULL));
    boost::random::exponential_distribution<double> time_d(lambda_);
    boost::random::uniform_int_distribution<> gid_d(0, (num_out_ - 1));
    boost::random::uniform_int_distribution<> cellgroup_d(0, (cell_groups_-1));
    boost::random::uniform_real_distribution<> percent_d(0.0,1.0);


    for(size_t i = 0; i < constraints.get_ngroups(); ++i){
        event_time = 0;
        //create events up until simulation end
        while(event_time < constraints.get_simtime()){
            double diff = time_g(rng);
            event_time += diff;
            if(event_time <= constraints.get_simtime()){
                percent = percent_g(rng);

                //SPIKE EVENT
                if(percent < cumulative_percents_[SPIKE]){
                    type = SPIKE;
                    dest = presyns.output_[gid_d(rng)];
                }
                //INTER THREAD EVENT
                else if(percent < cumulative_percents_[ITE]){
                    type = ITE;
                    dest = cellgroup_d(rng);
                    while(dest == i) //dest cannot equal i
                        dest = cellgroup_d(rng);
                }
                //LOCAL EVENT
                else{
                    type = LOCAL;
                    dest = i;//myID
                }
                int_tt = static_cast<int>(event_time);
                event_pool_[i].push(dest, int_tt, type);
            }
        }
    }
}

bool compare_lte(int id, double comparator){
    assert(!event_pool_[id].empty());
    return (comparator <= event_pool_.front().first.t_);
}

} //end of namespace
