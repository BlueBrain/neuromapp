/*
 * Neuromapp - spike.h, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
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
 * @file neuromapp/spike/spike.h
 * Spike Exchange Miniapp
 */

#ifndef MAPP_SPIKE_EXECUTE_
#define MAPP_SPIKE_EXECUTE_

/** \fn spike_execute(int argc, char *const argv[])
    \brief Spike Exchange Miniapp
    \param argc number of argument from the command line
    \param argv the command line from the driver or external call
    \return error message from mapp::mapp_error
 */
int spike_execute(int argc, char* const argv[]);

#endif
