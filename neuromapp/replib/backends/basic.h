/*
 * Neuromapp - benchmark.h, Copyright (c), 2015,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * judit.planas@epfl.ch,
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
 * GNU General Public License for more details. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

#ifndef MAPP_RL_BASIC_H
#define MAPP_RL_BASIC_H

#include <string.h>
#include <cstring>


#include "replib/utils/config.h"
#include "utils/mpi/timer.h"

namespace replib {

class Writer {

    public:
        Writer() {}
        ~Writer() {}
        virtual void init(replib::config &c) {}
        virtual void open(char * path) = 0;
        virtual void open(mapp::timer &t_io, const std::string &path) {}
        virtual void write(float * buffer, size_t count) = 0;
        virtual void write(mapp::timer &t_io, float * buffer, size_t count) {}
        virtual void close() = 0;
        virtual void close(mapp::timer &t_io) {}
        virtual void finalize() {}

        virtual unsigned int total_bytes() { return 0; }

};

} // end namespace

#endif // MAPP_RL_BASIC_H
