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
using neuromapp::cstandard;
namespace po = boost::program_options;
/*mpl typedef area */
typedef boost::mpl::vector<block<float,cstandard>, block<float,align>, block<double,cstandard>, block<double,align>, block<int,cstandard>, block<int,align>> block_list;
// TODO change to variable type, and allocation method
// what type should we default to?
using std_block = block<float,cstandard>;
typedef size_t size_type;
neuromapp::Timer timer;


/* create functor for benchmarking on the block list */
struct bench_block_list {
    template<class Block>
        void operator() (Block & const) {
            vector<std::string> csv_solo_fnames { "../compression/trans_data/values_10_a8214trans_solo.csv", "../compression/trans_data/values_10_a8215trans_solo.csv", "../compression/trans_data/values_10_a8216trans_solo.csv", "../compression/trans_data/values_10_a8217trans_solo.csv", "../compression/trans_data/values_10_a8218trans_solo.csv", "../compression/trans_data/values_10_a8219trans_solo.csv", "../compression/trans_data/values_10_a8220trans_solo.csv", "../compression/trans_data/values_10_a8749trans_solo.csv", "../compression/trans_data/values_10_a8750trans_solo.csv", "../compression/trans_data/values_10_a8751trans_solo.csv", "../compression/trans_data/values_10_a8752trans_solo.csv", "../compression/trans_data/values_10_a8761trans_solo.csv", "../compression/trans_data/values_8_a10249trans_solo.csv", "../compression/trans_data/values_8_a10250trans_solo.csv", "../compression/trans_data/values_8_a10251trans_solo.csv", "../compression/trans_data/values_8_a10252trans_solo.csv", "../compression/trans_data/values_8_a10256trans_solo.csv", "../compression/trans_data/values_8_a10261trans_solo.csv", "../compression/trans_data/values_8_a10262trans_solo.csv", "../compression/trans_data/values_8_a10263trans_solo.csv", "../compression/trans_data/values_8_a10264trans_solo.csv", "../compression/trans_data/values_8_a8780trans_solo.csv", "../compression/trans_data/values_8_a8781trans_solo.csv", "../compression/trans_data/values_8_a8801trans_solo.csv", "../compression/trans_data/values_8_a8802trans_solo.csv", "../compression/trans_data/values_8_a8803trans_solo.csv", "../compression/trans_data/values_8_a8804trans_solo.csv", "../compression/trans_data/values_9_a10237trans_solo.csv", "../compression/trans_data/values_9_a10238trans_solo.csv", "../compression/trans_data/values_9_a10239trans_solo.csv", "../compression/trans_data/values_9_a10240trans_solo.csv", "../compression/trans_data/values_9_a10245trans_solo.csv", "../compression/trans_data/values_9_a10257trans_solo.csv", "../compression/trans_data/values_9_a10258trans_solo.csv", "../compression/trans_data/values_9_a10259trans_solo.csv", "../compression/trans_data/values_9_a10260trans_solo.csv", "../compression/trans_data/values_9_a513trans_solo.csv", "../compression/trans_data/values_9_a514trans_solo.csv", "../compression/trans_data/values_9_a515trans_solo.csv", "../compression/trans_data/values_9_a516trans_solo.csv", "../compression/trans_data/values_9_a8737trans_solo.csv", "../compression/trans_data/values_9_a8738trans_solo.csv", "../compression/trans_data/values_9_a8739trans_solo.csv", "../compression/trans_data/values_9_a8740trans_solo.csv", "../compression/trans_data/values_9_a8782trans_solo.csv", "../compression/trans_data/values_9_a8783trans_solo.csv", "../compression/trans_data/values_9_a8784trans_solo.csv", "../compression/trans_data/values_9_a8785trans_solo.csv", "../compression/trans_data/values_9_a8786trans_solo.csv", "../compression/trans_data/values_9_a8787trans_solo.csv", "../compression/trans_data/values_9_a8788trans_solo.csv", "../compression/trans_data/values_9_a8789trans_solo.csv", "../compression/trans_data/values_9_a8790trans_solo.csv", "../compression/trans_data/values_9_a8791trans_solo.csv", "../compression/trans_data/values_9_a8792trans_solo.csv", "../compression/trans_data/values_9_a8825trans_solo.csv", "../compression/trans_data/values_9_a8826trans_solo.csv", "../compression/trans_data/values_9_a8827trans_solo.csv", "../compression/trans_data/values_9_a8828trans_solo.csv" };

            vector<std::string> csv_bulk_fnames {
                "../compression/trans_data/values_10_a8213trans_bulk.csv", "../compression/trans_data/values_10_a8214trans_bulk.csv", "../compression/trans_data/values_10_a8215trans_bulk.csv", "../compression/trans_data/values_10_a8216trans_bulk.csv", "../compression/trans_data/values_10_a8217trans_bulk.csv", "../compression/trans_data/values_10_a8218trans_bulk.csv", "../compression/trans_data/values_10_a8219trans_bulk.csv", "../compression/trans_data/values_10_a8220trans_bulk.csv", "../compression/trans_data/values_10_a8749trans_bulk.csv", "../compression/trans_data/values_10_a8750trans_bulk.csv", "../compression/trans_data/values_10_a8751trans_bulk.csv", "../compression/trans_data/values_10_a8752trans_bulk.csv", "../compression/trans_data/values_10_a8761trans_bulk.csv", "../compression/trans_data/values_8_a10249trans_bulk.csv", "../compression/trans_data/values_8_a10250trans_bulk.csv", "../compression/trans_data/values_8_a10251trans_bulk.csv", "../compression/trans_data/values_8_a10252trans_bulk.csv", "../compression/trans_data/values_8_a10256trans_bulk.csv", "../compression/trans_data/values_8_a10261trans_bulk.csv", "../compression/trans_data/values_8_a10262trans_bulk.csv", "../compression/trans_data/values_8_a10263trans_bulk.csv", "../compression/trans_data/values_8_a10264trans_bulk.csv", "../compression/trans_data/values_8_a8780trans_bulk.csv", "../compression/trans_data/values_8_a8781trans_bulk.csv", "../compression/trans_data/values_8_a8801trans_bulk.csv", "../compression/trans_data/values_8_a8802trans_bulk.csv", "../compression/trans_data/values_8_a8803trans_bulk.csv", "../compression/trans_data/values_8_a8804trans_bulk.csv", "../compression/trans_data/values_9_a10237trans_bulk.csv", "../compression/trans_data/values_9_a10238trans_bulk.csv", "../compression/trans_data/values_9_a10239trans_bulk.csv", "../compression/trans_data/values_9_a10240trans_bulk.csv", "../compression/trans_data/values_9_a10245trans_bulk.csv", "../compression/trans_data/values_9_a10257trans_bulk.csv", "../compression/trans_data/values_9_a10258trans_bulk.csv", "../compression/trans_data/values_9_a10259trans_bulk.csv", "../compression/trans_data/values_9_a10260trans_bulk.csv", "../compression/trans_data/values_9_a513trans_bulk.csv", "../compression/trans_data/values_9_a514trans_bulk.csv", "../compression/trans_data/values_9_a515trans_bulk.csv", "../compression/trans_data/values_9_a516trans_bulk.csv", "../compression/trans_data/values_9_a8737trans_bulk.csv", "../compression/trans_data/values_9_a8738trans_bulk.csv", "../compression/trans_data/values_9_a8739trans_bulk.csv", "../compression/trans_data/values_9_a8740trans_bulk.csv", "../compression/trans_data/values_9_a8782trans_bulk.csv", "../compression/trans_data/values_9_a8783trans_bulk.csv", "../compression/trans_data/values_9_a8784trans_bulk.csv", "../compression/trans_data/values_9_a8785trans_bulk.csv", "../compression/trans_data/values_9_a8786trans_bulk.csv", "../compression/trans_data/values_9_a8787trans_bulk.csv", "../compression/trans_data/values_9_a8788trans_bulk.csv", "../compression/trans_data/values_9_a8789trans_bulk.csv", "../compression/trans_data/values_9_a8790trans_bulk.csv", "../compression/trans_data/values_9_a8791trans_bulk.csv", "../compression/trans_data/values_9_a8792trans_bulk.csv", "../compression/trans_data/values_9_a8825trans_bulk.csv", "../compression/trans_data/values_9_a8826trans_bulk.csv", "../compression/trans_data/values_9_a8827trans_bulk.csv", "../compression/trans_datavalues_9_a8828trans_bulk.csv" };
            std::ofstream out("benchmark_res.log");
            for (std::string fname : csv_solo_fnames) {
                out << "fname is : " << fname << std::endl;

                std_block block_from_file(std::string);
                //prototype line above
                std_block b1 =  block_from_file(fname);
                for (size_type row = 0 ; row < b1.num_rows() ;row++) {
                    out << "sorting row " << row << "\n";
                    file_routine(b1,out,vm,row);
                }
            }
            for(std::string fname : csv_bulk_fnames) {
                out << "fname is : " << fname << std::endl;
                std_block block_from_file(std::string);
                //prototype line above
                std_block b1 =  block_from_file(fname);
                for (size_type row = 0 ; row < b1.num_rows() ;row++) {
                    out << "sorting row " << row << "\n";
                    file_routine(b1,out,vm,row);
                }
            }
        }

};

