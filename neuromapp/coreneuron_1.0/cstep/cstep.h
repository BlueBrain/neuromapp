/*
 * Neuromapp - cstep.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Bruno Magalhaes - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * bruno.magalhaes@epfl.ch>@epfl.ch
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
 * @file neuromapp/coreneuron_1.0/cstep/cstep.h
 * \brief Implements a miniapp combining kernel and solver miniapp
 *
 */

#ifndef MAPP_CSTEP_EXECUTE_
#define MAPP_CSTEP_EXECUTE_

#ifdef __cplusplus
     extern "C" {
#endif
    /** \fn coreneuron10_cstep_execute(int argc, char *const argv[])
        \brief miniapp reproducing a pseudo Neuron step
        \param argc number of argument from the command line
        \param argv the command line from the driver or external call
        \return error message from mapp::mapp_error
    */
     int coreneuron10_cstep_execute(int argc, char *const argv[]);
#ifdef __cplusplus
}
#endif

#endif