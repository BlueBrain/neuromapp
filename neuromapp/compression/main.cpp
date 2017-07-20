/* Filename : main.cpp
 * Authors : Devin Bayly, Tim Ewart
 * Organization : University of Arizona, EPFL
 * Purpose : xxx
 * Date : 2017-07-20 
 */
/*
 * File: main.cpp aka execute compression app
 * Author: Devin Bayly
 * Purpose:  this program establishes the command line argument parsing loop
 * used for the neuromapp when compression is typed out as option. 
 */

#include <sys/stat.h>
#include <chrono>
#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
// the includes are relative to the directory above
#include "compression/util.h"
#include "compression/compression.h"
#include "compression/compressor.h"
#include "compression/zlib_fns.h"
#include "compression/allocator.h"
#include "compression/exception.h"
#include "compression/block_sort.h"
#include "compression/block.h"
#include "compression/bit_shifting.h"
#include "compression/timer_tool.h"
#include <boost/mpl/list.hpp>

/* make namespace alias for program options */
using neuromapp::block;
namespace po = boost::program_options;
typedef size_t size_type;
neuromapp::Timer timer;

//todo include more boost program_options later on
int comp_execute(int argc,char *const argv[])
{
    //prototype line above
    //now the program options section
    try {
        //make desc
        po::options_description desc{"Allowed options"};
        //add options
        desc.add_options()
            ("help","Print out the help screen")
            ("file",po::value<std::string>(),"The create option")
            ("compress","perform a compress&uncompress on file specific block (prints out results)")
            ("sort","perform a sort on the block before any other operations")
            ("split","create new block of floating points split into their sign exponent mantissa repsentations")
            ("benchmark","run all of the files in data directory, create csv with output stats");
        //create variable map
        po::variables_map vm;
        po::store(po::parse_command_line(argc,argv,desc),vm);
        //finish function registration with notify
        po::notify(vm);
        /*consider top level options
         *  within the file vs benchmark the other options will be checked */
        if (vm.count("help")) {
            std::cout << desc << std::endl;
        }
        if(vm.count("file") && ! vm.count("benchmark")){
            string fname = vm["file"].as<std::string>();
            ifstream ifile(fname);
            if (vm.count("align")) {
                block<double,align> b1;
                ifile >> b1;
                file_routine(b1,vm);
            } else {
                block<double,cstandard> b1;
                ifile >> b1;
                file_routine(b1,vm);
            }

        }
        if(vm.count("benchmark") && ! vm.count("file")) {
            /* gett more info about how to access files, and iterate over the directory of entries */
            for (string fname : fname_vect) {
                block<double,cstandard> std_block;
                block<double,align> align_block;
                /* now pass both versions through the file routine separately */
                file_routine(std_block,vm);
                file_routine(align_block,vm);
            }

        }
    } catch (po::error &e) {
        std::cerr << e.what()  << std::endl;
    }
    // not quite sure what the return should be in this scenario
    return 0;
}

template <typename allocator_type>
void file_routine( block<double,allocator_type> & b1,po::variables_map & vm) {
    ofstream out("compress_app_run.log");
    if(vm.count("split")) {
        split_routine(b1,out,timer);
        return;// the split block doesn't get returned so don't proceed even if the options are given
    } 
    if ( vm.count("sort") ) {
        sort_routine(b1,out,timer);
    }
    if ( vm.count("compress") ) {
        compress_routine(b1,out,timer);
    }
}

/* so the routines can have a block that is either cstandard or align */
template <typename allocator_type>
void split_routine ( block<double,allocator_type> & block,ostream & os, Timer & time_it) {
        time_it.start();
        block<unsigned int, allocator_type> split_block = neuromapp::generate_split_block(block);
        time_it.end();
        os << "splitting took " << time_it.duration() << " ms" << std::endl;
        compress_routine(split_block,os,time_it);
}
template <typename allocator_type>
void sort_routine ( block<double,allocator_type> & block,ostream & os, Timer & time_it) {
    time_it.start();
    neuromapp::col_sort(block);
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


