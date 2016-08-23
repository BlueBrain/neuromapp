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

#ifndef MAPP_IOBENCH_BASIC_
#define MAPP_IOBENCH_BASIC_

#include <string>


class KVStatus {
    public:
        virtual bool success() { return true; }
};

class BaseKV {

    public:

        virtual void initDB(bool compress = true, int nthr = 1, int npairs = 1024, int mpi_rank = -1, int mpi_size = 0) {}
        virtual inline void putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size) {}
        virtual inline size_t getKV (KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size) { return 0; }
        virtual inline void waitKVput(std::vector<KVStatus *> &status, int start, int end) {}
        virtual inline void waitKVget(std::vector<KVStatus *> &status, int start, int end) {}
        virtual void deleteDB() {}
        virtual void closeDB() {}
        virtual std::string getDBName() { return "none"; }

        virtual void createKVStatus(int n, std::vector<KVStatus *> &status) {}
};

#endif // MAPP_IOBENCH_BASIC_
