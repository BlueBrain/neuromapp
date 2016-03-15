/*
 * Neuromapp - presyn_maker.h, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
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
 * @file neuromapp/coreneuron_1.0/environment/presyn_maker.h
 * \brief Contains presyn_maker class declaration.
 */

#ifndef MAPP_PRESYN_MAKER_H
#define MAPP_PRESYN_MAKER_H

#include <map>

#include "coreneuron_1.0/event_passing/environment/generator.h"

namespace environment {

typedef std::pair<int, std::vector<int> > input_presyn;

class presyn_maker {
private:
    int n_out_;
    int n_in_;
    int nets_per_;
    std::vector<int> outputs_;
    std::map<int, std::vector<int> > inputs_;
public:
    explicit presyn_maker(int out=0, int in=0, int netcons_per=0):
    n_out_(out), n_in_(in), nets_per_(netcons_per) {}

    void operator()(int nprocs, int ngroups, int rank);

    //GETTERS
    //gets from output presyns
    int operator[](int id) const;

    int get_nout() const { return n_out_; }
    int get_nin() const { return n_in_; }

    //gets from input presyns
    bool find_input(int id, input_presyn& presyn) const;
};

} //end of namespace

#endif
