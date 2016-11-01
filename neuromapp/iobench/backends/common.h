/*
 * Neuromapp - common.h, Copyright (c), 2015,
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
 * GNU General Public License for more details..  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/iobench/backends/common.h
 * Common code to initialize iobench Miniapp backend
 */

#ifndef MAPP_IOBENCH_COMMON_
#define MAPP_IOBENCH_COMMON_

#include "iobench/backends/map.h"

#ifdef IO_LDB
#include "iobench/backends/leveldb.h"
#endif

#ifdef IO_CASS
#include "iobench/backends/cassandra.h"
#endif


/** \fn BaseKV * createDB()
    \brief Create the appropriate DB backend
    \param backend name of the desired DB to create
    \return pointer to the created DB
 */
inline BaseKV * createDB (const std::string & backend)
{
    if (backend == "ldb") {
#ifdef IO_LDB
        return reinterpret_cast<BaseKV*>(new LDBKV());
#else
        std::cout << "Error: asked for LevelDB backend, but LevelDB was not found." << std::endl;
#endif
    } else if (backend == "cassandra") {
#ifdef IO_CASS
        return reinterpret_cast<BaseKV*>(new CassKV());
#else
        std::cout << "Error: asked for Cassandra backend, but Cassandra was not found." << std::endl;
#endif
    } else {
        return reinterpret_cast<BaseKV*>(new MapKV());
    }
}

#endif // MAPP_IOBENCH_COMMON_
