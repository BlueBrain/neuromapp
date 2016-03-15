#include <boost/range/algorithm_ext/iota.hpp>
#include <iostream>
#include <time.h>
#include <ctime>
#include <numeric>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random.hpp>

#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"

namespace environment {

void presyn_maker::operator()(int nprocs, int ngroups, int rank){
    //assign input and output gid's
    std::vector<int> available_inputs;
    std::vector<int> cellgroups;
    assert(nprocs >= 1);
    assert(n_out_ >= ngroups);

    if(nprocs == 1){
        for(int i = 0; i < n_out_; ++i){
            outputs_.push_back(i);
        }
        assert(inputs_.empty());
    }
    else{
        //create a list of outputs (rank, rank + n_out)
        for(int i = 0; i < (nprocs * n_out_); ++i){
            if(i >= (rank * n_out_) && i < ((rank * n_out_) + n_out_)){
                outputs_.push_back(i);
            }
            else{
                available_inputs.push_back(i);
            }
        }
        assert(available_inputs.size() >= n_in_);

        if(n_in_ > 0 && nets_per_ > 0){
            //used for random presyn and netcon selection
            boost::mt19937 generator(time(NULL) + rank);
            boost::uniform_int<> uni_dist;
            boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
                randomNumber(generator, uni_dist);
            boost::random_shuffle(available_inputs, randomNumber);

            //create a random map of inputs presyns
            available_inputs.resize(n_in_);
            cellgroups.resize(ngroups);
            boost::iota(cellgroups, 0);

            //for each input presyn,
            //select N unique net connections to cell groups
            boost::random_shuffle(cellgroups, randomNumber);
            for(int i = 0; i < n_in_; ++i){
                int presyn = available_inputs[i];
                for(int j = 0; j < nets_per_; ++j){
                    inputs_[presyn].push_back(cellgroups[j]);
                }
            }
        }
    }
}

bool presyn_maker::find_input(int id, input_presyn& presyn) const{
    std::map<int, std::vector<int> >::const_iterator it = inputs_.begin();
    it = inputs_.find(id);
    if(it == inputs_.end())
        return false;
    else{
        presyn = *it;
        return true;
    }
}

int presyn_maker::operator[](int id) const {
    assert(!outputs_.empty());
    return outputs_[id];
}

} //end of namespace
