/*
 * Neuromapp - mpi_block.h, Copyright (c), 2015,
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
 * @file neuromapp/iobench/backends/mpi_block.h
 * iobench Miniapp: map backend
 */

#ifndef MAPP_IOBENCH_MPIB_
#define MAPP_IOBENCH_MPIB_

#include <map>
#include <vector>
#include <cstring>

// Get OMP header if available
#include "utils/omp/compatibility.h"
#include "utils/omp/lock.h"
#include "iobench/backends/basic.h"

#include <mpi.h>
//#include "utils/mpi/controler.h"


class KVStatusMPIB : public KVStatus {
    public:
        bool success() { return true; }
};


class MPIBKV : public BaseKV {

    private:
        // Use a large buffer to avoid caching benefits
        float *      _buffer;
        size_t       _bufferSize;
        unsigned int _index;
        size_t       _blockSize;
        MPI_File     _file;

    public:
        MPIBKV() : _buffer(), _bufferSize(0), _index(0), _blockSize(0), _file() {}
        void initDB(iobench::args &a);
        void notifyBuffers(std::vector<char *> &keys, size_t key_size, std::vector<char *> &values, size_t value_size, std::vector<char *> &reads);
        void finalizeDB() {}
        inline void putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size);
        inline size_t getKV (KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size);
        inline void waitKVput(std::vector<KVStatus *> &status, int start, int end);
        inline void waitKVget(std::vector<KVStatus *> &status, int start, int end) {}
        void deleteDB();
        std::string getDBName() { return "MPI-block"; }

        void createKVStatus(int n, std::vector<KVStatus *> &status);
};

/** \fn void initDB(iobench::args &a)
    \brief Init the needed data for the specific DB
 */
void MPIBKV::initDB(iobench::args &a)
{
    // Disable OpenMP, as we're using MPI I/O to write
    if (omp_get_num_threads() > 1) {
        std::cout << "WARNING: iobench with MPI-block backend does not support multi-threading, setting OMP num threads to 1!" << std::endl;
        omp_set_num_threads(1);
    }

    // Try to allocate a buffer large enough (first attempt: 1 GB)
    _bufferSize = 1 * 1024 * 1024 * 1024;
    _buffer = (float *) malloc(_bufferSize);

    // If malloc fails (returns 0), try smaller values
    // Minimum size to allocate is the size needed for 1 iteration
    size_t min_size = a_.valuesize() * a_.npairs();
    while (_buffer == 0) {
        _bufferSize /= 2;
        if (_bufferSize < min_size) {
            _bufferSize = 0;
            break;
        }
        _buffer = (float *) malloc(_bufferSize);
    }

    // Open file
    char * report = strdup(a.output().c_str());
    MPI_Info info;
    MPI_Info_create(&info);

    // Use MPI_Info to disable collective buffers if using IME backend
    std::string ime("ime:/");
    if (a.output().compare(0, ime.length(), ime) == 0) {
        MPI_Info_set (info, "romio_ds_write", "disable");
        MPI_Info_set (info, "romio_cb_write", "disable");
    }

    int error = MPI_File_open(MPI_COMM_WORLD, report, MPI_MODE_WRONLY | MPI_MODE_CREATE, info, &_file);
    if (error != MPI_SUCCESS) {
        std::cout << "[" << a.rank() << "] Error opening file: " << report << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 911);
    }

    // Set fileview
    int elems = 3; // LB + 1 block of data + UB
    size_t total_bytes = min_size * a.procs();
    std::vector<int> lengths = {1, min_size, 1};
    std::vector<MPI_Aint> displacements = {0, min_size * a.id(), total_bytes};
    std::vector<MPI_Datatype> data_types = {MPI_LB, MPI_BYTE, MPI_UB};
    MPI_Datatype filetype;

    // Apply the view
    MPI_Type_struct(elems, &lengths[0], &displacements[0], &data_types[0], &filetype);
    MPI_Type_commit(&filetype);

    // No header data in miniapp
    MPI_Offset position_to_write = 0;

    char native[10] = "native";
    error = MPI_File_set_view(_file, position_to_write, MPI_FLOAT, filetype, native, MPI_INFO_NULL);
    if (error != MPI_SUCCESS) {
        std::cout << "[" << a.rank() << "] Error setting file view" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 913);
    }
}

/** \fn void notifyBuffers(std::vector<char *> &keys, size_t key_size, std::vector<char *> &values, size_t value_size, std::vector<char *> &reads)
    \brief Notify to the DB the buffers where data is generated
 */
void MPIBKV::notifyBuffers(std::vector<char *> &keys, size_t key_size, std::vector<char *> &values, size_t value_size, std::vector<char *> &reads)
{
    // Number of times 'values' vector fits in '_buffer'
    unsigned int times = (values.size() * value_size) / _bufferSize;
    // Set _bufferSize accordingly and ignore the remaining part of the buffer
    _bufferSize = times * values.size() * value_size;
    // Index _buffer as a char *
    char * buffer = _buffer;
    size_t offset = 0;
    for (unsigned int t = 0; t < times; t++) {
        for (unsigned int v = 0; v < values.size(); v++) {
            std::memcpy(buffer[offset], values[v], value_size);
            offset += value_size;
        }
    }
}

/** \fn void putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
    \brief Insert the given k/v pair into the DB
 */
inline void MPIBKV::putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
{
    // mpi file write at index, jump bsize for next, %buf size
    MPI_Status status;
    int error = MPI_File_write_all(_file, &_buffer[_index], value_size / sizeof(float), MPI_FLOAT, &status);
    _index += value_size / sizeof(float);
    if (_index >= _bufferSize) _index = 0;
}

/** \fn size_t putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
    \brief Retrieve from the DB the associated value to the given key. Returns retrieved value size
 */
inline size_t MPIBKV::getKV (KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
{
    // ????
    size_t str_size = 0;
    //std::memcpy(value, v.first, std::min(value_size, str_size));
    //if (str_size != value_size)
    //    std::cout << "Str size(" << str_size << ") & val size(" << value_size << ") DIFFER!!!" << std::endl;

    return str_size;
}

/** \fn void waitKVput(std::vector<KVStatus *> &status, int start, int end)
    \brief Wait until all the insertions associated to status are committed to the DB
 */
inline void MPIBKV::waitKVput(std::vector<KVStatus *> &status, int start, int end)
{
    // close file to force flushing
    int error = MPI_File_close(&_file);
    if (error != MPI_SUCCESS) {
        std::cout << "[" << c_.id() << "] Error closing file" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 912);
    }
    _index = 0;
}

/** \fn void deleteDB()
    \brief Clear DB contents
 */
void MPIBKV::deleteDB()
{
    free(_buffer);
}

/** \fn void createKVStatus(int n, std::vector<KVStatus *> &status)
    \brief Create the needed structures to handle asynchronous insertions. Opaque class from the outside
 */
void MPIBKV::createKVStatus(int n, std::vector<KVStatus *> &status)
{
    for (int i = 0; i < n; i++) {
        status.push_back(new KVStatusMPIB());
    }
}


#endif // MAPP_IOBENCH_MPIB_
