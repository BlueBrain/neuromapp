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
    std_block * gen_block(std::vector<int> vec);
    //built around assumption thhat the columns are the first number people are putting in?
    std::string fname = "../compression/data/csv/values_10_a8213.csv";// hardcoded for now
    //dims as in dimensions for the block, cols and rows, or just cols
    std::vector<int> dims;
    dims = vm["create"].as<std::vector <int>>();//the dimension doesn't include the header line
    std_block * values_block = gen_block(dims);
    block<std::string,cstandard> *header_block=new block<std::string,cstandard>(5);
    header_block->enter_data(fname);
    header_block->print();
    //try entering data from the memvolts file of randomly generated membrane values, use the *created_block to prevent copy argument passing
    values_block->enter_data(fname);
    // and then try  to print out the block with the print template funciton
    values_block->print();
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


