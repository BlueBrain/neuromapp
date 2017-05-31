#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define BOOST_TEST_MODULE devin_block_test
#include <boost/test/unit_test.hpp>


#include "compression/allocator.h"
#include "compression/exception.h"
#include "compression/block.h"
using neuromapp::block;
using neuromapp::cstandard;
using std_block = neuromapp::block<int,cstandard>;
using namespace std;
BOOST_AUTO_TEST_CASE( first_test) {
//use raw string literals for easier creation of varied tests for read

    string s1 {R"(4,5
1, 2, 5, 45,
1, 2, 5, 45,
1, 2, 5, 45,
1, 2, 5, 45,
1, 2, 5, 45)"};
    //std::cout << s1 << std::endl;
    stringstream ss1,ss2;
    ss1 << s1;
    std_block b1;
    ss1 >> b1;
    //capture output of block print
    ss2<<b1;
    BOOST_CHECK(ss2.str() == R"(
1 2 5 45
1 2 5 45
1 2 5 45
1 2 5 45
1 2 5 45)");

    //built by file, now by hand for comparison
    std_block b2(4,5);
    for (int i = 0; i < b2.num_rows();i++) {
        //this is the first by hand way of checking this I could think of
        
        int vals[] {1,2,5,45};
        for (int j = 0;j < b2.num_cols(); j++) {
            b2(j,i) = vals[j];
        }
    }
    BOOST_CHECK(b1==b2);
}

