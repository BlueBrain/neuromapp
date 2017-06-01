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
#include "compression/util.h"
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
using std_block = block<double,cstandard>;

//todo include more boost program_options later on
int comp_execute(int argc,char *const argv[])
{
    //now the program options section
    try {
        //make desc
        po::options_description desc{"Allowed options"};
        void block_from_file(po::variables_map & vm);
        //add options
        desc.add_options()
            //register gen_block with the create user option
            ("help","Print out the help screen")
            ("demo","this prints out a basic hello world zlib version")
            ("file",po::value<std::string>(),"The create option");
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
        if(vm.count("file")){
            block_from_file(vm);
        }
    }
    catch (po::error &e) {
        std::cerr << e.what()  << std::endl;
    }
    // not quite sure what the return should be in this scenario
    return 0;
}

void block_from_file(po::variables_map & vm)
{
    string fname = vm["file"].as<string>();
    //check file
    neuromapp::check_file(fname);
    std::ifstream i_file(fname);// use the filename given as argument to create block
    std_block do_read_block;
    i_file >> do_read_block;
    do_read_block.print(std::cout);
    
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


