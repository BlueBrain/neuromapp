#ifndef MINIAPPS_HEADER_HPP
#define MINIAPPS_HEADER_HPP

#define BOOST_TEST_MODULE math
#define BOOST_TEST_MAIN
//#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <utility>

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

    template<class Ba, class Bb, class cmech> // m and n are differents into the block that why I passe like argument
    void init(Ba& block_a, Bb& block_b, cmech & v){
        for(std::size_t j=0; j< v.newsize(); ++j)
            block_a(0,v.getnew(j)) = block_b[v.getold(j)];
    }

    template<class T,  memory::order O, int S >
    struct type_info{
        typedef T value_type;
        static const memory::order order = O;
        static const int s = S; /* select container, 0 = vector, 1 = array */
    };

    typedef boost::mpl::list<   /* array */
                                type_info<double,memory::AoS,0>,
                                type_info<double,memory::AoSoA,0>,
                                type_info<float,memory::AoS,0>,
                                type_info<float,memory::AoSoA,0>,
                                /* vector */
                                type_info<double,memory::AoS,1>,
                                type_info<double,memory::AoSoA,1>,
                                type_info<float,memory::AoS,1>,
                                type_info<float,memory::AoSoA,1>
                            > test_types;

    template<class T>
    T error_close();

    template<>
    float error_close(){
        return 1;
    }

    template<>
    double error_close(){
        return 0.0001;
    }

    }
}

#endif // MINIAPPS_HEADER_HPP
