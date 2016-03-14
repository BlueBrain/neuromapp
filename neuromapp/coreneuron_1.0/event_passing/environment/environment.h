/*
 * Neuromapp - environment.h, Copyright (c), 2015,
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
 * @file neuromapp/coreneuron_1.0/environment/environment.h
 * \brief Contains sim_constraints and sim_env class declaration.
 */

#ifndef MAPP_ENVIRONMENT_H
#define MAPP_ENVIRONMENT_H

#include "coreneuron_1.0/event_passing/environment/generator.h"
#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"

namespace environment {

class sim_constraints;

class sim_constraints{
private:
    //simulation variables
    const static int min_delay_ = 5;
    int num_groups_;
    int num_procs_;
    int rank_;
    double sim_time_;
    double time_;

public:
    explicit sim_constraints(int ngroups=1, int nprocs=1, int rank=0, double simtime=0):
    num_groups_(ngroups), num_procs_(nprocs), rank_(rank),
    sim_time_(simtime), time_(0.0) {}

    //GETTERS
    int get_ngroups() const {return num_groups_;}
    int get_nprocs() const {return num_procs_;}
    int get_rank() const {return rank_;}
    double get_simtime() const {return sim_time_;}
    double get_time() const {return time_;}

    //MODIFIERS
    void increment_time() {time_ += 1.0;}
};

class sim_env {
    sim_constraints& constraints_;
    event_generator& generator_;
    presyn_maker& presyns_;


    sim_env(sim_constraints& sc, event_generator& eg, presyn_maker& pm):
    constraints_(sc), generator_(eg), presyns_(pm) {}
};

} //end of namespace
#endif
