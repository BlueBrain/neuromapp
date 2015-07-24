//#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE HelloTest
#include "hello/hello.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(hello_test){
    char * const argv[] = {"--name=world","NULL"};
    int argc = 2;
    BOOST_CHECK(hello_execute(argc,argv)==0);
}
