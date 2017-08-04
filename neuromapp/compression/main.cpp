/*
 * Neuromapp - main.cpp, Copyright (c), 2015,
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
#include "compression/compressor.h"
#include "compression/allocator.h"
#include "neuromapp/compression/common/data/path.h"
#include "compression/exception.h"
#include "compression/block_sort.h"
#include "compression/block.h"
#include "compression/bit_shifting.h"
#include "compression/timer_tool.h"
#include "compression/compression.h"
#include "compression/main_functions.h"
#include <boost/mpl/list.hpp>

/* make namespace alias for program options */
using namespace std;
using neuromapp::block;
using neuromapp::Timer;
namespace po = boost::program_options;
typedef size_t size_type;

//todo include more boost program_options later on
int comp_execute(int argc,char *const argv[])
{
    //now the program options section
    try {
        // create the timer 
        Timer timer;
        //make desc
        po::options_description desc{"Allowed options"};
        //add options
        desc.add_options()
            ("help","Print out the help screen")
            ("file",po::value<std::string>(),"The create option")
            ("compress","perform a compress&uncompress on file specific block (prints out results)")
            ("sort","perform a sort on the block before any other operations")
            ("split","create new block of floating points split into their sign exponent mantissa repsentations")
            ("path","")
            ("align","use non-standard allocator for block process")
            ("kernel_measure","run increasingly complex calculations on block comparing timing performance tradeoff for compression")
            ("stream_benchmark","use a McCalpin STREAM inspired set of benchmarks to measure bandwith on the computer")
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
        if(vm.count("file") && ! vm.count("benchmark") && ! vm.count("kernel_measure")){
            string fname = vm["file"].as<std::string>();
            ifstream ifile(fname);
            if (vm.count("align")) {
                block<double,neuromapp::align> b1;
                ifile >> b1;
                file_routine(b1,vm,timer);
            } else {
                block<double,neuromapp::cstandard> b1;
                ifile >> b1;
                file_routine(b1,vm,timer);
            }
        }
        if (vm.count("path")) {
            std::cout << mapp::path_specifier::give_path() << std::endl;;
        }
        if(vm.count("benchmark")) {
            /* gett more info about how to access files, and iterate over the directory of entries */
            vector<string> fname_vect {"../compression/trans_data/values_10_a8213trans_bulk.csv"};
            for (string fname : fname_vect) {
                block<double,neuromapp::cstandard> std_block;
                block<double,neuromapp::align> align_block;
                /* now pass both versions through the file routine separately */
                file_routine(std_block,vm,timer);
                file_routine(align_block,vm,timer);
            }

        }

        if (vm.count("stream_benchmark")) {
            if (vm.count("align")) {
                stream_bench_routine<double,neuromapp::align>();
            } else {
                stream_bench_routine<double,neuromapp::cstandard>();
            }
        }

        if (vm.count("kernel_measure")) {
            string fname ;
            if ( vm.count("file")) {
                fname = vm["file"].as<std::string>();
            } else {
                fname = "../compression/trans_data/values_10_a8213trans_both.csv";
            }
            if (vm.count("align")) {
                k_m_routine<neuromapp::align>(fname);
            } else {
                k_m_routine<neuromapp::cstandard>(fname);
            }
        }


    } catch (po::error &e) {
        std::cerr << e.what()  << std::endl;
    }
    // not quite sure what the return should be in this scenario
    return 0;
}

