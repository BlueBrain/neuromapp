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
    std::vector<int> cells;
    assert(nprocs >= 1);

    //populate list of gid's
    cells.resize(n_cells_);
    boost::iota(cells, 0);

    //used for random presyn and netcon selection
    boost::mt19937 generator(time(NULL) + rank);
    boost::uniform_int<> uni_dist;
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
        randomNumber(generator, uni_dist);

    int cells_per = n_cells_ / nprocs;
    int first_cell = cells_per * rank;
    int cur;

    //create local presyns with empty vectors
    for(int i = 0; i < cells_per; ++i){
        outputs_[first_cell + i].reserve(fan_in_);
    }
    //foreach gid, select srcs
    for(int i = 0; i < cells_per; ++i){
        boost::random_shuffle(cells, randomNumber);
        for(int j = 0; j < fan_in_; ++j){
            cur = cells[j];
            //local GID
            if(cur >= first_cell && cur < (first_cell + cells_per)){
                //add self to src gid
                outputs_[cur].push_back(first_cell + i);
            }
            //remote GID
            else{
                //add self to input presyn for gid
                inputs_[cur].push_back(first_cell + i);
            }
        }
    }
    if(rank == 0){
    for(std::map<int,presyn>::iterator it = inputs_.begin(); it != inputs_.end(); ++it){
        std::cout<<"Input: "<<it->first<<" has "<<it->second.size()<<" connections";
        for(int i = 0; i < it->second.size(); ++i){
            std::cout<<" "<<it->second[i];
        }
        std::cout<<std::endl;
    }
    for(std::map<int,presyn>::iterator it = outputs_.begin(); it != outputs_.end(); ++it){
        std::cout<<"Output: "<<it->first<<" has "<<it->second.size()<<"connections";
        for(int i = 0; i < it->second.size(); ++i){
            std::cout<<" "<<it->second[i];
        }
        std::cout<<std::endl;
    }
    }
}

const presyn* presyn_maker::find_input(int key) const{
    std::map<int, std::vector<int> >::const_iterator it = inputs_.begin();
    const presyn* input_ptr = NULL;
    it = inputs_.find(key);
    if(it != inputs_.end()){
        input_ptr = &(it->second);
    }
    return input_ptr;
}

const presyn* presyn_maker::find_output(int key) const{
    std::map<int, std::vector<int> >::const_iterator it = outputs_.begin();
    const presyn* output_ptr = NULL;
    it = outputs_.find(key);
    if(it != outputs_.end()){
        output_ptr = &(it->second);
    }
    return output_ptr;
}

} //end of namespace
