#include <boost/range/algorithm_ext/iota.hpp>
#include <iostream>
#include <time.h>
#include <ctime>
#include <numeric>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/random/mersenne_twister.hpp>

#include "coreneuron_1.0/environment/presyn_maker.h"
#include "coreneuron_1.0/environment/environment.h"

namespace environment {

void presyn_maker::operator()(const sim_env& env){
    //assign input and output gid's
    std::vector<int> available_inputs;
    std::vector<int> cellgroups;
    int nprocs = env.num_procs_;
    int ngroups = env.num_group_;
    int rank = env.rank_;

    assert(ngroups > 2);
    if(nprocs > 1){
        for(int i = 0; i < (nprocs * n_out_); ++i){
            if(i >= (rank * n_out_) && i < ((rank * n_out_) + n_out_)){
                outputs_.push_back(i);
            }
            else{
                available_inputs.push_back(i);
            }
        }
        //create a randomly ordered list of inputs_
        assert(available_inputs.size() >= n_in_);

        //random presyn and netcon selection
        boost::mt19937 generator(time(NULL) + rank);
        boost::uniform_int<> uni_dist;
        boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
            randomNumber(generator, uni_dist);
        boost::random_shuffle(available_inputs, randomNumber);
        available_inputs.resize(n_in_);
        //create a vector of randomly ordered cellgroups
        cellgroups.resize(ngroups);
        boost::iota(cellgroups, 0);

        //for each input presyn,
        //select N unique netcons to cell groups
        boost::random_shuffle(cellgroups, randomNumber);
        for(int i = 0; i < n_in_; ++i){
            int presyn = available_inputs[i];
            for(int j = 0; j < nets_per_; ++j){
                inputs_[presyn].push_back(cellgroups[j]);
            }
        }
    }
    else{
        for(int i = 0; i < num_out_; ++i){
            outputs_.push_back(i);
        }
        assert(inputs_.empty());
    }
}

bool find const(int id, input_presyn& presyn){
    std::map<int, std::vector<int> >::iterator it = inputs_.begin();
    it = inputs_.find(id);
    if(it == inputs_.end())
        return false;
    else{
        presyn = *it;
        return true;
    }
}

} //end of namespace
