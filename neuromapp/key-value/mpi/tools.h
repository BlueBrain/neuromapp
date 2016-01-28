/*
 * Neuromapp - tools.h, Copyright (c), 2015,
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
 * @file neuromapp/key-value/mpi/tools.h
 * \brief basic shell for mpi
 */

#ifndef MAPP_TOOLS_H
#define MAPP_TOOLS_H

#include <mpi.h>

namespace keyvalue {

    class master_mpi{
    private:
        master_mpi(){
            MPI::Init();
            s = MPI::COMM_WORLD.Get_size();
            r = MPI::COMM_WORLD.Get_rank();
        };

        ~master_mpi(){
            MPI::Finalize(); //magic
        }

        master_mpi(master_mpi const& copy); //not implemented
        master_mpi& operator=(master_mpi const& copy); //not implemented

    public:

        static master_mpi& getInstance(){
            static master_mpi instance;
            return instance;
        }

        int size() const {
            return s;
        }

        int rank() const {
            return r;
        }

    private:
        int s;
        int r;
    };
    
    static master_mpi const& master = master_mpi::getInstance();
}

#endif