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
#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"

namespace environment {

event_generator::event_generator(int nSpike, int nIte, int nLocal){
    sum_ = static_cast<double>(nSpike + nIte + nLocal);
    //mean here the amount of time between spikes for entire sim
    cumulative_percents_[SPIKE] = nSpike/sum_;
    cumulative_percents_[ITE] = (nSpike + nIte)/sum_;

}

void event_generator::operator()(
int simtime, int ngroups, int rank, const presyn_maker& presyns){
    int dest = 0;
    double event_time = 0;
    int int_tt = 0;
    double percent = 0;
    event_type type;
    queueing::event ev;

    double mean = simtime / sum_;
    double lambda = 1.0 / (mean * ngroups);

    std::cout<<"MEAN: "<<mean<<std::endl;
    std::cout<<"LAMBDA: "<<lambda<<std::endl;


    //create random number generator/distributions
    /*
     * rng       => random number generator
     * time_d    => exponential distribution of event times
     * gid_d     => uniform distribution to create indices for output gids
     * percent_d => uniform distribution to decide event type based on percent
     */
    boost::mt19937 rng(rank + time(NULL));
    boost::random::exponential_distribution<double> time_d(lambda);
    boost::random::uniform_int_distribution<> gid_d(0, (presyns.get_nout() - 1));
    boost::random::uniform_int_distribution<> cellgroup_d(0, (ngroups-1));
    boost::random::uniform_real_distribution<> percent_d(0.0,1.0);


    event_pool_.resize(ngroups);
    for(size_t i = 0; i < ngroups; ++i){
        event_time = 0;
        //create events up until simulation end
        while(event_time < simtime){
            double diff = time_d(rng);
            event_time += diff;
            if(event_time >= simtime){
                break;
            }
            else{
                percent = percent_d(rng);

                //SPIKE EVENT
                if(percent < cumulative_percents_[SPIKE]){
                    type = SPIKE;
                    dest = presyns[gid_d(rng)];
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
                ev.data_ = dest;
                ev.t_ = static_cast<double>(int_tt);
                gen_event g(ev, type);
                event_pool_[i].push(g);
            }
        }
    }
}

bool event_generator::compare_top_lte(int id, double comparator) const{
    assert(!event_pool_[id].empty());
    return (event_pool_[id].front().first.t_ <= comparator);
}

gen_event event_generator::pop(int id){
    gen_event ev = event_pool_[id].front();
    event_pool_[id].pop();
    return ev;
}

} //end of namespace
