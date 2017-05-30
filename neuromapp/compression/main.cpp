/*
 * File: main.cpp aka execute compression app
 * Author: Devin Bayly
 * Purpose:  this program establishes the command line argument parsing loop
 * used for the neuromapp when compression is typed out as option. 
 */

#include <sys/stat.h>
#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
// the includes are relative to the directory above
#include "compression/compression.h"
#include "compression/zlib_fns.h"
#include "compression/allocator.h"
#include "compression/exception.h"
#include "compression/block.h"

/* make namespace alias for program options */
using neuromapp::block;
using neuromapp::cstandard;
namespace po = boost::program_options;
// todo change to variable type, and allocation method
using std_block = block<float,cstandard>;

//todo include more boost program_options later on
int comp_execute(int argc,char *const argv[])
{
    //now the program options section
    try {
        //make desc
        po::options_description desc{"Allowed options"};
        void create_block(po::variables_map & vm);
        //add options
        desc.add_options()
            //register gen_block with the create user option
            ("help","Print out the help screen")
            ("demo","this prints out a basic hello world zlib version")
            ("create",po::value< std::vector<int>>()->multitoken(),"The create option");
        //create variable map
        po::variables_map vm;
        po::store(po::parse_command_line(argc,argv,desc),vm);
        //finish function registration with notify
        po::notify(vm);
        //loop over reasonable options
        if (vm.count("demo")) {
            zlib::hello_zlib();
        }
        if (vm.count("help")) {
            std::cout << desc << std::endl;
        }
        if(vm.count("create")){
            create_block(vm);
        }
    }
    catch (po::error &e) {
        std::cerr << e.what()  << std::endl;
    }
    // not quite sure what the return should be in this scenario
    return 0;
}

void create_block(po::variables_map & vm)
{
    std::ifstream i_file("../compression/data/csv/values_10_a8214_val.csv");
    //TODO make another option for taking a filename from the user, should be easier than this
    std_block do_read_block;
    i_file >> do_read_block;
    std::cout <<&do_read_block(0,0)  << std::endl;
    
}
//todo decide if pointer is right return value 
std_block * gen_block(std::vector<int> vec)
{
    if (vec.size() == 2) {
        std_block * new_block = new std_block(vec[1],vec[0]);// hopefully constructor dishes out enough heap space for this
        return new_block;
    } else if (vec.size() == 1) {
        // only 1d block
        std_block* new_block = new std_block(vec[0]);
        return new_block;
    }
}


