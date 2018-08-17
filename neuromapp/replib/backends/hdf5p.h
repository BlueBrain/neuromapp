/*
 * Neuromapp - benchmark.h, Copyright (c), 2015,
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

//#ifdef RL_HDF5

#ifndef MAPP_RL_H5P_H
#define MAPP_RL_H5P_H

#include <string.h>
#include <cstring>

#include "hdf5.h"

#include "replib/backends/basic.h"
#include "replib/utils/config.h"
#include "replib/utils/fileview.h"
#include "replib/mpiio_dist/rnd1b.h"
#include "replib/mpiio_dist/file1b.h"
#include "replib/mpiio_dist/fileNb.h"

#include "utils/mpi/controler.h"
#include "utils/mpi/timer.h"


namespace replib {

class H5PWriter : public replib::Writer {

    private:
        /* Private attributes, no need for getters/setters */
        size_t numElemsPerTStep_;      // Number of floats per time step
        size_t tStepsPerWrite_;        // Number of time steps written at each operation
        size_t numElemsPerWrite_;      // Number of floats to write at each operation
        size_t globalElemsPerTStep_;   // Total number of floats per time step (aggregated)
        size_t totalSimTSteps_;        // Total number of simulation time steps

        /* The following members are private and exclusive of HDF5,
         * so there is no need to implement getters or setters
         */
        hid_t  fid_;        // File ID
        hid_t  data_ds_id_; // Group ID
        hid_t  memspace_;   // Memspace
        hid_t  filespace_;  // Filespace
        std::vector<hid_t>  filespaces_;  // Filespaces
        int fs_idx_;
        hid_t  cplist_;     // Collective I/O property
        hid_t  iplist_;     // Independent I/O property
        herr_t status_;     // Status error

        hsize_t rankOffset_;    // Offset at which this rank writes wrt the beginning of current time step
        std::vector<hsize_t> dataOffset_;   // Offset at which each rank writes, per dimension
        std::vector<hsize_t> writeDims_;    // Dimensions of the data written by one rank at once
        std::vector<hsize_t> dataDims_;     // Total #elems written in the dataset, per dimension


    public:

        H5PWriter();
        ~H5PWriter();

        void init(replib::config &c);
        void open(char * path);
        void open(mapp::timer &t_io, const std::string &path);
        void write(float * buffer, size_t count);
        void write(mapp::timer &t_io, float * buffer, size_t count);
        void close();
        void close(mapp::timer &t_io);
        unsigned int total_bytes();

};
/** \fun H5PWriter()
    \brief create the object, no special actions required for MPI I/O
 */
H5PWriter::H5PWriter() : numElemsPerTStep_(0), tStepsPerWrite_(0), numElemsPerWrite_(0), globalElemsPerTStep_(0),
        totalSimTSteps_(0), fid_(0), data_ds_id_(0), memspace_(0), filespace_(0), fs_idx_(0), cplist_(0), iplist_(0), status_(0),
        rankOffset_(0) {
    // Create vectors of 2 dimensions
    filespaces_ = std::vector<hid_t>();
    dataOffset_ = std::vector<hsize_t>(2);
    writeDims_ = std::vector<hsize_t>(2);
    dataDims_ = std::vector<hsize_t>(2);

}


/** \fun ~H5PWriter()
    \brief Class destructor */
H5PWriter::~H5PWriter() {}


/** \fun init(const replib::config &c)
    \brief initialize the object using the given configuration */
