/* Filename : main.cpp
 * Authors : Devin Bayly, Tim Ewart
 * Organization : University of Arizona, EPFL
 * Purpose:  this program establishes the command line argument parsing loop
 * Date : 2017-07-20 
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
#include "compression/compressor.h"
#include "compression/allocator.h"
#include "compression/exception.h"
#include "compression/block_sort.h"
#include "compression/block.h"
#include "compression/bit_shifting.h"
#include "compression/timer_tool.h"
#include "compression/compression.h"
#include "compression/main_functions.h"
#include <boost/mpl/list.hpp>

/* make namespace alias for program options */
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
                block<double,neuromapp::align> b1;
                ifile >> b1;
                file_routine(b1,vm,timer);
            } else {
                block<double,neuromapp::cstandard> b1;
                ifile >> b1;
                file_routine(b1,vm,timer);
            }

        }
        if(vm.count("benchmark") && ! vm.count("file")) {
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
    } catch (po::error &e) {
        std::cerr << e.what()  << std::endl;
    }
    // not quite sure what the return should be in this scenario
    return 0;
}

