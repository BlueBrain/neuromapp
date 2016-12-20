/*
 * Neuromapp - error handling, Copyright (c), 2015,
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
 * @file neuromapp/utils/mpi/error.h
 * \brief MPI error handler
 */


#ifndef MAPP_MPIERR_H
#define MAPP_MPIERR_H

#include <mpi.h>

namespace mapp{
    /** error handling for MPI */
    class MPIError{
    public:
        static inline std::string errorToString (int error) {
            if (error == MPI_SUCCESS) {
                return std::string("Success");
            }
            int length = 1024;
            char err[1024];
            MPI_Error_string(error, err, &length);
            return std::string(err);
        }
    };
} // end namespace
#endif
