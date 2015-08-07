
#define BOOST_TEST_MODULE StorageTest

#include <boost/test/unit_test.hpp>

#include "coreneuron_1.0/common/util/nrnthread_handler.h"

#include "test/coreneuron_1.0/path.h" // this file is generated automatically
#include "test/coreneuron_1.0/helper.h" // common functionalities

#include "utils/error.h"
#include "utils/storage/storage.h"
#include "utils/storage/storage.hpp"

template <class T>
struct delay {
    T value;
    delay(T x): value(x) {}
    T operator()() const {  return value; }
};

template <class T>
delay<T> make_delay(T x) { return delay<T>(x); }

BOOST_AUTO_TEST_CASE(container_test){
    double a(0.0);
    impl::container c(&a);
    double* b = c.get<double>();
    BOOST_CHECK(*b==0.0);
}

BOOST_AUTO_TEST_CASE(storage_test){
    storage s;
    double a(3.14);
    int b(1);
    a = s.get<double>("double",make_delay(a));
    b = s.get<int>("int",make_delay(b));

    BOOST_CHECK(s.has<double>("double")==true);
    BOOST_CHECK(s.has<int>("int")==true);

    double ra = s.get<double>("double");
    int rb = s.get<int>("int");
    
    BOOST_CHECK(ra==a);
    BOOST_CHECK(rb==b);

    BOOST_CHECK_THROW(s.get<int>("wrongname"), missing_data);
    BOOST_CHECK_THROW(s.get<double>("int"), bad_type_exception);
}

BOOST_AUTO_TEST_CASE(NrnThread_test){
    std::string path(mapp::helper_build_path::test_data_path());
    std::string wrongpath("wrongpath");
    std::string name("name");

    NrnThread * nt = (NrnThread *) storage_get (name.c_str(),make_nrnthread,
                                                (void*)wrongpath.c_str(), dealloc_nrnthread);
    BOOST_CHECK(nt==NULL);
    if(nt == NULL){
        storage_clear(name.c_str());
    }
}