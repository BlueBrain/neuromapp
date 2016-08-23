/*
 * Neuromapp - map.h, Copyright (c), 2015,
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
 * @file neuromapp/iobench/backends/map.h
 * iobench Miniapp: map backend
 */

#ifndef MAPP_IOBENCH_MAP_
#define MAPP_IOBENCH_MAP_

#include <omp.h>

#include "basic.h"


/** \fn createDB()
    \brief Create the appropriate DB backend
    \param backend name of the desired DB to create
    \param db std::vector returning the initialized DBs
    \return sakjdhfaspi
 */
inline void createDB (const std::string & backend, std::vector<BaseKV *> & db);


class KVStatusMap : public KVStatus {
    public:
        bool success() { return true; }
};


class MapKV : public BaseKV {

    private:
        typedef std::pair<std::string, size_t> Key;
        typedef std::pair<char *, size_t> Value;
        typedef std::map<Key, Value> IOMap;

        IOMap _map;
        omp_lock_t _mapLock;

        // Use private maps per thread and merge results from time to time
        std::vector<IOMap *> _thrMaps;

    public:
        MapKV() : _map(), _thrMaps(){}
        void initDB(bool compress = true, int nthr = 1, int npairs = 1024, int mpi_rank = -1, int mpi_size = 0);
        inline void putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size);
        inline size_t getKV (KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size);
        inline void waitKVput(std::vector<KVStatus *> &status, int start, int end);
        inline void waitKVget(std::vector<KVStatus *> &status, int start, int end) {}
        void deleteDB();
        std::string getDBName() { return "map"; }

        void createKVStatus(int n, std::vector<KVStatus *> &status);
};

/** \fn void initDB(bool compress, int nthr, int npairs)
    \brief Init the needed data for the specific DB
 */
void MapKV::initDB(bool compress, int nthr, int npairs, int mpi_rank, int mpi_size)
{
    omp_init_lock(&_mapLock);
    for (int i = 0; i < nthr; i++) {
        _thrMaps.push_back(new IOMap());
    }
}

/** \fn void putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
    \brief Insert the given k/v pair into the DB
 */
inline void MapKV::putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
{
    int id = omp_get_thread_num();

    std::string kdata((char *) key, key_size);
    Key k(kdata, key_size);

    char * vdata = new char[value_size];
    std::memcpy(vdata, value, value_size);
    Value v(vdata, value_size);

    _thrMaps[id]->insert(std::pair<Key, Value>(k, v));

    if (_thrMaps[id]->size() > 32 + id) {
        omp_set_lock(&_mapLock);
        _map.insert(_thrMaps[id]->begin(), _thrMaps[id]->end());
        omp_unset_lock(&_mapLock);

        _thrMaps[id]->clear();
    }
}

/** \fn size_t putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
    \brief Retrieve from the DB the associated value to the given key. Returns retrieved value size
 */
inline size_t MapKV::getKV (KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
{
    std::string kdata((char *) key, key_size);
    Key k(kdata, key_size);

    Value v = _map[k];
    size_t str_size = v.second;
    std::memcpy(value, v.first, std::min(value_size, str_size));
    if (str_size != value_size)
        std::cout << "Str size(" << str_size << ") & val size(" << value_size << ") DIFFER!!!" << std::endl;

    return str_size;
}

/** \fn void waitKVput(std::vector<KVStatus *> &status, int start, int end)
    \brief Wait until all the insertions associated to status are committed to the DB
 */
inline void MapKV::waitKVput(std::vector<KVStatus *> &status, int start, int end)
{
    int id = omp_get_thread_num();
    omp_set_lock(&_mapLock);
    _map.insert(_thrMaps[id]->begin(), _thrMaps[id]->end());
    omp_unset_lock(&_mapLock);

    _thrMaps[id]->clear();
}

/** \fn void createKVStatus(int n, std::vector<KVStatus *> &status)
    \brief Clear DB contents
 */
void MapKV::deleteDB()
{
    _map.clear();
}

/** \fn void deleteDB()
    \brief Create the needed structures to handle asynchronous insertions. Opaque class from the outside
 */
void MapKV::createKVStatus(int n, std::vector<KVStatus *> &status)
{
    for (int i = 0; i < n; i++) {
        status.push_back(new KVStatusMap());
    }
}


#endif // MAPP_IOBENCH_MAP_
