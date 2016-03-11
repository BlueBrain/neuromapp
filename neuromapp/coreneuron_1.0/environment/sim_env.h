/*
 * Neuromapp - sim_env.h, Copyright (c), 2015,
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
 * @file neuromapp/coreneuron_1.0/environment/sim_env.h
 * \brief Contains sim_env class declaration.
 */

#ifndef MAPP_SIM_ENV_H
#define MAPP_SIM_ENV_H

namespace environment {

struct sim_env{
#ifdef _OPENMP
    mapp::omp_lock lock_;
#else
    mapp::dummy_lock lock_;
#endif

    //simulation variables
    const static int min_delay_ = 5;
    int num_groups_;
    double sim_time_;
    int num_procs_;
    int time_;
    int rank_;

    //CONTAINERS
    std::vector<event> spikein_;
    std::vector<event> spikeout_;
    std::vector<int> nin_;
    std::vector<int> displ_;
};

}
#endif