void H5PWriter::init(replib::config &c) {
    fileview * f;
    if (c.write() == "file1b") {
        f = file1b(c);
    } else if (c.write() == "fileNb") {
        std::cout << "fileNb: UNIMPLEMENTED" << std::endl;
        return;
        //f = fileNb(c);
    } else { // "rnd1b"
        std::cout << "rnd1b: UNIMPLEMENTED" << std::endl;
        return;
        //f = rnd1b(c);
    }

    numElemsPerWrite_ = f->length_at(1) / sizeof(float);
    tStepsPerWrite_ = c.sim_steps();
    numElemsPerTStep_ = numElemsPerWrite_ / tStepsPerWrite_;
    totalSimTSteps_ = c.sim_steps() * c.rep_steps();
    MPI_Allreduce(&numElemsPerTStep_, &globalElemsPerTStep_, 1, MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);

    // This works only for file1b
    MPI_Scan(&numElemsPerTStep_, &rankOffset_, 1, MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
    // MPI_Scan is inclusive, subtract the elements of the current rank to find out the real offset
    rankOffset_ -= numElemsPerTStep_;

    filespaces_.reserve(c.rep_steps());

}


/** \fun open(const char * path)
    \brief Open the file with MPI I/O and set the fileview.
           Inline version to be as fast as possible */
inline void H5PWriter::open(char * report) {
    /* HDF5 File creation */
    // Set up file access property list with parallel I/O access
    MPI_Info info = MPI_INFO_NULL;
    hid_t plist_id = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_fapl_mpio(plist_id, MPI_COMM_WORLD, info);

    // Create a new file collectively
    fid_ = H5Fcreate(report, H5F_ACC_TRUNC, H5P_DEFAULT, plist_id);

    // Release property list identifier
    H5Pclose(plist_id);

    // https://stackoverflow.com/questions/15379399/writing-appending-arrays-of-float-to-the-only-dataset-in-hdf5-file-in-c


    /* Create dataset '/data' */
    // Dataset dimensions
    dataDims_[0] = totalSimTSteps_; //0; //tStepsPerWrite_; --> We'll extend it at each write
    dataDims_[1] = globalElemsPerTStep_;
    int nDims = dataDims_.size();
    std::vector<hsize_t> maxDims = std::vector<hsize_t>(2);
    maxDims[0] = totalSimTSteps_;
    maxDims[1] = dataDims_[1];

    // Dataset chunking
    // The maximum number of elements in a chunk is (2^32)-1
    // The maximum size for any chunk is 4 GB
    std::vector<hsize_t> chDims = std::vector<hsize_t>(2);
    chDims[0] = std::min(totalSimTSteps_, tStepsPerWrite_* 2); // timesteps
    chDims[1] = std::min((size_t) 2000, globalElemsPerTStep_); //numComps; // compartments
    int chnDims = chDims.size();

    // Create dataset with chunking
    hid_t dcpl = H5Pcreate(H5P_DATASET_CREATE);
    H5Pset_layout(dcpl, H5D_CHUNKED);
    // Create the dataspace for the dataset
    hid_t space = H5Screate_simple(nDims, &dataDims_[0], &maxDims[0]);
    hid_t err = H5Pset_chunk(dcpl, chnDims, &chDims[0]);
    // Create the dataset with default properties
    data_ds_id_ = H5Dcreate2(fid_, "/data", H5T_NATIVE_FLOAT, space, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    // Close filespace
    H5Sclose(space);
    H5Pclose(dcpl);

    // Create property lists for collective/independent dataset write
    cplist_ = H5Pcreate(H5P_DATASET_XFER);
    iplist_ = H5Pcreate(H5P_DATASET_XFER);
    H5Pset_dxpl_mpio(cplist_, H5FD_MPIO_COLLECTIVE);
    H5Pset_dxpl_mpio(iplist_, H5FD_MPIO_INDEPENDENT);

    // Create initial vectors of offsets and length counts
    // Setting the appropriate offsets and counts is tricky
    // This presentation was super useful to understand how hyperslabs work:
    // http://www.speedup.ch/workshops/w37_2008/HDF5-Tutorial-PDF/PSI-HDF5-PARALLEL.pdf
    //std::vector<hsize_t> offset = std::vector<hsize_t>(2);
    dataOffset_[0] = 0;
    dataOffset_[1] = rankOffset_;
    //std::vector<hsize_t> writeDims_ = std::vector<hsize_t>(2);
    writeDims_[0] = tStepsPerWrite_; // #rows at a time
    writeDims_[1] = numElemsPerTStep_; // #comps at a time

    /* Create memspace and filespace hyperslabs */
    memspace_ = H5Screate_simple(writeDims_.size(), &writeDims_[0], NULL);
    filespace_ = H5Dget_space(data_ds_id_);


    for (int count = 0; count < filespaces_.capacity(); count++) {
        hid_t filespace = H5Dget_space(data_ds_id_);
        H5Sselect_hyperslab(filespace, H5S_SELECT_SET, &dataOffset_[0], NULL, &writeDims_[0], NULL);
        filespaces_.push_back(filespace);
        dataOffset_[0] += tStepsPerWrite_;
    }
    fs_idx_ = 0;

}

/** \fun open(mapp::timer &t_io, const std::string & path)
    \brief Open the file with MPI I/O and set the fileview.
           Timer will be used to account the time of opening the file and setting the fileview */
void H5PWriter::open(mapp::timer &t_io, const std::string & path) {
    // Get the file path
    char * report = strdup(path.c_str());

    //Open the file
    t_io.tic();
    open(report);
    t_io.toc();

}


/** \fun write(float * buffer, size_t count)
    \brief Write to file. Inline version to be as fast as possible */
inline void H5PWriter::write(float * buffer, size_t count) {
    /* Extend the dataset */
    // We could extend the dataset at each write, but this is not efficient at all!
    //dataDims_[0] += tStepsPerWrite_;
    // dataDims_[1] = globalElemsPerTStep_; --> Value doesn't change
    //H5Dset_extent(data_ds_id_, &dataDims_[0]);



    /** first we discard elements from filespace by retaining only needed ones and then start adding new sub-dimensions */
    //if(writeDims_.empty()) {
    //    H5Sselect_none(filespace_);
    //    H5Sselect_none(memspace_);
    //} else {
    //    H5Sselect_hyperslab(filespace_, H5S_SELECT_SET, &dataOffset_[0], NULL, &writeDims_[0], NULL);
    //}

    /* Write the dataset */
    H5Dwrite(data_ds_id_, H5T_NATIVE_FLOAT, memspace_, filespaces_[fs_idx_], cplist_, (void*) buffer);
    fs_idx_++;



    //dataOffset_[0] += tStepsPerWrite_;
}

/** \fun write(mapp::timer &t_io, float * buffer, size_t count)
    \brief Write to file. Since we need to measure the writing time, this function receives
           the timer that should be used to surround the write call */
void H5PWriter::write(mapp::timer &t_io, float * buffer, size_t count) {
    MPI_Status status;
    // Consider whether t_io should only count time spent in H5Dwrite to compute I/O statistics
    t_io.tic();
    write(buffer, count);
    t_io.toc();
}

/** \fun close()
    \brief Close the file. Inline version to be as fast as possible */
inline void H5PWriter::close() {

    for (int i = 0; i < filespaces_.size(); i++)
        H5Sclose(filespaces_[i]);
    H5Sclose(filespace_);
    H5Sclose(memspace_);

    H5Pclose(cplist_);
    H5Pclose(iplist_);
    H5Dclose(data_ds_id_);
    H5Fclose(fid_);
}

/** \fun close(mapp::timer &t_io)
    \brief Close the file.
           Timer will be used to account the time of closing the file */
void H5PWriter::close(mapp::timer &t_io) {
    t_io.tic();
    close();
    t_io.toc();
}

/** \fun total_bytes()
    \brief Return the total amount of data (in bytes) written at each reporting step */
unsigned int H5PWriter::total_bytes() {
    return numElemsPerWrite_ * sizeof(float);
}

} // end namespace

#endif // MAPP_RL_H5P_H
//#endif // RL_HDF5
