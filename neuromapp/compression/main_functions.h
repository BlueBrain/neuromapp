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
#include <boost/program_options.hpp>
#include "compression/block_sort.h"
#include "compression/block.h"
#include "compression/bit_shifting.h"
#include "compression/timer_tool.h"
#include "compression/kernel_measurements.h"
#include "compression/stream_benchmark.h"
#include "compression/conv_info.h"


namespace po = boost::program_options;
using neuromapp::Timer;
using neuromapp::block;
using neuromapp::scale_benchmark;
using neuromapp::add_benchmark;
using neuromapp::triad_benchmark;
using neuromapp::copy_benchmark;
using neuromapp::binary_stream_vectors;
using neuromapp::stream_vectors;
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
void k_m_routine(string & fname,po::variables_map vm) {
    neuromapp::run_km<allocator_type>(fname,vm);
}

template <typename allocator_type>
/**
 * sort_routine 
 *
 *
 * @brief
 *
 * @param  block<double,allocator_type> & block, Timer & time_it
 *
 * @return void
 */
void sort_routine ( block<double,allocator_type> & block, Timer & time_it) {
    time_it.start();

    size_type sorting_row = 0;
    neuromapp::col_sort(&block,sorting_row);
    time_it.end();
    std::cout << "sorting took " << time_it.duration () << " ms " << std::endl;
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
void stream_bench_routine(po::variables_map vm) {
    //create two different stream_bench objects, one compress one not
    //must run the stream bench a separate time without the split argument provided for comparison
    if (vm.count("split")) {
        binary_stream_vectors<value_type,allocator_type> vectors;
        if (vm.count("compression")) vectors.set_compress(true);
        else vectors.set_compress(false);
        copy_benchmark<binary_stream_vectors,value_type,allocator_type> (vectors) ;
        scale_benchmark<binary_stream_vectors,value_type,allocator_type> (vectors) ;
        add_benchmark<binary_stream_vectors,value_type,allocator_type> (vectors) ;
        triad_benchmark<binary_stream_vectors,value_type,allocator_type> (vectors) ;
    } else {
        stream_vectors<value_type,allocator_type>vectors;
        if (vm.count("compression")) vectors.set_compress(true) ;
        else vectors.set_compress(false); 
        copy_benchmark<stream_vectors,value_type,allocator_type> (vectors) ;
        scale_benchmark<stream_vectors,value_type,allocator_type> (vectors) ;
        add_benchmark<stream_vectors,value_type,allocator_type> (vectors) ;
        triad_benchmark<stream_vectors,value_type,allocator_type> (vectors) ;
    }
    //need to figure out how to deduce the type
}

template <typename value_type,typename allocator_type>
/**
 * compress_routine 
 *
 *
 * @brief
 *
 * @param  block<value_type,allocator_type> & block, Timer & time_it
 *
 * @return void
 */
void compress_routine ( block<value_type,allocator_type> & block, Timer & time_it) {
    time_it.start();
    block.compress();
    time_it.end();
    //compress into miliseconds
    std::cout << " compressed memory size: " <<  block.get_current_size() 
        << " starting memory size: " <<  block.memory_allocated()
        << " compression speed: " <<time_it.duration() << std::endl;
    time_it.start();
    block.uncompress();
    time_it.end();
    std::cout << " uncompressed memory size: " <<  block.get_current_size() 
        << " starting memory size: " <<  block.memory_allocated()
        << " uncompression speed: " << time_it.duration() << std::endl;
}

/* so the routines can have a block that is either cstandard or align */
template <typename allocator_type>
/**
 * split_routine 
 *
 *
 * @brief
 *
 * @param  block<double,allocator_type> & unsplit_block, Timer & time_it
 *
 * @return void
 */
 block<typename Conv_info<double>::bytetype, allocator_type> split_routine ( block<double,allocator_type> & unsplit_block, Timer & time_it) {
    time_it.start();
    block<typename Conv_info<double>::bytetype, allocator_type> split_block = neuromapp::generate_split_block(unsplit_block);
    //sanity check for split version
    time_it.end();
    std::cout << "splitting took " << time_it.duration() << " ms" << std::endl;
    compress_routine<Conv_info<double>::bytetype,allocator_type>(split_block,time_it);
    time_it.start();
    block<double,allocator_type> return_trip = neuromapp::generate_unsplit_block<double,allocator_type>(split_block);
    time_it.end();
    std::cout << "unsplitting took " << time_it.duration() << " ms" << std::endl;
    return split_block;
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
    if(vm.count("split")) {
        split_routine(b1,time_it);
        return;// the split block doesn't get returned so don't proceed even if the options are given
    } 
    if ( vm.count("sort") ) {
        sort_routine(b1,time_it);
    }
    if ( vm.count("compress") ) {
        compress_routine(b1,time_it);
    }
}

template <typename allocator_type>
/**
 * bench_routine 
 *
 *
 * @brief
 *
 * @param  block<double,allocator_type> & b1,Timer & time_it
 *
 * @return void
 */
void bench_routine( block<double,allocator_type> & b1,Timer & time_it,po::variables_map vm) {
    sort_routine(b1,time_it);
    compress_routine(b1,time_it);
    split_routine(b1,time_it);
}

#endif
