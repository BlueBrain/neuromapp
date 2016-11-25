/*
 * Neuromapp - statistics.h, Copyright (c), 2015,
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

/**
 * @file neuromapp/replib/utils/statistics.h
 * \brief basic shell for statistics
 */

#ifndef MAP_STATISTICS_H
#define MAP_STATISTICS_H

#include <vector>
#include <ostream>

#include "replib/utils/config.h"

namespace replib {

struct bw_stats {
    int rank_;
    unsigned int size_;
    double time_;
    double mbw_;
};

class statistics {
    public:
        /** \fun statistics(replib::config const& conf = replib::config(),
                unsigned int bytes = 0.0, std::vector<double> times = std::vector<double>())
            \brief create the statistics object
         */
        explicit statistics(replib::config const& conf = replib::config(),
                unsigned int bytes = 0, std::vector<double> times = std::vector<double>()) :
                c_(conf), bytes_(bytes), times_(times), g_mbw_(0.), a_mbw_(0.), max_(), min_() {}

        inline unsigned int bytes() const {return bytes_;}
        inline double mbw() const {return g_mbw_;}
        inline double aggr_mbw() const {return a_mbw_;}
        inline const bw_stats& get_max() const { return max_; }
        inline const bw_stats& get_min() const { return min_; }
        void process();
        void print(std::ostream& os) const;

    private:
        replib::config const &c_;
        unsigned int bytes_;
        std::vector<double> times_;
        double g_mbw_;
        double a_mbw_;
        bw_stats max_;
        bw_stats min_;
};

/** \brief basic overload the ostream operator to print the argument */
inline std::ostream &operator<<(std::ostream &out, statistics& s){
    s.print(out);
    return out;
}

} //end namespace

#endif
