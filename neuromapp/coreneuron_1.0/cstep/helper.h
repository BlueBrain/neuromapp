/*
 * Neuromapp - helper.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Bruno Magalhaes - Swiss Federal Institute of technology in Lausanne,
 * Cremonesi Francesco - Swiss Federal Institute of technology in Lausanne,
 * Sam Yates - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * bruno.magalhaes@epfl.ch
 * francesco.cremonesi@epfl.ch
 * sam.yates@epfl.ch
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
 * @file neuromapp/coreneuron_1.0/cstep/helper.h
 * \brief Implements the helper of the cstep miniapp
 */

#ifndef MAPP_CSTEP_HELPER_
#define MAPP_CSTEP_HELPER_

/** \struct input_parameters
 *  \brief contains the data provides by the user
 */
struct input_parameters{
    /** path to the input data */
    char * d; 
    /** number of OMP thread 
     \warning The default value is 1 OMP thread
     */
    int th;
    /** key for the storage library 
     \warning The default key name is cstep_storage_name_helper
     */
    char * name;
};

/** \fn cstep_print_usage()
    \brief Print the usage of the cstep function
 */
int cstep_print_usage();

/** \fn int cstep_help(int argc, char * const argv[], struct input_parameters * p)
    \brief Interpret the command line and extract/set up the needed parameter
    \param argc The number of argument in the command line
    \param the command line
    \param p the structure where the input data are saved
 */
int cstep_help(int argc, char * const argv[], struct input_parameters * p);

#endif