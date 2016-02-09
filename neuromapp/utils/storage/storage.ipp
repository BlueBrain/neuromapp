/*
 * Neuromapp - storage.ipp, Copyright (c), 2015,
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
 * @file neuromapp/utils/storage/storage.ipp
 * \brief implementation of the storage container
 */

#include <iostream>

template < typename T >
T &storage::put_copy(std::string const &name, const T &x) {
    storage_map::iterator it = M.find(name);

    T *c=0;
    if (it != M.end()) {
        it->second.destroy();
        try {
            c = new T(x);
            it->second = impl::container(c);
        }
        catch (...) { //case new fails 
            M.erase(it);
            throw;
        }
    }
    else {
        c = new T(x);
        M.insert(std::make_pair(name, impl::container(c)));
    }

    return *c;
}

template <typename T>
T *storage::get_ptr(std::string const &name) {
    storage_map::iterator it = M.find(name);
    if (it==M.end()) return 0;

    T *item=it->second.get<T>();
    if(!item) throw bad_type_exception("type mismatch for item '"+name+"'");
    return item;
}

template <typename T, class F>
T &storage::get(std::string const &name, F make_item) {
    T *item = get_ptr<T>(name);
    if (!item) return put_copy<T>(name, make_item()); // call the functors, storage_ctor_wrapper()
    return *item;
}

template < typename T>
T &storage::get(std::string const & name) {
    T *item = get_ptr<T>(name);
    if (!item) throw missing_data("no entry named '"+name+"'");

    return *item;
}

template <typename T>
bool storage::has(std::string const &name) const {
    storage_map::const_iterator it = M.find(name);
    return it!=M.end() && it->second.get<T>()!=0;
}
