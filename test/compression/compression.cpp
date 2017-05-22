//
//  block.cpp
//  learn_engine
//
//  Created by Tim Ewart on 27/10/2016.
//
//
#include "iostream"

#include <boost/mpl/list.hpp>

#include "block.h"

#define BOOST_TEST_MODULE block

#include <boost/test/unit_test.hpp>

template <class T, class A>
struct shell {
    typedef T value_type;
    typedef A allocator_type;
};

typedef boost::mpl::list<shell<float, neuromapp::cstandard>, shell<float, neuromapp::align>,
                         shell<double, neuromapp::cstandard>, shell<double, neuromapp::align>>
    test_allocator_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(block_constructor, T, test_allocator_types) {
    typedef typename T::value_type value_type;
    typedef typename T::allocator_type allocator_type;
    {
        neuromapp::block<value_type, allocator_type> b;
        BOOST_CHECK_EQUAL(b.num_rows(), 1);
        BOOST_CHECK_EQUAL(b.num_cols(), neuromapp::resize_helper<allocator_type>(1, sizeof(value_type)));
        BOOST_CHECK_EQUAL(b.dim0(), 1);
        BOOST_CHECK_EQUAL(b.dim1(), 1);
    }

    {
        neuromapp::block<value_type, allocator_type> b(150);
        BOOST_CHECK_EQUAL(b.num_rows(), 1);
        BOOST_CHECK_EQUAL(b.num_cols(), neuromapp::resize_helper<allocator_type>(150, sizeof(value_type)));
        BOOST_CHECK_EQUAL(b.dim0(), 150);
        BOOST_CHECK_EQUAL(b.dim1(), 1);
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(block_resize, T, test_allocator_types) {
    typedef typename T::value_type value_type;
    typedef typename T::allocator_type allocator_type;

    neuromapp::block<value_type, allocator_type> b(150);
    BOOST_CHECK_EQUAL(b.num_rows(), 1);
    BOOST_CHECK_EQUAL(b.num_cols(), neuromapp::resize_helper<allocator_type>(150, sizeof(value_type)));
    BOOST_CHECK_EQUAL(b.dim0(), 150);
    BOOST_CHECK_EQUAL(b.dim1(), 1);
    b.resize(300);
    BOOST_CHECK_EQUAL(b.num_rows(), 1);
    BOOST_CHECK_EQUAL(b.num_cols(), neuromapp::resize_helper<allocator_type>(300, sizeof(value_type)));
    BOOST_CHECK_EQUAL(b.dim0(), 300);
    BOOST_CHECK_EQUAL(b.dim1(), 1);
    b.resize(100, 100);
    BOOST_CHECK_EQUAL(b.num_rows(), 100);
    BOOST_CHECK_EQUAL(b.num_cols(), neuromapp::resize_helper<allocator_type>(100, sizeof(value_type)));
    BOOST_CHECK_EQUAL(b.dim0(), 100);
    BOOST_CHECK_EQUAL(b.dim1(), 100);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(move_constructor, T, test_allocator_types) {
    typedef typename T::value_type value_type;
    typedef typename T::allocator_type allocator_type;
    neuromapp::block<value_type, allocator_type> b1;
    neuromapp::block<value_type, allocator_type> b2 = std::move(b1); // calls implicit move ctor

    BOOST_CHECK_EQUAL(b1.num_rows(), 0);
    BOOST_CHECK_EQUAL(b1.num_cols(), 0);
    BOOST_CHECK_EQUAL(b1.dim0(), 0);

    uintptr_t p = reinterpret_cast<uintptr_t>(b1.data());
    BOOST_CHECK_EQUAL(p, 0);

    BOOST_CHECK_EQUAL(b2.num_rows(), 1);
    BOOST_CHECK_EQUAL(b2.dim0(), 1);
    BOOST_CHECK_EQUAL(b2.num_cols(), neuromapp::resize_helper<allocator_type>(1, sizeof(value_type)));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(copy_constructor, T, test_allocator_types) {
    typedef typename T::value_type value_type;
    typedef typename T::allocator_type allocator_type;
    neuromapp::block<value_type, allocator_type> b1;
    neuromapp::block<value_type, allocator_type> b2(b1); // calls implicit move ctor

    BOOST_CHECK_EQUAL(b1.num_rows(), b2.num_rows());
    BOOST_CHECK_EQUAL(b1.num_cols(), b2.num_cols());
    // adress of the block should be different
    BOOST_CHECK_PREDICATE(std::not_equal_to<uintptr_t>(),
                          (reinterpret_cast<uintptr_t>(b1.data()))(reinterpret_cast<uintptr_t>(b2.data())));
    // buffer should be the same
    int r = memcmp(b1.data(), b2.data(), b1.num_rows() * b2.num_rows());
    BOOST_CHECK_EQUAL(r, 0);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(bracket_operator, T, test_allocator_types) {
    typedef typename T::value_type value_type;
    typedef typename T::allocator_type allocator_type;
    neuromapp::block<value_type, allocator_type> b1(10);
    for (int i = 0; i < 10; ++i)
        b1(i) = static_cast<value_type>(i);
    for (int i = 0; i < 10; ++i)
        BOOST_CHECK_CLOSE(b1(i), i, 0.0001);
}
