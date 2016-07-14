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
#include "coreneuron_1.0/event_passing/environment/neurondistribution.h"

namespace environment {

typedef std::vector<int> presyn;
enum degree {fixedindegree, fixedoutdegree};
/** presyn_maker
 * creates input and output presyns required for spike exchange
 */
class presyn_maker {
private:
    int fan_;
    degree degree_;
    std::map<int, presyn> inputs_;
    std::map<int, presyn> outputs_;
public:
    /** \fn presyn_maker(int ncells, int fanin)
     *  \brief creates the presyn_maker and sets member variables
     *  \param ncells the total number of cells in the simulation
     *  \param fan the number of in/outcoming connections per cell
     */
    explicit presyn_maker(int fan=0, degree fd=fixedindegree):
    fan_(fan), degree_(fd){}

    /** \fn void operator()(int nprocs, int ngroups, int rank)
     *  \brief generates both the input and output presyns.
     *  \param nprocs the number of processes in the simulation
     *  \param ngroups the number of cell groups per process
     *  \param rank the rank of the current process
     */
    void operator()(int rank, neurondistribution* neuron_dist);

//GETTERS

    /** \fn find_input(int id, presyn& ps)
     *  \brief searches for an input presyn(IP) matching the parameter key. If
     *  IP is found, the param presyn is modified to reference the desired IP.
     *  \param key integer key used to find the input presyn
     *  \param ps used to return the matching input presyn val by reference
     *  only valid if find_input returns true.
     *  \return true if matching presyn is found, else false
     */
    const presyn* find_input(int key) const;

    /** \fn find_output(int key, presyn& ps)
     *  \brief searches for an out presyn(OP) matching the parameter key. If
     *  OP is found, the param presyn is modified to reference the desired OP.
     *  \param key the integer used to retrieve the output presyn
     *  \param ps used to return the matching presyn val by reference
     *  only valid if find_output returns true.
     *  \return true if matching presyn is found, else false
     */
    const presyn* find_output(int key) const;
};

} //end of namespace

#endif
