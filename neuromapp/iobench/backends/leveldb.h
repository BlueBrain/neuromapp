/*
 * Neuromapp - leveldb.h, Copyright (c), 2015,
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
 * @file neuromapp/iobench/backends/leveldb.h
 * iobench Miniapp: LevelDB backend
 */

#ifndef MAPP_IOBENCH_LDB_
#define MAPP_IOBENCH_LDB_

#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

// Get OMP header if available
#include "utils/omp/compatibility.h"
#include "iobench/backends/basic.h"

// LevelDB headers
#include <leveldb/db.h>
#include <leveldb/cache.h>

/** \fn createDB()
    \brief Create the appropriate DB backend
    \param backend name of the desired DB to create
    \param db std::vector returning the initialized DBs
 */
inline void createDB (const std::string & backend, std::vector<BaseKV *> & db);


class KVStatusLDB : public KVStatus {
    public:
        leveldb::Status _status;

        bool success() { return _status.ok(); }
};


class LDBKV : public BaseKV {

    private:
        leveldb::DB * _ldb;
        std::string   _path;
        bool          _async;

    public:
        LDBKV() : _ldb(NULL), _path(), _async(false) {}
        void initDB(iobench::args &a);
        void finalizeDB() {}
        inline void putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size);
        inline size_t getKV (KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size);
        inline void waitKVput(std::vector<KVStatus *> &status, int start, int end) {}
        inline void waitKVget(std::vector<KVStatus *> &status, int start, int end) {}
        void deleteDB();
        std::string getDBName() { return "leveldb"; }

        void createKVStatus(int n, std::vector<KVStatus *> &status);
};

/** \fn void initDB(iobench::args &a)
    \brief Init the needed data for the specific DB
 */
void LDBKV::initDB(iobench::args &a)
{
    leveldb::Options options;
    options.create_if_missing = true;

    _async = a.async();

    if (!a.compress())
        options.compression = leveldb::kNoCompression;

    if (a.procs() == 1)
        //_path = "/scratch/jplanas/kv/ldb/test";
        _path = "./test";
    else {
        std::stringstream rank;
        rank << a.rank();
        //_path = "/scratch/jplanas/kv/ldb/test_" + rank.str();
        _path = "./test" + rank.str();

    }

    options.write_buffer_size = 512 * 1024 * 1024; // 4 MB default
    leveldb::Cache * cache = leveldb::NewLRUCache(512 * 1024 * 1024); // 8 MB default
    options.block_cache = cache;

    const leveldb::Status status = leveldb::DB::Open(options, _path, &_ldb);

    if (!status.ok()) {
      std::cout << "Error opening store file: " << status.ToString() << std::endl;
    }

    std::cout << "LevelDB start: OK!" << std::endl;
}

/** \fn void putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
    \brief Insert the given k/v pair into the DB
 */
inline void LDBKV::putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
{
    const leveldb::Slice keySlice((const char *) key, key_size);
    const leveldb::Slice valueSlice((const char *) value, value_size);

    leveldb::WriteOptions options;
    options.sync = _async;

    ((KVStatusLDB *) kvs)->_status = _ldb->Put(options, keySlice, valueSlice);
}

/** \fn size_t putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
    \brief Retrieve from the DB the associated value to the given key. Returns retrieved value size
 */
inline size_t LDBKV::getKV (KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
{
    const leveldb::Slice keySlice((const char *) key, key_size);
    std::string ret_str;
    ((KVStatusLDB *) kvs)->_status = _ldb->Get(leveldb::ReadOptions(), keySlice, &ret_str);

    size_t str_size = ret_str.size();
    void * src = (void *) ret_str.c_str();
    std::memcpy(value, src, std::min(value_size, ret_str.size()));
    if (str_size != value_size)
        std::cout << "Str size(" << str_size << ") & val size(" << value_size << ") DIFFER!!!" << std::endl;

    return str_size;
}

/** \fn void deleteDB()
    \brief Clear DB contents
 */
void LDBKV::deleteDB()
{
    DestroyDB(_path, leveldb::Options());
}

/** \fn void createKVStatus(int n, std::vector<KVStatus *> &status)
    \brief Create the needed structures to handle asynchronous insertions. Opaque class from the outside
 */
void LDBKV::createKVStatus(int n, std::vector<KVStatus *> &status)
{
    for (int i = 0; i < n; i++) {
        status.push_back(new KVStatusLDB());
    }
}


#endif // MAPP_IOBENCH_LDB_
