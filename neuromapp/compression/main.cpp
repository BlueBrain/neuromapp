/*
* File: main.cpp aka execute compression app
* Author: Devin Bayly
* Purpose:  this program establishes the command line argument parsing loop
* used for the neuromapp when compression is typed out as option. 
*/

#include <iterator>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
// the includes are relative to the directory above
#include "compression.h"
#include "compression/allocator.h"
#include "compression/exception.h"
#include "compression/block.h"
// include the zlib header.
#include "/usr/include/zlib.h"

/* make namespace alias for program options */
using neuromapp::block;
using neuromapp::cstandard;
namespace po = boost::program_options;
// todo change to variable type, and allocation method
using std_block = block<int,cstandard>;


//todo decide if pointer is right return value 
std_block * gen_block(std::vector<int> vec)
{
    if (vec.size() == 2) {
        std_block * new_block = new std_block(vec[0],vec[1]);// hopefully constructor dishes out enough heap space for this
        return new_block;
    } else if (vec.size() == 1) {
        // only 1d block
        std_block* new_block = new std_block(vec[0]);
        return new_block;
    }

}

//todo include more boost program_options later on
int comp_execute(int argc,char *const argv[])
{
    //prototype for the hello_zlib
    //
    void hello_zlib();
    void enter_data(std_block &,const std::string &);

    //now the program options section
    try {
        //make desc
        po::options_description desc{"Allowed options"};
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
            hello_zlib();
        }
        if (vm.count("help")) {
            std::cout << desc << std::endl;
        }
        if(vm.count("create")){
            //dims as in dimensions for the block, cols and rows, or just cols
            std::vector<int> dims;
            dims = vm["create"].as<std::vector <int>>();// this should launch the gen_block function
            std_block * created_block = gen_block(dims);
            //try entering data from the memvolts file of randomly generated membrane values, use the *created_block to prevent copy argument passing
            enter_data(*created_block,"/home/lil/Documents/BlueBrainProj_study-abroad/gsoc/memvolts.csv");
            // and then try  to print out the block with the print template funciton
            created_block->print();
        }
    }
    catch (po::error &e) {
        std::cerr << e.what()  << std::endl;
    }
    // not quite sure what the return should be in this scenario
    return 0;
}

void hello_zlib() {
    char a[] = "hello world";
    char b[50];
    z_stream def_stream;// this means the struct that will be used for deflation

    //and now setting the initial struct variables
    def_stream.zalloc = Z_NULL;
    def_stream.zfree = Z_NULL;
    def_stream.opaque = Z_NULL;
    def_stream.avail_in = (uInt)strlen(a) + 1;// makes space for the null char?
    def_stream.next_in = (Bytef*) a;// this means the first byte is from array a
    def_stream.next_out = (Bytef*)b;
    deflateInit(&def_stream,Z_DEFAULT_COMPRESSION);// initializes the compression of array a
    deflate(&def_stream,Z_FINISH);// begins compression
    deflateEnd(&def_stream);// frees the structure used for compression

    // method for getting size of compressed array
    printf("Deflated size is :%lu\n",(char*)def_stream.next_out -b );// think about this line
    /* vector<char> file_chars(start,end); */
    /* for_each(file_chars.begin(),file_chars.end(),[](char letter){cout << ((letter == 'e') ? letter : 'X');}); */
}
// this is the tool for adding entries to our block
void enter_data(std_block & empty_block,const std::string & fname)
{
    //could use either the iterator, or the indexing, dunno which is better
    std::ifstream in_file(fname);
    std::cout << fname << std::endl;
    auto block_it = empty_block.begin();
    std::string str_temp {""};
    while(in_file >> str_temp && block_it != empty_block.end()){
        int int_temp {stoi(str_temp)};
        *block_it = int_temp;
        block_it++;
    }
    // close the file
    in_file.close();

}
