#ifndef MINIAPPS_HEADER_HPP
#define MINIAPPS_HEADER_HPP

#define BOOST_TEST_MODULE math
#define BOOST_TEST_MAIN
//#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <stdexcept>
#include <algorithm>  

#include "cyme.hpp"
#include "utils/constant.h"
#include "utils/pack.hpp"



#include <boost/mpl/list.hpp>
#include <boost/cstdint.hpp> 
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/test/floating_point_comparison.hpp>

#define RELATIVE_ERROR 0.001

namespace cyme {
    namespace test {

    static boost::random::uniform_real_distribution<float>    RandomDouble = boost::random::uniform_real_distribution<float>(-5,5); 
    static boost::random::uniform_real_distribution<double>   Randomfloat  = boost::random::uniform_real_distribution<double>(-5,5); 
    static boost::random::mt19937    rng;

    template<class T>
    T GetRandom();

    template<>
    float GetRandom<float>(){
        return Randomfloat(rng);
    }
   
    template<>
    double GetRandom<double>(){
        return RandomDouble(rng);
    }

    template<class Ba, class Bb> // m and n are differents into the block that why I passe like argument
    void init(Ba& block_a, Bb& block_b){
        for(std::size_t j=0; j<block_a.size_block(); ++j){
            block_a(0,j) = block_b[j];
        }
    }

    } 
}

#endif // MINIAPPS_HEADER_HPP
