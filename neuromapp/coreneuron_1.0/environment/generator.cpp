#include <boost/range/algorithm_ext/iota.hpp>
#include <iostream>
#include <time.h>
#include <ctime>
#include <numeric>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random.hpp>

#include "coreneuron_1.0/environment/generator.h"
#include "coreneuron_1.0/environment/sim_env.h"
#include "coreneuron_1.0/environment/presyns_maker.h"

namespace environment {

event_generator::event_generator(int nSpikes, int nLocal, int nIte){
    double  sum_ = Spike + nIte + nLocal;
    //mean here the amount of time between spikes for entire sim
    cumulative_percents_[SPIKE] = nSpikes/sum;
    cumulative_percents_[ITE] = (nSpike + nItes)/sum;
}

void event_generator::operator(
const sim_env& env, const presyn_maker& presyns){
    int dest = 0;
    int n = 0;
    int spike_c = 0;
    int ite_c = 0;
    int local_c = 0;
    double event_time = 0;
    int int_tt = 0;
    double percent = 0;
    event_type type;

    double mean = env.sim_time_ / sum_;
    double lambda = 1.0 / (mean * env.num_groups_);


    //create random number generator/distributions
    /*
     * rng       => random number generator
     * time_d    => exponential distribution of event times
     * gid_d     => used to create indices for output gids
     * percent_d => used to decide event type based on percent
     */
    boost::mt19937 rng(env.rank_ + time(NULL));
    boost::random::exponential_distribution<double> time_d(lambda_);
    boost::random::uniform_int_distribution<> gid_d(0, (num_out_ - 1));
    boost::random::uniform_int_distribution<> cellgroup_d(0, (cell_groups_ - 1));
    boost::random::uniform_real_distribution<> percent_d(0.0,1.0);


    for(size_t i = 0; i < thread_datas_.size(); ++i){
        event_time = 0;
        //create events up until simulation end
        while(event_time < env.sim_time_){
            double diff = time_g(rng);
            event_time += diff;
            if(event_time <= env.sim_time_){
                percent = percent_g(rng);

                //SPIKE EVENT
                if(percent < cumulative_percents_[SPIKE]){
                    type = SPIKE;
                    ++spike_c;
                    dest = presyns.output_[gid_g(rng)];
                }
                //INTER THREAD EVENT
                else if(percent < cumulative_percents_[ITE]){
                    type = ITE;
                    ++ite_c;
                    dest = cellgroup_g(rng);
                    while(dest == i) //dest cannot equal i
                        dest = cellgroup_g(rng);
                }
                //LOCAL EVENT
                else{
                    type = LOCAL;
                    ++local_c;
                    dest = i;//myID
                }
                int_tt = static_cast<int>(event_time);
                event_pool_[i].push(dest, int_tt, type);
            }
        }
    }
    std::cout<<"spikes: "<<spike_c<<std::endl;
    std::cout<<"locals: "<<local_c<<std::endl;
    std::cout<<"ites: "<<ite_c<<std::endl;
}

bool compare_lte(int id, double comparator){
    assert(!event_pool_[id].empty());
    return (comparator <= event_pool_.front().first.t_);
}

} //end of namespace
