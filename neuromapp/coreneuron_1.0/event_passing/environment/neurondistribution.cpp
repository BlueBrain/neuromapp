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

environment::nestdistribution::nestdistribution(size_t groups, size_t me, size_t cells):
        num_groups(groups), group_id(me), global_number(cells)
{
    //keep it simple
    local_number = 0;
    for (int i=0; i<global_number; i++)
        if (isLocal(i))
            local_number++;

}

environment::nestdistribution::nestdistribution(size_t groups, size_t me, nestdistribution* parent_distr):
        global_number(parent_distr->getglobalcells())
{
    num_groups = parent_distr->num_groups * groups;
    group_id = parent_distr->num_groups * me + parent_distr->group_id;

    //keep it simple
    local_number = 0;
    for (int i=0; i<global_number; i++)
        if (isLocal(i))
            local_number++;

}




