/*
 * Neuromapp - statistic.h, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
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

/**
 * @file neuromapp/keyvalue/utils/statistic.h
 * \brief basic shell for arguments
 */

#ifndef MAP_STATISTIC_H
#define MAP_STATISTIC_H

#include <vector>
#include <ostream>

#include "keyvalue/utils/argument.h"

namespace keyvalue {

class statistic{
    typedef double value_type; // maybe a day the class will be templated
    public:
        explicit statistic(keyvalue::argument const& arg = keyvalue::argument(),
                           std::vector<value_type> vtime = std::vector<value_type>()):
                           a(arg),v(vtime),g_mbw(0.),g_iops(0.){}
    inline double mbw() const {return g_mbw;}
    inline double iops() const {return g_iops;}
    void process();
    void print(std::ostream& os) const;
private:
    keyvalue::argument const & a;
    std::vector<value_type> v;
    double g_mbw, g_iops;
};

/** \brief basic overload the ostream operator to print the arguement */
inline std::ostream &operator<<(std::ostream &out, statistic&  a){
    a.print(out);
    return out;
}

} //end namespace
#endif