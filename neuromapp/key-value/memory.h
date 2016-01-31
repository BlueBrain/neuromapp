/*
 * Neuromapp - meta.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/key-value/memory.h
 * \brief Implements the memory for the benchmark neuron like
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
//floating point generator
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
//uuid generator key must be unique
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include "key-value/utils/tools.h"

#include <utility>

namespace keyvalue{

boost::random::uniform_real_distribution<double>  RandomDouble = boost::random::uniform_real_distribution<double>(-75,75);
boost::random::mt19937    rng;
boost::uuids::random_generator generator;

double RandomNumber() {
    return RandomDouble(rng);
}

/** group of cell, simple buffer of memory */
class nrnthread{
public:
    typedef double value_type;
    typedef std::vector<value_type> container_type;

    /** \function nrnthread(std::size_t n = 0)
        \brief allocate the maint container */
    explicit nrnthread(std::size_t n = 0){
        cellgroup.resize(n);
        std::generate(cellgroup.begin(),cellgroup.end(), RandomNumber);
    }

    /** \fun size() const
        \brief get the number of element of the vector */
    inline std::size_t size() const{
        return cellgroup.size();
    }

    /** \fun front_pointer()
        \brief get the front pointer of the main containter */
    inline container_type::const_pointer front_pointer() const{
        assert(cellgroup.size() > 0);
        return &cellgroup.front(); // this is the pointer of the vector
    }
    
private:
    /** container, contiguous buffer i.e. std::vector */
    container_type cellgroup;
};

/** group of group of cell with additional meta data */
template<class M>
class group{
public:
    typedef M meta_type;

    /** \fun group(std::size_t n=0)
        \brief allocate the memory of the group */
    explicit group(std::size_t n=0){
        data.reserve(n);
        gid.reserve(n);
        m.reserve(n);
    }

    /** \fun group(std::size_t n=0)
        \brief push_back a nrnthread group and do the initialisation
        of the uuid and the meta data */
    void push_back(nrnthread const& v){
        std::string uuid = boost::uuids::to_string(generator())
                            + boost::lexical_cast<std::string>(keyvalue::utils::master.rank());
        data.push_back(v);
        gid.push_back(uuid);
        m.push_back(meta_type(uuid,data.back().front_pointer(),v.size()));
    }

    /** \fun meta_at(std::size_t i)
        \brief get a given element of the meta data, read only */
    typename std::vector<meta_type>::const_reference meta_at(std::size_t i) const{
        return m.at(i);
    }

    /** \fun meta_at(std::size_t i)
     \brief get a given element of the meta data, write */
    typename std::vector<meta_type>::reference meta_at(std::size_t i) {
        return m.at(i);
    }
    
private:
    /** the group of cell */
    std::vector<nrnthread> data;
    /** every group of cell is associated to a gid */
    std::vector<std::string> gid;
    /** meta data for the key value, key trace of gid and the memory setting (pointer + size) */
    std::vector<meta_type> m;
};

}
#endif
