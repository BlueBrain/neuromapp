#define BOOST_TEST_MODULE ArgvDataTest

#include <boost/test/unit_test.hpp>

#include "utils/argv_data.h"

BOOST_AUTO_TEST_CASE(argv_data_empty){
    argv_data A;
    BOOST_CHECK(A.argc()==0);
    BOOST_CHECK(A.argv()[0]==(char *)0);
}

BOOST_AUTO_TEST_CASE(argv_data_values){
    std::string s[4]={"zero","one","two","three"};
    int narg=sizeof(s)/sizeof(s[0]);

    argv_data A(s,s+narg);

    int argc=A.argc();
    char * const *argv=A.argv();

    BOOST_CHECK(argc==narg);
    for (int i=0;i<narg;++i)
        BOOST_CHECK(argv[i]==s[i]);

    BOOST_CHECK(argv[argc]==(char *)0);
}
