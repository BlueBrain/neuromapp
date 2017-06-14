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
#include "compression/block.h"

/* make namespace alias for program options */
using neuromapp::block;
using neuromapp::cstandard;
namespace po = boost::program_options;
// todo change to variable type, and allocation method
// what type should we default to?
using std_block = block<double,cstandard>;

//todo include more boost program_options later on
int comp_execute(int argc,char *const argv[])
{
    void file_routine(std::string,std::ostream &,po::variables_map &) ;
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
            ("benchmark","run all of the files in data directory, create csv with output stats");
        //create variable map
        po::variables_map vm;
        po::store(po::parse_command_line(argc,argv,desc),vm);
        //finish function registration with notify
        po::notify(vm);
        //loop over reasonable options
        if (vm.count("help")) {
            std::cout << desc << std::endl;
        }
        if(vm.count("file") && ! vm.count("benchmark")){
            std::string fname = vm["file"].as<std::string>();
            file_routine(fname,std::cout,vm);
        }
        if(vm.count("benchmark") && ! vm.count("file")) {
            vector<std::string> csv_solo_fnames {  "../compression/data/csv/values_10_a8213solo.csv", "../compression/data/csv/values_10_a8214solo.csv", "../compression/data/csv/values_10_a8215solo.csv", "../compression/data/csv/values_10_a8216solo.csv", "../compression/data/csv/values_10_a8217solo.csv", "../compression/data/csv/values_10_a8218solo.csv", "../compression/data/csv/values_10_a8219solo.csv", "../compression/data/csv/values_10_a8220solo.csv", "../compression/data/csv/values_10_a8749solo.csv", "../compression/data/csv/values_10_a8750solo.csv", "../compression/data/csv/values_10_a8751solo.csv", "../compression/data/csv/values_10_a8752solo.csv", "../compression/data/csv/values_10_a8761solo.csv", "../compression/data/csv/values_8_a10249solo.csv", "../compression/data/csv/values_8_a10250solo.csv", "../compression/data/csv/values_8_a10251solo.csv", "../compression/data/csv/values_8_a10252solo.csv", "../compression/data/csv/values_8_a10256solo.csv", "../compression/data/csv/values_8_a10261solo.csv", "../compression/data/csv/values_8_a10262solo.csv", "../compression/data/csv/values_8_a10263solo.csv", "../compression/data/csv/values_8_a10264solo.csv", "../compression/data/csv/values_8_a8780solo.csv", "../compression/data/csv/values_8_a8781solo.csv", "../compression/data/csv/values_8_a8801solo.csv", "../compression/data/csv/values_8_a8802solo.csv", "../compression/data/csv/values_8_a8803solo.csv", "../compression/data/csv/values_8_a8804solo.csv", "../compression/data/csv/values_9_a10237solo.csv", "../compression/data/csv/values_9_a10238solo.csv"};
            std::ofstream out("benchmark_res.log");
            for (std::string fname : csv_solo_fnames) {
                file_routine(fname,out,vm);
            }
        }
    } catch (po::error &e) {
        std::cerr << e.what()  << std::endl;
    }
    // not quite sure what the return should be in this scenario
    return 0;
}

void file_routine(std::string fname, ostream & os ,po::variables_map & vm) {
    std_block block_from_file(std::string);
    //prototype line above
    std_block b1 =  block_from_file(fname);
    if ( vm.count("sort") ) {
        //sort the block before continuing
        sort(b1.begin(),b1.end());
    }
    if ( vm.count("compress") ) {
        chrono::time_point<chrono::system_clock> start,end;
        start = chrono::system_clock::now();
        b1.compress();
        end = chrono::system_clock::now();
        chrono::duration<double> compress_time = end-start;
        os << " compressed memory size: " <<  b1.get_current_size() 
            << " starting memory size: " <<  b1.memory_allocated()
            << " compression Speed: " << compress_time.count() <<"\n";
        start = chrono::system_clock::now();
        b1.uncompress();
        end = chrono::system_clock::now();
        compress_time = end-start;
        os << " uncompressed memory size: " <<  b1.get_current_size() 
            << " starting memory size: " <<  b1.memory_allocated()
            << " uncompression Speed: " << compress_time.count()<< "\n";
    }
    os << "\nblock is : \n" << b1;
}


std_block block_from_file(std::string fname)
{
    //check file
    neuromapp::check_file(fname);
    std::ifstream i_file(fname);// use the filename given as argument to create block
    std_block file_block;
    i_file >> file_block;
    return file_block;
}


