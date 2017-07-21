/* Filename : compression.h
 * Authors : Devin Bayly, Tim Ewart
 * Organization : University of Arizona, EPFL
 * Purpose : This file is just a header used to provide templated functions to the main programing executed within the neuromapp app.
 * Date : 2017-07-20 
 */

#ifndef MAPP_COMPRESSION_EXECUTE_
#define MAPP_COMPRESSION_EXECUTE_ 
#include "block_sort.h"
#include <boost/program_options.hpp>
#include "block.h"
#include "bit_shifting.h"
#include "timer_tool.h"

namespace po = boost::program_options;
using neuromapp::Timer;
using neuromapp::block;
typedef size_t size_type;


template <typename allocator_type>
void sort_routine ( block<double,allocator_type> & block,ostream & os, Timer & time_it) {
    time_it.start();
    //TODO ask whether we should specify for any particular row on the sorting
    size_type sorting_row = 0;
    neuromapp::col_sort(&block,sorting_row);
    time_it.end();
    os << "sorting took " << time_it.duration () << " ms " << std::endl;
}

template <typename value_type,typename allocator_type>
void compress_routine ( block<value_type,allocator_type> & block,ostream & os, Timer & time_it) {
        time_it.start();
        block.compress();
        time_it.end();
        //compress into miliseconds
        os << " compressed memory size: " <<  block.get_current_size() 
            << " starting memory size: " <<  block.memory_allocated()
            << " compression speed: " <<time_it.duration();
        time_it.start();
        block.uncompress();
        time_it.end();
        os << " uncompressed memory size: " <<  block.get_current_size() 
            << " starting memory size: " <<  block.memory_allocated()
            << " uncompression speed: " << time_it.duration();
}

/* so the routines can have a block that is either cstandard or align */
template <typename allocator_type>
void split_routine ( block<double,allocator_type> & unsplit_block,ostream & os, Timer & time_it) {
        time_it.start();
        block<unsigned int, allocator_type> split_block = neuromapp::generate_split_block(unsplit_block);
        time_it.end();
        os << "splitting took " << time_it.duration() << " ms" << std::endl;
        compress_routine<unsigned int,allocator_type>(split_block,os,time_it);
}

template <typename allocator_type>
void file_routine( block<double,allocator_type> & b1,po::variables_map & vm,Timer & time_it) {
    ofstream out("compress_app_run.log");
    if(vm.count("split")) {
        split_routine(b1,out,time_it);
        return;// the split block doesn't get returned so don't proceed even if the options are given
    } 
    if ( vm.count("sort") ) {
        sort_routine(b1,out,time_it);
    }
    if ( vm.count("compress") ) {
        compress_routine(b1,out,time_it);
    }
}


int comp_execute(int argc,char *const argv[]);
#endif /* ifndef MAPP_COMPRESSION_EXECUTE_ */
