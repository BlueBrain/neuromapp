#include <iostream>

#include <boost/chrono.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "cyme.hpp"
#include "utils/constant.h"
#include "utils/pack.hpp"
#include "utils/stack.h"

#include "mechanism/channel/Na.hpp"
using namespace mechanism::channel;
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
    pack<Na,double,AoSoA> a(16384,0); // pack 16384 synapse, AoSoA
    init(a);

    s.push_back(boost::bind(&pack<Na, double ,AoSoA>::execution,&a)); // fill up the stack

    boost::chrono::system_clock::time_point start =  boost::chrono::system_clock::now();
    s.flush(); // execute the stack
    boost::chrono::duration<double>  sec = boost::chrono::system_clock::now() - start;
    std::cout <<  sec.count() << " [s] "  << std::endl;
}
