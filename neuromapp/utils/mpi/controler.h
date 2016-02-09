/*
 * Neuromapp - controler, Copyright (c), 2015,
 * Sam Yates - Swiss Federal Institute of technology in Lausanne,
 * timothee ewart - Swiss Federal Institute of technology in Lausanne,
 * sam.yates@epfl.ch (work)
 * timothee.ewart@epfl.ch (idea)
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
 * @file neuromapp/utils/mpi/controler.h
 * \brief singleton to startmpi
 */


#ifndef MAPP_CONTROLER_H
#define MAPP_CONTROLER_H

#include <iostream>
#include <mpi.h>

#include "utils/mpi/print.h"

namespace mapp{

    /** brief this singleton to wrap the mpi infos call once and destroy one using a reference */
    class controler{
    private:
        controler(){
            MPI::Init();
            s = MPI::COMM_WORLD.Get_size();
            r = MPI::COMM_WORLD.Get_rank();
            std::cout << mapp::mpi_filter_master(); // MPI print master only move to  mpi_filter_all for all
        };

        controler(controler const& copy); //not implemented
        controler& operator=(controler const& copy); //not implemented

    public:

        ~controler(){
            MPI::Finalize();
        }

        static controler& getInstance(){
            static controler instance;
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

    /** singleton call once at the beginning and destroy at the end automatic nothing to do */
    static controler const& master = controler::getInstance();

} // end namespace
#endif
