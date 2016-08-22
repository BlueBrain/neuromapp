/*
 * neurondistribution.cpp
 *
 *  Created on: Jul 11, 2016
 *      Author: schumann
 */
#include <cassert>
#include <iostream>
#include "coreneuron_1.0/event_passing/environment/neurondistribution.h"

environment::continousdistribution::continousdistribution(size_t groups, size_t me, size_t cells):
        global_number(cells)
{
    //neuron distribution
    const int offset = cells % groups;
    const bool hasonemore = offset > me;
    local_number = cells / groups + hasonemore;

    start = me * local_number;
    if (!hasonemore)
        start += offset;
}

environment::continousdistribution::continousdistribution(size_t groups, size_t me, environment::continousdistribution* parent_distr):
    global_number(parent_distr->getglobalcells())
{
    //neuron distribution
    const int offset = parent_distr->getlocalcells() % groups;
    const bool hasonemore = offset > me;
    local_number = parent_distr->getlocalcells() / groups + hasonemore;

    start = me * local_number + parent_distr->start;
    if (!hasonemore)
        start += offset;
}




