//
//  benchmark.h
//  neuromapp
//
//  Created by Tim Ewart on 30/01/2016.
//
//

#ifndef benchmark_h
#define benchmark_h


#include <mpi.h>
#include <vector>
#include <numeric>
#include <functional>

#include "key-value/meta.h"
#include "key-value/memory.h"
#include "key-value/memory.h"
#include "key-value/meta.h"
#include "key-value/mpikey-value.h"
#include "key-value/utils/tools.h"
#include "key-value/utils/arguments.h"


class parallel{
public:
    explicit parallel(){
        rank = keyvalue::utils::master.rank();
        size = keyvalue::utils::master.size();
        threads = 1;
    }
    
    inline int get_rank() const{
        return rank;
    }
    
    inline int get_size() const{
        return size;
    }
    
    inline int get_threads() const{
        return threads;
    }
private:
    int rank,size,threads;
};

template<class M>
class benchmark{
public:
    typedef M meta_type;
    /** \fun benchmark(std::size_t choice, std::size_t qmb = 4096)
        \brief compute the total number of compartment (2.5 = 2.5 MB per neuron, 350 compartiment per neuron)
        4096 MB, 25% of the memory of a compute node of the BG/Q
     */
    benchmark(argument const& args = argument()):a(args){
        s = args.voltage_size();
        int cg_size = s / args.cg();
        int first_size = cg_size + (s % args.cg());

        g = keyvalue::group<meta_type>(cg_size);
        g.push_back(keyvalue::nrnthread(first_size));

        for (int i = 1; i < args.cg(); i++)
            g.push_back(keyvalue::nrnthread(cg_size));
    }
    
    /** \fun size() const 
        \brief return the total size, carefull memory is dynamic an update
         of the size may be usefull 
    */
    inline std::size_t size() const {
        return s;
    }

    /** \fun update_size() const 
        \brief if we complete the container the size changes, must be updated */
    inline std::size_t update_size() const {
        return g.total_size();
    }
    
    /** \fun get_group() const
        \brief get the groupe i.e. the memory */
    keyvalue::group<meta_type> const& get_group() const{
        return g;
    }
    
    /** \fun get_args() const
        \brief return the argument */
    argument const & get_args() const {
        return a;
    }
    
private:
    /** memory for the bench */
    keyvalue::group<meta_type> g;
    /** mpi + thread info */
    parallel p;
    /** reference on the arguments structure */
    argument const & a;
    /** correspond to the total number of compartement */
    std::size_t s;
};

template<class M>
void run_loop(benchmark<M> const& b){
    // extract the group of memory
    keyvalue::group<M> const& g = b.get_group();
    argument const& a = b.get_args();
    
    // build the needed function in function of the backend
    KeyValueMap_meta kvmap;

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
                kvmap.insert(g.meta_at(cg));


            #pragma omp parallel for
            for (int cg = 0; cg < a.cg(); cg++)
                kvmap.wait(g.meta_at(cg));

            t.toc();
            vtime.push_back(t.time());
        }
    }
    
    statistic(b,vtime);
}

    struct inverse{
        void operator()(double& a) {
            a = 1/a;
        };
    };

    template<class M>
    void statistic(benchmark<M> const& b, std::vector<double>& vtime){
        // extract the arguements
        argument const& a = b.get_args();

        int reqs = a.cg();
        int bytes = a.voltage_size() * sizeof(double);
    
        std::vector<double>::iterator it = vtime.begin();

        std::cout << "  Master only, else change the mask \n";
    
        while(it != vtime.end()){
            std::cout << "  Time: " << *it << "[s]" << std::endl
                      << "  I/O: "  << reqs/(*it) << " IOPS" << std::endl
                      << "  BW: "   << bytes/((*it)*1024.*1024.) << " MB/s" << std::endl;
        ++it;
        }

        double g_iops = 0, g_mbw = 0;
        std::for_each(vtime.begin(), vtime.end(),inverse());
        g_iops = g_mbw = keyvalue::utils::accumulate(vtime.begin(), vtime.end(), 0.); // MPI is inside
        g_iops *= reqs;
        g_mbw *= bytes/(1024.*1024.);

        std::cout << " g_iops " << g_iops << " IOPS \n "
                  << " g_mbw "  << g_mbw << " MB/s \n " ;
    }


#endif /* benchmark_h */
