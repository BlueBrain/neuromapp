/*
 * Neuromapp - helper.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Cremonesi Francesco - Swiss Federal Institute of technology in Lausanne,
 * Sam Yates - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * francesco.cremonesi@epfl.ch
 * sam.yates@epfl.ch
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
 * @file neuromapp/coreneuron_1.0/solver/helper.h
 * Implements the helper function of the solver miniapp
 */

#ifndef MAPP_SOLVER_HELPER_
#define MAPP_SOLVER_HELPER_

/** \struct input_parameters
    \brief contains the data provides by the user
 */
struct input_parameters{
    /** data set */
    char * d;
    /** key for the storage */
    char * name;
};

/** \fn cstep_print_usage()
    \brief Print the usage of the cstep function
    \return error code MAPP_USAGE
 */
int solver_print_usage();

/** \fn int solver_help(int argc, char * const argv[], struct input_parameters * p)
    \brief Interpret the command line and extract/set up the needed parameter
    \param argc The number of argument in the command line
    \param the command line
    \param p the structure where the input data are saved
    \return may return error code MAPP_BAD_ARG, MAPP_BAD_DATA if the arguments are wrong
 */
int solver_help(int argc, char* const argv[], struct input_parameters * p);

#endif