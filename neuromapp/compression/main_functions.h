/*
 * Neuromapp - main_functions.h, Copyright (c), 2015,
 * Devin Bayly - University of Arizona
 * baylyd@email.arizona.edu,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */
#ifndef MAIN_FUNCTIONS_H
#define MAIN_FUNCTIONS_H
#include "compression/block_sort.h"
#include <boost/program_options.hpp>
#include "compression/block.h"
#include "compression/bit_shifting.h"
#include "compression/timer_tool.h"
#include "compression/kernel_measurements.h"
#include "compression/stream_benchmark.h"

namespace po = boost::program_options;
using neuromapp::Timer;
using neuromapp::block;
using neuromapp::stream_bench;
typedef size_t size_type;

template<typename allocator_type>
            /**
            * k_m_routine 
            *
            *
            * @brief
            *
            * @param string & fname
            *
            * @return void
            */
void k_m_routine(string & fname) {
    neuromapp::run_km<allocator_type>(fname);
}

template <typename allocator_type>
            /**
            * sort_routine 
            *
            *
            * @brief
            *
            * @param  block<double,allocator_type> & block,ostream & os, Timer & time_it
            *
            * @return void
            */
void sort_routine ( block<double,allocator_type> & block,ostream & os, Timer & time_it) {
    time_it.start();

    size_type sorting_row = 0;
    neuromapp::col_sort(&block,sorting_row);
    time_it.end();
    os << "sorting took " << time_it.duration () << " ms " << std::endl;
}

template <typename value_type,typename allocator_type>
            /**
            * stream_bench_routine 
            *
            *
            * @brief
            *
            * @param 
            *
            * @return void
            */
void stream_bench_routine() {
    //create two different stream_bench objects, one compress one not
    stream_bench<value_type,allocator_type> non_str_bench(false);
    non_str_bench.run_stream_benchmark();
    non_str_bench.output_results();
    stream_bench<value_type,allocator_type> comp_str_bench(true);
    comp_str_bench.run_stream_benchmark();
    comp_str_bench.output_results();
}

template <typename value_type,typename allocator_type>
            /**
            * compress_routine 
            *
            *
            * @brief
            *
            * @param  block<value_type,allocator_type> & block,ostream & os, Timer & time_it
            *
            * @return void
            */
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
            /**
            * split_routine 
            *
            *
            * @brief
            *
            * @param  block<double,allocator_type> & unsplit_block,ostream & os, Timer & time_it
            *
            * @return void
            */
void split_routine ( block<double,allocator_type> & unsplit_block,ostream & os, Timer & time_it) {
        time_it.start();
        block<unsigned int, allocator_type> split_block = neuromapp::generate_split_block(unsplit_block);
        time_it.end();
        os << "splitting took " << time_it.duration() << " ms" << std::endl;
        compress_routine<unsigned int,allocator_type>(split_block,os,time_it);
}

template <typename allocator_type>
            /**
            * file_routine 
            *
            *
            * @brief
            *
            * @param  block<double,allocator_type> & b1,po::variables_map & vm,Timer & time_it
            *
            * @return void
            */
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



#endif
