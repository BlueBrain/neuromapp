//
//  benchmark.h
//  neuromapp
//
//  Created by Tim Ewart on 30/01/2016.
//
//

#ifndef benchmark_h
#define benchmark_h

#include <vector>
#include <numeric>
#include <functional>

#include "key-value/meta.h"
#include "key-value/memory.h"
#include "key-value/memory.h"
#include "key-value/meta.h"
#include "key-value/mpikey-value.h"
#include "key-value/utils/tools.h"
#include "key-value/utils/argument.h"
#include "key-value/utils/statistic.h"


template<class M>
class benchmark{
public:
    typedef M meta_type;
    /** \fun benchmark(std::size_t choice, std::size_t qmb = 4096)
        \brief compute the total number of compartment (2.5 = 2.5 MB per neuron, 350 compartiment per neuron)
        4096 MB, 25% of the memory of a compute node of the BG/Q
     */
    benchmark(keyvalue::argument const& args = keyvalue::argument()):a(args){
        s = args.voltages_size();
        int cg_size = s / args.cg();
        int first_size = cg_size + (s % args.cg());

        g = keyvalue::group<meta_type>(cg_size);
        g.push_back(keyvalue::nrnthread(first_size));

        for (int i = 1; i < args.cg(); i++)
            g.push_back(keyvalue::nrnthread(cg_size));
    }
    
    /** \fun get_group() const
        \brief get the group i.e. the memory */
    keyvalue::group<meta_type> const& get_group() const{
        return g;
    }
    
    /** \fun get_args() const
        \brief return the argument */
    keyvalue::argument const & get_args() const {
        return a;
    }
    
private:
    /** memory for the bench */
    keyvalue::group<meta_type> g;
    /** reference on the arguments structure */
    keyvalue::argument const & a;
    /** correspond to the total number of compartement */
    std::size_t s;
};

template<class M>
keyvalue::statistic run_loop(benchmark<M> const& b){
    // extract the group of memory
    keyvalue::group<M> const& g = b.get_group();
    keyvalue::argument const& a = b.get_args();
    
    // build the needed function in function of the backend
    keyvalue_map kv;

    // the timer
    keyvalue::utils::timer t;
    // go to dodo
    int comp_time_us = 100 * a.usecase() * 1000;
    
    // keep time trace
    std::vector<double> vtime;
    vtime.reserve(1024);
    
    // these two loops should be merge
    for (float st = 0; st < a.st(); st += a.md()) {
        for (float md = 0; md < a.md(); md += a.dt()) {
            usleep(comp_time_us);

            t.tic();

            #pragma omp parallel for
            for (int cg = 0; cg < a.cg(); cg++)
                kv.insert(g.meta_at(cg));


            #pragma omp parallel for
            for (int cg = 0; cg < a.cg(); cg++)
                kv.wait(g.meta_at(cg));

            t.toc();
            vtime.push_back(t.time());
        }
    }
    
    return keyvalue::statistic(a,vtime);
}


#endif /* benchmark_h */
