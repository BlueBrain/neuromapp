//
//  trait.h
//  queue
//
//  Created by Ewart Timothée on 15/07/16.
//
//

#ifndef trait_h
#define trait_h

#include <boost/heap/binomial_heap.hpp>
#include <boost/heap/d_ary_heap.hpp>
#include <boost/heap/priority_queue.hpp>
#include <boost/heap/fibonacci_heap.hpp>
#include <boost/heap/pairing_heap.hpp>
#include <boost/heap/skew_heap.hpp>

#include "coreneuron_1.0/queue/tool/priority_queue.hpp" // MH work

enum container {sptq_queue, bin_queue, priority_queue,binomial_heap,
                fibonacci_heap,pairing_heap,skew_heap,d_ary_heap};
//serial queue
template<container q>
struct helper_type;

template<>
struct helper_type<priority_queue>{
    typedef std::priority_queue<double,std::vector<double>,std::greater<double> > value_type;
    const static char name[];
};

template<>
struct helper_type<sptq_queue>{
    typedef tool::sptq_queue<double, std::greater<double> > value_type;
    const static char name[];
};

template<>
struct helper_type<bin_queue>{ // no comparator great by default
    typedef tool::bin_queue<double> value_type;
    const static char name[];
};

template<>
struct helper_type<binomial_heap>{
    typedef boost::heap::binomial_heap<double, boost::heap::compare<std::greater<double> > > value_type;
    const static char name[];
};

template<>
struct helper_type<fibonacci_heap>{
    typedef boost::heap::fibonacci_heap<double, boost::heap::compare<std::greater<double> > > value_type;
    const static char name[];
};

template<>
struct helper_type<pairing_heap>{
    typedef boost::heap::pairing_heap<double, boost::heap::compare<std::greater<double> > > value_type;
    const static char name[];
};

template<>
struct helper_type<skew_heap>{
    typedef boost::heap::skew_heap<double, boost::heap::compare<std::greater<double> > > value_type;
    const static char name[];
};

template<>
struct helper_type<d_ary_heap>{
    typedef boost::heap::d_ary_heap<double,boost::heap::arity<16>, boost::heap::compare<std::greater<double> > > value_type;
    const static char name[];
};

//because no c++11
const char helper_type<priority_queue>::name[] = "std::priority_queue";
const char helper_type<sptq_queue>::name[] = "original_sptq_queue";
const char helper_type<bin_queue>::name[] = "original_bin_queue";
const char helper_type<binomial_heap>::name[] = "boost::binomial_heap";
const char helper_type<fibonacci_heap>::name[] = "boost::fibonacci_heap";
const char helper_type<pairing_heap>::name[] = "boost::pairing_heap";
const char helper_type<skew_heap>::name[] = "boost::skew_heap";
const char helper_type<d_ary_heap>::name[] = "boost::d_aray_heap";

#endif /* trait_h */
