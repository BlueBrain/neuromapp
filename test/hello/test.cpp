//#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE HelloTest
#include "hello/hello.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(hello_test){
    char arg1[]="--name=world";
    char arg2[]="NULL";
    char * const argv[] = {arg1, arg2};
    int argc = 2;
    BOOST_CHECK(hello_execute(argc,argv)==0);
}
