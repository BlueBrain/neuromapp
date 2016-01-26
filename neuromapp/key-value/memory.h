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

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>

namespace keyvalue{

static boost::random::uniform_real_distribution<double>  RandomDouble = boost::random::uniform_real_distribution<double>(-75,75);
static boost::random::mt19937    rng;

double RandomNumber() {return RandomDouble(rng);}


class nrnthread{
public:
    explicit nrnthread(std::size_t n = 0){
        cellgroup.resize(n);
        std::generate(cellgroup.begin(),cellgroup.end(), std::rand);
    }

    inline std::size_t size(){
        return cellgroup.size();
    }

private:
    std::vector<double> cellgroup;
};

class group{
public:
    explicit group(std::size_t n=0){
        data.reserve(n);
    }

    void push_back(nrnthread const& v){
        data.push_back(v);
    }

private:
    std::vector<nrnthread> data;
};

}
#endif