//todo include more boost program_options later on
int comp_execute(int argc,char *const argv[])
{
    void file_routine(std_block&,std::ostream &,po::variables_map &,size_type = 0 ) ;
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
        //loop over reasonable options
        if (vm.count("help")) {
            std::cout << desc << std::endl;
        }
        if(vm.count("file") && ! vm.count("benchmark")){
            std::string fname = vm["file"].as<std::string>();
            std_block block_from_file(std::string);
            //prototype line above
            std_block b1 =  block_from_file(fname);
            file_routine(b1,std::cout,vm);
        }
        if(vm.count("benchmark") && ! vm.count("file")) {
            boost::mpl::for_each<block_list>(bench_block_list());
        }
        } catch (po::error &e) {
            std::cerr << e.what()  << std::endl;
        }
        // not quite sure what the return should be in this scenario
        return 0;
    }

    void file_routine( std_block & b1, ostream & os ,po::variables_map & vm,size_type row_sort=0) {
        if(vm.count("split")) {
            timer.start();
            block<unsigned int, cstandard> split_block = neuromapp::generate_split_block(b1);
            timer.end();
            timer.print("splitting took");
            if (vm.count("compress")) {
                timer.start();
                split_block.compress();
                timer.end();
                os <<" SPLTcompressed memory size: " <<  split_block.get_current_size() 
                    << " SPLTstarting memory size: " <<  split_block.memory_allocated()
                    << "SPLTcompression speed: " << timer.duration() ;

                timer.start();
                split_block.uncompress();
                timer.end();
                os << " SPLTuncompressed memory size: " <<  split_block.get_current_size() 
                    << " SPLTstarting memory size: " <<  split_block.memory_allocated()
                    << " SPLTuncompression speed: " <<timer.duration();
                return;
            }
            // renaming b1 will make the rest of the benchmark apply to the split, this output will just be compared to benchmark runs that don't include this 
        } else {
            os << "non split, timing included to balance data 0.0\n";
        }

        if ( vm.count("sort") ) {
            //sort the block before continuing
            //TODO change from hardcoded for 0 row as sort
            neuromapp::col_sort(&b1,row_sort);
        }
        if ( vm.count("compress") ) {
            timer.start();
            b1.compress();
            timer.end();
            //compress into miliseconds
            os << " compressed memory size: " <<  b1.get_current_size() 
                << " starting memory size: " <<  b1.memory_allocated()
                << " compression speed: " <<timer.duration();
            timer.start();
            b1.uncompress();
            timer.end();
            os << " uncompressed memory size: " <<  b1.get_current_size() 
                << " starting memory size: " <<  b1.memory_allocated()
                << " uncompression speed: " << timer.duration();
        }
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


