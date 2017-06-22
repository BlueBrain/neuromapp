/*
 * Neuromapp - adios_block.h, Copyright (c), 2017,
 * Jeremy FOURIAUX - Swiss Federal Institute of technology in Lausanne,
 * jeremy.fouriaux@epfl.ch,
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
 * @file neuromapp/iobench/backends/adios_block.h
 * iobench Miniapp: map backend
 */

#ifndef MAPP_IOBENCH_ADIOS_B_
#define MAPP_IOBENCH_ADIOS_B_

#include <map>
#include <vector>
#include <cstring>

// Get OMP header if available
#include "utils/omp/compatibility.h"
#include "utils/omp/lock.h"
#include "iobench/backends/basic.h"

#include <mpi.h>
#include "utils/mpi/controler.h"


class KVStatusADIOS_B : public KVStatus {
    public:
        int _error;

        bool success() { return _error == MPI_SUCCESS; }

        std::string getErrorString()
        {
            int length = 1024;
            char err[1024];
            MPI_Error_string(_error, err, &length);
            return std::string(err);
        }
};

struct ADIOSFileDescriptor {
        char *       _path;
        MPI_File     _file;
        MPI_Info     _info;
        MPI_Datatype _filetype;

};

class ADIOS_BKV : public BaseKV {

    private:
        // Use a large buffer to avoid caching benefits
        float *           _buffer;
        size_t            _bufferSize;
        unsigned int      _index;
        size_t            _blockSize; // Size of all blocks written by all ranks in 1 iteration
        ADIOSFileDescriptor _fileDesc;

        void openFile(iobench::args *a = NULL);

    public:
        ADIOS_BKV() : _buffer(NULL), _bufferSize(0), _index(0), _blockSize(0), _fileDesc() {}
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


/** \fn void openFile()
    \brief Use MPI to open the file and set the fileview
 */
void ADIOS_BKV::openFile(iobench::args *a)
{
    // args are only needed the first time
    if (_fileDesc._path == NULL && a == NULL) {
        std::cout << "ERROR: ADIOS_BKV::openFile() needs iobench::args the first time is called!" << std::endl;
    }

    if (a != NULL) {
        // Set the needed information in _fileDesc so that the file can be open multiple
        // times without needing args
        std::string output = (a->output().empty()) ? "./test_kv_mpib.bin" : a->output();
        _fileDesc._path = strdup(output.c_str());

        MPI_Info_create(&_fileDesc._info);
        // Use MPI_Info to disable collective buffers if using IME backend
        std::string ime("ime:/");
        if (output.compare(0, ime.length(), ime) == 0) {
            MPI_Info_set (_fileDesc._info, "romio_ds_write", "disable");
            MPI_Info_set (_fileDesc._info, "romio_cb_write", "disable");
        }

        // Set fileview
         int elems = 3; // LB + 1 block of data + UB
         size_t min_size = a->valuesize() * a->npairs();
         _blockSize = min_size * a->procs();
         int lengths[] = {1, min_size, 1};
         MPI_Aint displacements[] = {0, min_size * a->rank(), _blockSize};
         MPI_Datatype data_types[] = {MPI_LB, MPI_BYTE, MPI_UB};

         MPI_Type_struct(elems, &lengths[0], &displacements[0], &data_types[0], &_fileDesc._filetype);
         MPI_Type_commit(&_fileDesc._filetype);

    }

    // Open file
    int error = MPI_File_open(MPI_COMM_WORLD, _fileDesc._path, MPI_MODE_RDWR | MPI_MODE_CREATE, _fileDesc._info, &_fileDesc._file);
    if (error != MPI_SUCCESS) {
        std::cout << "[" << mapp::controler::getInstance().rank() << "] Error opening file: "
                << _fileDesc._path << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 911);
    }

    // Apply the view
    // No header data in mini-app
    MPI_Offset position_to_write = 0;

    char native[10] = "native";
    error = MPI_File_set_view(_fileDesc._file, position_to_write, MPI_FLOAT, _fileDesc._filetype, native, MPI_INFO_NULL);
    if (error != MPI_SUCCESS) {
        std::cout << "[" << mapp::controler::getInstance().rank() << "] Error setting file view" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 913);
    }

    // Index should be reset here
    _index = 0;
}


/** \fn void initDB(iobench::args &a)
    \brief Init the needed data for the specific DB
 */
