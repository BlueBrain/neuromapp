/*
 * Neuromapp - solver.cpp, Copyright (c), 2015,
 * Sam Yates - Swiss Federal Institute of technology in Lausanne,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Sam.Yatest@epfl.ch,
 * timothee.ewart@epfl.ch
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/test/utils/test_storage.cpp
 *  Test the storage class
 */

#define BOOST_TEST_MODULE StorageTest

#include <boost/test/unit_test.hpp>

#include "coreneuron_1.0/common/util/nrnthread_handler.h"

#include "neuromapp/coreneuron_1.0/common/data/path.h" // this file is generated automatically
#include "coreneuron_1.0/common/data/helper.h" // common functionalities

#include "utils/error.h"
#include "utils/storage/storage.h"
#include "utils/storage/storage.hpp"

/** \cond */

template <class T>
struct delay {
    T value;
    delay(T x): value(x) {}
    T operator()() const {  return value; }
};

template <class T>
delay<T> make_delay(T x) { return delay<T>(x); }

struct log_deletes {
    explicit log_deletes(int value_): value(value_) {}
    ~log_deletes() { count+=value; }

    int value;

    static int count;
    static void reset_count() { count=0; }
};

int log_deletes::count=0;
/** \endcond */

BOOST_AUTO_TEST_CASE(container_test){
    double a(0.0);
    impl::container c(&a);
    double* b = c.get<double>();
    BOOST_CHECK(*b==0.0);

    int *x = c.get<int>();
    BOOST_CHECK(x==(int *)0);
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

    log_deletes x(3);
    log_deletes &sx = s.get<log_deletes>("logdel",make_delay(x));

    (void)sx; // Only checking non-const reference access and delete counts.

    log_deletes::reset_count();
    BOOST_CHECK(log_deletes::count==0);

    s.put_copy("logdel",make_delay(100));
    BOOST_CHECK(log_deletes::count==3);
}

static int dealloc_count;
static void *identity(void *p) { return p; }

static void inc_dealloc_count(void *p) {
    ++dealloc_count;
}

BOOST_AUTO_TEST_CASE(storage_test_c_wrapper){
    const char *name="c_wrapper_data";
    storage_clear(name);

    dealloc_count=0;

    int value=3;
    int *v_ptr=(int *)storage_get(name,identity,(void *)&value,inc_dealloc_count);

    BOOST_CHECK(v_ptr==&value);
    BOOST_CHECK(*v_ptr==3);
    BOOST_CHECK(dealloc_count==0);

    storage_clear(name);
    BOOST_CHECK(dealloc_count==1);
}

BOOST_AUTO_TEST_CASE(NrnThread_test){
    std::string path(mapp::helper_build_path::test_data_path());
    std::string wrongpath("wrongpath");
    std::string name("name");

    NrnThread * nt = (NrnThread *) storage_get (name.c_str(),make_nrnthread,
                                                (void*)wrongpath.c_str(), free_nrnthread);
    BOOST_CHECK(nt==NULL);
    if(nt == NULL){
        storage_clear(name.c_str());
    }
}
