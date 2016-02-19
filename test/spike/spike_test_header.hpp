/*
 * Miniapp - spike_test_header.hpp, Copyright (c), 2016,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
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
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library.
*/

#ifndef MAPP_SPIKE_TEST_HEADER_HPP
#define MAPP_SPIKE_TEST_HEADER_HPP

#include <boost/mpl/list.hpp>
#include "test/spike/pool_adapter.h"
#include "spike/environment.h"
typedef boost::mpl::list<
                            queueing::pool_adapter,
                            spike::environment
                            > full_test_types;

#endif