void ADIOS_BKV::initDB(iobench::args &a)
{
    // Disable OpenMP, as we're using MPI I/O to write
    if (a.threads() > 1) {
        std::cout << "WARNING: iobench with MPI-block backend does not support multi-threading, setting OMP num threads to 1!" << std::endl;
        omp_set_num_threads(1);
        a.threads() = 1;
    }

    // Try to allocate a buffer large enough (first attempt: 1 GB)
    _bufferSize = 1 * 1024 * 1024 * 1024;
    _buffer = (float *) malloc(_bufferSize);

    // If malloc fails (returns 0), try smaller values
    // Minimum size to allocate is the size needed for 1 iteration
    size_t min_size = a.valuesize() * a.npairs();
    while (_buffer == 0) {
        _bufferSize /= 2;
        if (_bufferSize < min_size) {
            _bufferSize = 0;
            break;
        }
        _buffer = (float *) malloc(_bufferSize);
    }

    // Init file descriptor properly and open file
    _fileDesc._path = NULL;
    _fileDesc._file = 0;
    openFile(&a);
}

/** \fn void notifyBuffers(std::vector<char *> &keys, size_t key_size, std::vector<char *> &values, size_t value_size, std::vector<char *> &reads)
    \brief Notify to the DB the buffers where data is generated
 */
void ADIOS_BKV::notifyBuffers(std::vector<char *> &keys, size_t key_size, std::vector<char *> &values, size_t value_size, std::vector<char *> &reads)
{
    // Number of times 'values' vector fits in '_buffer'
    unsigned int times = _bufferSize / (values.size() * value_size);
    // Set _bufferSize accordingly and ignore the remaining part of the buffer
    _bufferSize = times * values.size() * value_size;
    // Index _buffer as a char *
    char * buffer = (char *) _buffer;
    size_t offset = 0;
    for (unsigned int t = 0; t < times; t++) {
        for (unsigned int v = 0; v < values.size(); v++) {
            std::memcpy(&buffer[offset], values[v], value_size);
            offset += value_size;
        }
    }
}

/** \fn void putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
    \brief Insert the given k/v pair into the DB
 */
inline void ADIOS_BKV::putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
{
    // mpi file write at index, jump bsize for next, %buf size
    MPI_Status status;
    ((KVStatusADIOS_B *) kvs)->_error = MPI_File_write_all(_fileDesc._file, &_buffer[_index], value_size / sizeof(float), MPI_FLOAT, &status);
    _index += value_size / sizeof(float);
    if (_index >= _bufferSize) _index = 0;

    //((KVStatusADIOS_B *) kvs)->_error = MPI_File_write_all(_fileDesc._file, value, value_size / sizeof(float), MPI_FLOAT, &status);

}

/** \fn size_t getKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
    \brief Retrieve from the DB the associated value to the given key. Returns retrieved value size
 */
inline size_t ADIOS_BKV::getKV (KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
{
    MPI_Status status;
    ((KVStatusADIOS_B *) kvs)->_error = MPI_File_read_at(_fileDesc._file, _index, value, value_size/sizeof(float), MPI_FLOAT, &status);
    _index += value_size;
    return value_size;
}

/** \fn void waitKVput(std::vector<KVStatus *> &status, int start, int end)
    \brief Wait until all the insertions associated to status are committed to the DB
 */
inline void ADIOS_BKV::waitKVput(std::vector<KVStatus *> &status, int start, int end)
{
    // TODO: Use KVStatus to control the success of this operation?
    // close file to force flushing
    int error = MPI_File_close(&_fileDesc._file);
    if (error != MPI_SUCCESS) {
        std::cout << "[" << mapp::controler::getInstance().rank() << "] Error closing file" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 912);
    }
    _fileDesc._file = 0;
    _index = 0;
}

/** \fn void deleteDB()
    \brief Clear DB contents
 */
void ADIOS_BKV::deleteDB()
{
    free(_buffer);
}

/** \fn void createKVStatus(int n, std::vector<KVStatus *> &status)
    \brief Create the needed structures to handle asynchronous insertions. Opaque class from the outside
 */
void ADIOS_BKV::createKVStatus(int n, std::vector<KVStatus *> &status)
{
    for (int i = 0; i < n; i++) {
        status.push_back(new KVStatusADIOS_B());
    }

    // Calling this function indicates that a write/read operation will follow
    // Since waitKVput() closes the file, make sure it's open again
    if (_fileDesc._file == 0) openFile(NULL);
}


#endif // MAPP_IOBENCH_ADIOS_B_
