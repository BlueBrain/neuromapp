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

    static boost::random::uniform_real_distribution<float>    RandomDouble = boost::random::uniform_real_distribution<float>(0.01,5); 
    static boost::random::uniform_real_distribution<double>   Randomfloat  = boost::random::uniform_real_distribution<double>(0.01,5); 
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

    template<class T,  memory::order O>
    struct type_info{
        typedef T value_type;
        static const memory::order order = O; 
    };

    typedef boost::mpl::list<
                                type_info<double,memory::AoS>,
                                type_info<double,memory::AoSoA>,
                                type_info<float,memory::AoS>,
                                type_info<float,memory::AoSoA>
                            > test_types;

    template<class T>
    T error_close();

    template<>
    float error_close(){
        return 0.1;
    }
   
    template<>
    double error_close(){
        return 0.0001;
    }

    } 
}

#endif // MINIAPPS_HEADER_HPP
