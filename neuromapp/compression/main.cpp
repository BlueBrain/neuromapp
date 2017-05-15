/*
* File: main.cpp aka execute compression app
* Author: Devin Bayly
* Purpose:  this program establishes the command line argument parsing loop
* used for the neuromapp when compression is typed out as option. 
*/

#include <iostream>
#include <string>
#include <boost/program_options.hpp>
// the includes are relative to the directory above
#include "compression/allocator.h"
#include "compression/exception.h"
#include "compression/block.h"
// do I need  to use the util error?

/* make namespace alias for program options */
namespace po = boost::program_options;

//also need to make it possible to only get one of these
block & gen_block(int n,int m=0)
{
    if (m != 0) {
        block new_block(n,m);// hopefully constructor dishes out enough heap space for this
    } else {
        // only 1d block
        block new_block(n);
    }
    return new_block;

}

//include the boost program options later on
//first make sure that the compression option can be shown.
int comp_execute(int argc,char *const argv[])
{
    try {
        //!make sure to read up on notifiers before setting this in stone
        //make desc
        po::option_description desc{"Allowed Options"};
        //add options
        desc.add_options()
            //?need to figure out how to pass two int values to the  notifier
            //register gen_block with the create user option
            ("create",po::value<int>()->notifier(&gen_block),"Make a block with these dimensions")
        //create variable map
        po::variables_map vm;
        po::store(po::parse_command_line(argc,argv,desc),vm);
        //finish function registration with notify
        notify(vm);
        //loop over reasonable options
        if(vm.count("create")){
            vm[create].as<int>();// this should launch the gen_block function
        }
        

    }
    catch (po::error &e) {
        std::cerr << e.what()  << std::endl;
    }
    // not quite sure what the return should be in this scenario
    return 0;
}

