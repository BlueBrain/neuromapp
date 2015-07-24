#pragma once

#include <string>
#include <map>
#include <utility>
#include <typeinfo>
#include <iostream>
#include <stdexcept>

namespace impl {
    struct container {
        void * data_;
        const std::type_info * tid_;
        void (*del_)(void *);

        template < typename T>
        container(T * d, void (*del)(void *) ) : data_( (void *) d ), tid_(&typeid(T)), del_(del) {};
    };
}

struct bad_type_exception: public std::runtime_error {
    explicit bad_type_exception(const std::string &what_str): std::runtime_error(what_str) {}
};

struct missing_data : public std::runtime_error {
    explicit missing_data(const std::string &what_str): std::runtime_error(what_str) {}
};

class storage {
public:
    storage(){};
    ~storage();

    template < typename T >
    T & put_copy(std::string const &name, const T & x);

    template < typename T, class F>
    T & get(std::string const &name, F f);

    template < typename T>
    T& get(const std::string&);

private:
    std::map < std::string, impl::container > M;
};

#include "storage.ipp"
