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
 * @file neuromapp/coreneuron_1.0/kernel/helper.h
 * \brief Implements the helper of the cstep miniapp
 */

#ifndef MAPP_KERNEL_HELPER_
#define MAPP_KERNEL_HELPER_

/** \struct input_parameters
 *  \brief contains the data provides by the user
 */
struct input_parameters{
    /** The looking mechanism 
    \warning The default value is "Na"
    */
    char * m;
    /** The looking function state or current
     \warning The default value is "state"
     */
    char * f;
    /** The data set */
    char * d;
    /** The number of OMP thread 
     \warning The default value is 1 OMP thread
     */
    int th;
    /** key for the storage library
     \warning The default key name is coreneuron_1.0_kernel_data
     */
    int duplicate;
    /** key for the number of duplication
     \warning The default key value is one
     */
    int step;
    /** key for the number of step
     \warning The default key value is one
     */
    char * name;
};

/** \fn cstep_print_usage()
    \brief Print the usage of the cstep function
    \return error code MAPP_USAGE
 */
int kernel_print_usage();

/** \fn int cstep_help(int argc, char * const argv[], struct input_parameters * p)
    \brief Interpret the command line and extract/set up the needed parameter
    \param argc The number of argument in the command line
    \param the command line
    \param p the structure where the input data are saved
    \return may return error code MAPP_BAD_ARG, MAPP_BAD_DATA if the arguments are wrong
*/
int kernel_help(int argc, char * const argv[], struct input_parameters * p);

/** \fn kernel_help_mechanism(const char* m)
    \brief Check if the looking mechanism exists else it return an error code
    \return error code MAPP_BAD_ARG
 */
int kernel_help_mechanism(const char* m);

/** \fn kernel_help_function(const char* f)
    \brief Check if the looking function exists else it return an error code
    \return error code MAPP_BAD_ARG
 */
int kernel_help_function(const char* f);

#endif
