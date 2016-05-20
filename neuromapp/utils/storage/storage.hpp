/*
 * Neuromapp - storage.hpp, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Cremonesi Francesco - Swiss Federal Institute of technology in Lausanne,
 * Sam Yates - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * francesco.cremonesi@epfl.ch
 * sam.yates@epfl.ch
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
 * @file neuromapp/utils/storage/storage.hpp
 * \brief implementation of the storage container
 */

#ifndef MAPP_STORAGE_HPP
#define MAPP_STORAGE_HPP

#include <string>
#include <map>
#include <utility>
#include <typeinfo>
#include <iostream>
#include <stdexcept>

//! namespace specific for the storage implementation only
namespace impl {

    template <typename T>
    void default_deleter(void *p) { delete (T *)p; }

    /** \class container Generic type for the storage container (std::map)
        \brief Generic type for the storage container (std::map)
     */
    class container {
    public:
        template <typename T>
        explicit container(T * d, void (*deleter)(void *)=impl::default_deleter<T>):data_((void *)d), tid_(&typeid(T)), del_(deleter) {};

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

        template <typename T>
        const T *get() const {
            if (!data_) throw std::logic_error("impl::container get post-destroy");
            if (typeid(T)!=*tid_) return 0;
            else return (const T *)data_;
        }

    private:
        void *data_;
        const std::type_info * tid_;
        void (*del_)(void *);
    };
}

/** \struct bad_type_exception
    \brief  exception generator for type error in the storage
 */
struct bad_type_exception: public std::runtime_error {
    explicit bad_type_exception(const std::string &what_str): std::runtime_error(what_str) {}
};

/** \struct missing_data 
    \brief  exception generator for missing data in the storage
 */
struct missing_data : public std::runtime_error {
    explicit missing_data(const std::string &what_str): std::runtime_error(what_str) {}
};

/** \class storage 
    \brief store the different data set associated to a giben key provided by the user
 */
class storage {
public:
    storage() {};
    ~storage();

    /** put a copy of a given data set*/
    template <typename T>
    T &put_copy(std::string const &name, const T &x);

    /** get a data set if it does not exist 
        \param name the data set
        \param f functor for the initialisation
     */
    template <typename T, class F>
    T &get(std::string const &name, F f);

    /* Get the data set 
        \param name the data set
     */
    template <typename T>
    T &get(const std::string &name);

    /* Check if the data set exist 
        \param looking a data set
     */
    template <typename T>
    bool has(const std::string &name) const;

    /** clean up */
    void clear(const std::string &name);

private:
    template <typename T>
    T *get_ptr(const std::string &name);
    typedef std::map<std::string, impl::container> storage_map;
    storage_map M;
};

#include "storage.ipp"

#endif
