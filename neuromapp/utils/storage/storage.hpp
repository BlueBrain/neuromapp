#pragma once

#include <string>
#include <map>
#include <utility>
#include <typeinfo>
#include <iostream>
#include <stdexcept>

namespace impl {
    template <typename T>
    void default_deleter(void *p) { delete (T *)p; }

    class container {
    public:
        template <typename T>
        explicit container(T * d, void (*deleter)(void *)=impl::default_deleter<T>):
            data_((void *)d), tid_(&typeid(T)), del_(deleter) {};

        void destroy() {
            if (!data_) throw std::logic_error("impl::container double destroy");
            del_(data_);
            data_=0;
        }

        template <typename T>
        T *get() {
            if (!data_) throw std::logic_error("impl::container get post-destroy");
            if (typeid(T)!=*tid_) return 0;
            else return (T *)data_;
        }

    private:
        void *data_;
        const std::type_info * tid_;
        void (*del_)(void *);
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
    storage() {};
    ~storage();

    template <typename T>
    T &put_copy(std::string const &name, const T &x);

    template <typename T, class F>
    T &get(std::string const &name, F f);

    template <typename T>
    T &get(const std::string &name);

    template <typename T>
    bool has(const std::string &name) const;

    void clear(const std::string &name);

private:
    template <typename T>
    T *get_ptr(const std::string &name);
    typedef std::map<std::string, impl::container> storage_map;
    storage_map M;
};

#include "storage.ipp"
