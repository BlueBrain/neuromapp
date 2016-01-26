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

namespace keyvalue{

boost::random::uniform_real_distribution<double>  RandomDouble = boost::random::uniform_real_distribution<double>(-75,75);
boost::random::mt19937    rng;
boost::uuids::random_generator generator;

double RandomNumber() {
    return RandomDouble(rng);
}

class nrnthread{
public:
    explicit nrnthread(std::size_t n = 0){
        cellgroup.resize(n);
        std::generate(cellgroup.begin(),cellgroup.end(), RandomNumber);
    }

    inline std::size_t size() const{
        return cellgroup.size();
    }

    std::vector<double>::const_reference front() const{
        return cellgroup.front();
    }

private:
    std::vector<double> cellgroup;
};

template<selector s>
class group{
public:
    typedef typename meta_trait<s>::meta_type meta_type;
    
    explicit group(std::size_t n=0){
        data.reserve(n);
        gid.reserve(n);
        m.reserve(n);
    }

    void push_back(nrnthread const& v){
        std::string uuid = boost::uuids::to_string(generator());
        data.push_back(v);
        gid.push_back(uuid);
        m.push_back(meta_type(uuid,&v.front(),v.size()*sizeof(double)));
    }
    
    typename std::vector<meta_type>::reference meta_at(std::size_t i){
        return m.at(i);
    }

    typename std::vector<meta_type>::const_reference meta_at(std::size_t i) const{
        return m.at(i);
    }
    
private:
    std::vector<nrnthread> data;
    std::vector<std::string> gid;
    std::vector<meta_type> m;
};

}
#endif
