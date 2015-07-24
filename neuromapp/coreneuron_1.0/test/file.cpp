#define BOOST_TEST_MODULE FileTest
#include <iostream>
#include <boost/test/unit_test.hpp>

#include "coreneuron_1.0/test/path.h" // this file is generated automatically
#include "coreneuron_1.0/test/helper.hpp"

namespace bfs = ::boost::filesystem;

/** this test checks if the input data exists under .zip format */
BOOST_AUTO_TEST_CASE(touch_zip_test){
    bfs::path p(mapp::path_zip());
    bool b = bfs::exists(p);
    BOOST_CHECK(b);
}

/** this test unzip the input and checks if output is obtained,
    the test unzip the test case for the futur input */
BOOST_AUTO_TEST_CASE(touch_unzip_test){
    bfs::path p(mapp::path_unzip());
    bool b = bfs::exists(p);
    BOOST_CHECK(b);
}
