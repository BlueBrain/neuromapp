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

event_generator::event_generator(int ngroups){
    event_pool_.resize(ngroups);
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
