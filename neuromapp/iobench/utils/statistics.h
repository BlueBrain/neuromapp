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
 * @file neuromapp/iobench/utils/statistics.h
 * \brief basic shell for statistics
 */

#ifndef MAP_IOBENCH_STATS_H
#define MAP_IOBENCH_STATS_H

#include <vector>

namespace iobench {

/** \fun void compute_statistics(const std::vector<double> &values, double & avg, double & stddev, double & stderr)
    \brief Compute the statistics: given a vector of double values, compute the average, standard deviation and standard error
 */
void compute_statistics(const std::vector<double> &values, double & avg, double & stddev, double & stderr)
{
    // Average
    int n = values.size();
    avg = std::accumulate(values.begin(), values.end(), 0.0) / n;

    // Std deviation
    stddev = 0.0;
    for (int i = 0; i < n; i++) {
        stddev += (values[i] - avg) * (values[i] - avg);
    }
    stddev = sqrt(1.0 / n * stddev);

    // Std error
    stderr = stddev / sqrt(n);


}


} //end namespace

#endif // MAP_IOBENCH_STATS_H
