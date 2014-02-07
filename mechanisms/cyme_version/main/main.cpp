#include <iostream>

#include <boost/chrono.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "cyme.hpp"
#include "utils/constant.h"
#include "utils/pack.hpp"
#include "utils/stack.h"

#include "mechanism/channel/Na.hpp"
#include "mechanism/synapse/ProbAMPANMDA_EMC.hpp"

using namespace mechanism::channel;
using namespace mechanism::synapse;
using namespace corebluron;
using namespace memory;

template<class Ba>
void init(Ba& block_a){
    for(int i=0; i<block_a.size(); ++i)
        for(int j=0; j<block_a.size_block(); ++j){
            typename Ba::value_type random = 10*drand48();
            block_a(i,j) = random;
        }
}

int main(int argc, char* argv[]){

    stack s;
/*
    pack<Na,double,AoSoA,1024> a(0);
    pack<ProbAMPANMDA,double,AoSoA,1024> b(0);
    pack<Na,double,AoSoA,128> c(0);
*/
    pack<Na,double,AoSoA> a(1000000,0);
//    pack<ProbAMPANMDA,double,AoSoA> b(0,1024);
//    pack<Na,double,AoSoA> c(0,128);

    init(a);
//    init(b);
//    init(c);
/*
    s.push_back(boost::bind(&pack<Na, double, AoSoA, 1024>::execution,&a)); // fill up the stack
    s.push_back(boost::bind(&pack<ProbAMPANMDA, double, AoSoA, 1024>::execution,&b)); // fill up the stack
    s.push_back(boost::bind(&pack<Na, double, AoSoA,128>::execution,&c)); // fill up the stack
*/
    s.push_back(boost::bind(&pack<Na, double, AoSoA>::execution,&a)); // fill up the stack
//    s.push_back(boost::bind(&pack<ProbAMPANMDA, double, AoSoA>::execution,&b)); // fill up the stack
//    s.push_back(boost::bind(&pack<Na, double, AoSoA>::execution,&c)); // fill up the stack

    boost::chrono::system_clock::time_point start =  boost::chrono::system_clock::now();
    s.flush(); // execute the stack
    boost::chrono::duration<double>  sec = boost::chrono::system_clock::now() - start;
    std::cout <<  sec.count() << " [s] "  << std::endl;
}
